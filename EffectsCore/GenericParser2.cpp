#include "../qcommon/qcommon.h"

#define MAX_TOKEN_SIZE 1024

class TextPool
{
public:
	TextPool( int initSize = 10240 );
	~TextPool();

	TextPool *GetNext() { return mNext; }
	void SetNext( TextPool *which ) { mNext = which; }
	void *operator new( size_t size ) { return Z_MallocInternal(size); }
	void operator delete( void *ptr ) { Z_FreeInternal(ptr); }

	char *AllocText( char *text, bool addNULL = true, TextPool **poolPtr = 0 );

private:
	char *mPool;
	TextPool *mNext;
	int mSize;
	int mUsed;
};

class GPObject
{
public:
	GPObject( const char *initName );

	const char *GetName() { return mName; }
	GPObject *GetNext() { return mNext; }
	void SetNext( GPObject *which ) { mNext = which; }
	GPObject *GetInOrderNext() { return mInOrderNext; }
	void SetInOrderNext( GPObject *which ) { mInOrderNext = which; }
	void SetInOrderPrevious( GPObject *which ) { mInOrderPrevious = which; }
	void *operator new( size_t size ) { return Z_MallocInternal(size); }
	void operator delete( void *ptr ) { Z_FreeInternal(ptr); }

	bool WriteText( TextPool **textPool, const char *text );

protected:
	const char *mName;
	GPObject *mNext;
	GPObject *mInOrderNext;
	GPObject *mInOrderPrevious;
};

class GPValue : public GPObject
{
public:
	GPValue( const char *initName, const char *initValue = 0 );
	~GPValue();

	GPValue *GetNext() { return (GPValue *)mNext; }
	GPValue *Duplicate( TextPool **textPool = 0 );
	bool IsList();
	const char *GetTopValue();
	void AddValue( const char *newValue, TextPool **textPool = 0 );
	bool Parse( char **dataPtr, TextPool **textPool );
	bool Write( TextPool **textPool, int depth );

private:
	GPObject *mList;
};

class GPGroup : public GPObject
{
public:
	GPGroup( const char *initName = "Top Level", GPGroup *initParent = 0 );
	~GPGroup();

	GPGroup *GetNext() { return (GPGroup *)mNext; }
	int GetNumSubGroups();
	int GetNumPairs();

	void Clean();
	GPGroup *Duplicate( TextPool **textPool = 0, GPGroup *initParent = 0 );

	void SetWriteable( const bool writeable ) { mWriteable = writeable; }

	GPValue *AddPair( const char *name, const char *value, TextPool **textPool = 0 );
	void AddPair( GPValue *newPair );
	GPGroup *AddGroup( const char *name, TextPool **textPool = 0 );
	void AddGroup( GPGroup *newGroup );
	GPGroup *FindSubGroup( const char *name );
	bool Parse( char **dataPtr, TextPool **textPool );
	bool Write( TextPool **textPool, int depth = 0 );

	GPValue *FindPair( const char *key );
	const char *FindPairValue( const char *key, const char *defaultVal = 0 );

private:
	void SortObject( GPObject *object, GPObject **unsortedList, GPObject **sortedList, GPObject **lastObject );

	GPValue *mPairs;
	GPValue *mInOrderPairs;
	GPValue *mCurrentPair;
	GPGroup *mSubGroups;
	GPGroup *mInOrderSubGroups;
	GPGroup *mCurrentSubGroup;
	GPGroup *mParent;
	bool mWriteable;
};

class GenericParser2
{
public:
	GenericParser2();
	~GenericParser2();

	void SetWriteable( const bool writeable ) { mWriteable = writeable; }

	bool Parse( char **dataPtr, bool cleanFirst = true, bool writeable = false );
	void Clean();
	bool Write( TextPool *textPool );

private:
	GPGroup mTopLevel;
	TextPool *mTextPool;
	bool mWriteable;
};

static char token[MAX_TOKEN_SIZE];

static char *GetToken( char **text, bool allowLineBreaks, bool readUntilEOL = false )
{
	char *pointer = *text;
	int length = 0;
	char c = 0;
	bool foundLineBreak;

	token[0] = 0;
	if ( !pointer )
		return token;

	for ( ;; )
	{
		foundLineBreak = false;
		while ( 1 )
		{
			c = *pointer;
			if ( c > ' ' )
				break;
			if ( !c )
			{
				*text = 0;
				return token;
			}
			if ( c == '\n' )
				foundLineBreak = true;
			pointer++;
		}
		if ( foundLineBreak && !allowLineBreaks )
		{
			*text = pointer;
			return token;
		}

		c = *pointer;

		if ( c == '/' && pointer[1] == '/' )
		{
			pointer += 2;
			while ( *pointer && *pointer != '\n' )
				pointer++;
		}
		else if ( c == '/' && pointer[1] == '*' )
		{
			pointer += 2;
			while ( *pointer && ( *pointer != '*' || pointer[1] != '/' ) )
				pointer++;
			if ( *pointer )
				pointer += 2;
		}
		else
		{
			break;
		}
	}

	if ( c == '\"' && !readUntilEOL )
	{
		pointer++;
		while ( 1 )
		{
			c = *pointer++;
			if ( c == '\"' )
				break;
			else if ( !c )
				break;
			else if ( length < MAX_TOKEN_SIZE )
				token[length++] = c;
		}
	}
	else if ( readUntilEOL )
	{
		while ( c != '\n' && c != '\r' )
		{
			if ( c == '/' && ( pointer[1] == '/' || pointer[1] == '*' ) )
				break;
			if ( length < MAX_TOKEN_SIZE )
				token[length++] = c;
			pointer++;
			c = *pointer;
		}
		while ( length && token[length - 1] < ' ' )
			length--;
	}
	else
	{
		while ( c > ' ' )
		{
			if ( length < MAX_TOKEN_SIZE )
				token[length++] = c;
			pointer++;
			c = *pointer;
		}
	}

	if ( token[0] == '\"' )
	{
		length--;
		memmove(token, token + 1, length);
		if ( length && token[length - 1] == '\"' )
			length--;
	}

	if ( length >= MAX_TOKEN_SIZE )
		length = 0;
	token[length] = 0;
	*text = pointer;

	return token;
}

TextPool::TextPool( int initSize ) : mNext(0), mSize(initSize), mUsed(0)
{
	mPool = (char *)Z_MallocInternal(mSize);
}

TextPool::~TextPool()
{
	Z_FreeInternal(mPool);
}

char *TextPool::AllocText( char *text, bool addNULL, TextPool **poolPtr )
{
	int length = strlen(text) + ( addNULL ? 1 : 0 );

	if ( mUsed + length + 1 > mSize )
	{
		if ( poolPtr )
		{
			( *poolPtr )->SetNext(new TextPool(mSize));
			*poolPtr = ( *poolPtr )->GetNext();
			return ( *poolPtr )->AllocText(text, addNULL);
		}
		return 0;
	}

	strcpy(mPool + mUsed, text);
	mUsed += length;
	mPool[mUsed] = 0;

	return mPool + mUsed - length;
}

void CleanTextPool( TextPool *pool )
{
	TextPool *next;

	while ( pool )
	{
		next = pool->GetNext();
		delete pool;
		pool = next;
	}
}

GPObject::GPObject( const char *initName ) : mName(initName), mNext(0), mInOrderNext(0), mInOrderPrevious(0)
{
}

bool GPObject::WriteText( TextPool **textPool, const char *text )
{
	if ( strchr(text, ' ') || !text[0] )
	{
		( *textPool )->AllocText("\"", false, textPool);
		( *textPool )->AllocText((char *)text, false, textPool);
		( *textPool )->AllocText("\"", false, textPool);
	}
	else
	{
		( *textPool )->AllocText((char *)text, false, textPool);
	}

	return true;
}

GPValue::GPValue( const char *initName, const char *initValue ) : GPObject(initName), mList(0)
{
	if ( initValue )
		AddValue(initValue);
}

GPValue::~GPValue()
{
	GPObject *next;

	while ( mList )
	{
		next = mList->GetNext();
		delete mList;
		mList = next;
	}
}

GPValue *GPValue::Duplicate( TextPool **textPool )
{
	GPValue *newValue;
	GPObject *iterator;
	char *name;

	if ( textPool )
		name = ( *textPool )->AllocText((char *)mName, true, textPool);
	else
		name = (char *)mName;

	newValue = new GPValue(name);
	iterator = mList;
	while ( iterator )
	{
		if ( textPool )
			name = ( *textPool )->AllocText((char *)iterator->GetName(), true, textPool);
		else
			name = (char *)iterator->GetName();
		newValue->AddValue(name);
		iterator = iterator->GetNext();
	}

	return newValue;
}

bool GPValue::IsList()
{
	if ( !mList || !mList->GetNext() )
		return false;

	return true;
}

const char *GPValue::GetTopValue()
{
	if ( mList )
		return mList->GetName();

	return 0;
}

void GPValue::AddValue( const char *newValue, TextPool **textPool )
{
	if ( textPool )
		newValue = ( *textPool )->AllocText((char *)newValue, true, textPool);

	if ( mList == 0 )
	{
		mList = new GPObject(newValue);
		mList->SetInOrderNext(mList);
	}
	else
	{
		mList->GetInOrderNext()->SetNext(new GPObject(newValue));
		mList->SetInOrderNext(mList->GetInOrderNext()->GetNext());
	}
}

bool GPValue::Parse( char **dataPtr, TextPool **textPool )
{
	char *token;
	char *value;

	while ( 1 )
	{
		token = GetToken(dataPtr, true, true);

		if ( !token[0] )
			return false;
		else if ( stricmp(token, "]") == 0 )
			break;

		value = ( *textPool )->AllocText(token, true, textPool);
		AddValue(value);
	}

	return true;
}

bool GPValue::Write( TextPool **textPool, int depth )
{
	int i;
	GPObject *next;

	if ( !mList )
		return true;

	for ( i = 0; i < depth; i++ )
		( *textPool )->AllocText("\t", false, textPool);

	WriteText(textPool, mName);

	if ( !mList->GetNext() )
	{
		( *textPool )->AllocText("\t\t", false, textPool);
		mList->WriteText(textPool, mList->GetName());
		( *textPool )->AllocText("\r\n", false, textPool);
	}
	else
	{
		( *textPool )->AllocText("\r\n", false, textPool);

		for ( i = 0; i < depth; i++ )
			( *textPool )->AllocText("\t", false, textPool);
		( *textPool )->AllocText("[\r\n", false, textPool);

		next = mList;
		while ( next )
		{
			for ( i = 0; i < depth + 1; i++ )
				( *textPool )->AllocText("\t", false, textPool);
			mList->WriteText(textPool, next->GetName());
			( *textPool )->AllocText("\r\n", false, textPool);

			next = next->GetNext();
		}

		for ( i = 0; i < depth; i++ )
			( *textPool )->AllocText("\t", false, textPool);
		( *textPool )->AllocText("]\r\n", false, textPool);
	}

	return true;
}

GPGroup::GPGroup( const char *initName, GPGroup *initParent ) :
	GPObject(initName),
	mPairs(0),
	mInOrderPairs(0),
	mCurrentPair(0),
	mSubGroups(0),
	mInOrderSubGroups(0),
	mCurrentSubGroup(0),
	mParent(initParent),
	mWriteable(false)
{
}

int GPGroup::GetNumSubGroups()
{
	int count;
	GPGroup *group;

	count = 0;
	group = mSubGroups;
	do
	{
		count++;
		group = group->GetNext();
	}
	while ( group );

	return count;
}

int GPGroup::GetNumPairs()
{
	int count;
	GPValue *pair;

	count = 0;
	pair = mPairs;
	do
	{
		count++;
		pair = pair->GetNext();
	}
	while ( pair );

	return count;
}

void GPGroup::Clean()
{
	while ( mPairs )
	{
		mCurrentPair = mPairs->GetNext();
		delete mPairs;
		mPairs = mCurrentPair;
	}

	while ( mSubGroups )
	{
		mCurrentSubGroup = mSubGroups->GetNext();
		delete mSubGroups;
		mSubGroups = mCurrentSubGroup;
	}

	mPairs = mInOrderPairs = mCurrentPair = 0;
	mSubGroups = mInOrderSubGroups = mCurrentSubGroup = 0;
	mParent = 0;
	mWriteable = false;
}

GPGroup::~GPGroup()
{
	Clean();
}

GPGroup *GPGroup::Duplicate( TextPool **textPool, GPGroup *initParent )
{
	GPGroup *newGroup;
	GPGroup *subSub;
	GPGroup *newSub;
	GPValue *newPair;
	GPValue *subPair;
	char *name;

	if ( textPool )
		name = ( *textPool )->AllocText((char *)mName, true, textPool);
	else
		name = (char *)mName;

	newGroup = new GPGroup(name);

	subSub = mSubGroups;
	while ( subSub )
	{
		newSub = subSub->Duplicate(textPool, newGroup);
		newGroup->AddGroup(newSub);
		subSub = subSub->GetNext();
	}

	subPair = mPairs;
	while ( subPair )
	{
		newPair = subPair->Duplicate(textPool);
		newGroup->AddPair(newPair);
		subPair = subPair->GetNext();
	}

	return newGroup;
}

void GPGroup::SortObject( GPObject *object, GPObject **unsortedList, GPObject **sortedList, GPObject **lastObject )
{
	GPObject *test;
	GPObject *last;

	if ( !*unsortedList )
	{
		*unsortedList = *sortedList = object;
	}
	else
	{
		( *lastObject )->SetNext(object);

		test = *sortedList;
		last = 0;
		while ( test )
		{
			if ( stricmp(object->GetName(), test->GetName()) < 0 )
				break;

			last = test;
			test = test->GetInOrderNext();
		}

		if ( test )
		{
			test->SetInOrderPrevious(object);
			object->SetInOrderNext(test);
		}
		if ( last )
		{
			last->SetInOrderNext(object);
			object->SetInOrderPrevious(last);
		}
		else
		{
			*sortedList = object;
		}
	}

	*lastObject = object;
}

GPValue *GPGroup::AddPair( const char *name, const char *value, TextPool **textPool )
{
	GPValue *newPair;

	if ( textPool )
	{
		name = ( *textPool )->AllocText((char *)name, true, textPool);
		if ( value )
			value = ( *textPool )->AllocText((char *)value, true, textPool);
	}

	newPair = new GPValue(name, value);

	AddPair(newPair);

	return newPair;
}

void GPGroup::AddPair( GPValue *newPair )
{
	SortObject(newPair, (GPObject **)&mPairs, (GPObject **)&mInOrderPairs, (GPObject **)&mCurrentPair);
}

GPGroup *GPGroup::AddGroup( const char *name, TextPool **textPool )
{
	GPGroup *newGroup;

	if ( textPool )
		name = ( *textPool )->AllocText((char *)name, true, textPool);

	newGroup = new GPGroup(name);

	AddGroup(newGroup);

	return newGroup;
}

void GPGroup::AddGroup( GPGroup *newGroup )
{
	SortObject(newGroup, (GPObject **)&mSubGroups, (GPObject **)&mInOrderSubGroups, (GPObject **)&mCurrentSubGroup);
}

GPGroup *GPGroup::FindSubGroup( const char *name )
{
	GPGroup *group;

	group = mSubGroups;
	while ( group )
	{
		if ( !stricmp(name, group->GetName()) )
			return group;
		group = group->GetNext();
	}
	return NULL;
}

bool GPGroup::Parse( char **dataPtr, TextPool **textPool )
{
	char *token;
	char lastToken[MAX_TOKEN_SIZE];
	GPGroup *newSubGroup;
	GPValue *newPair;

	while ( 1 )
	{
		token = GetToken(dataPtr, true);

		if ( !token[0] )
		{
			if ( mParent )
				return false;
			else
				break;
		}
		else if ( stricmp(token, "}") == 0 )
		{
			break;
		}

		I_strncpyz(lastToken, token, sizeof(lastToken));

		token = GetToken(dataPtr, true, true);
		if ( stricmp(token, "{") == 0 )
		{
			newSubGroup = AddGroup(lastToken, textPool);
			newSubGroup->SetWriteable(mWriteable);
			if ( !newSubGroup->Parse(dataPtr, textPool) )
				return false;
		}
		else if ( stricmp(token, "[") == 0 )
		{
			newPair = AddPair(lastToken, 0, textPool);
			if ( !newPair->Parse(dataPtr, textPool) )
				return false;
		}
		else
		{
			AddPair(lastToken, token, textPool);
		}
	}

	return true;
}

bool GPGroup::Write( TextPool **textPool, int depth )
{
	int i;
	GPValue *mPair = mPairs;
	GPGroup *mSubGroup = mSubGroups;

	if ( depth >= 0 )
	{
		for ( i = 0; i < depth; i++ )
			( *textPool )->AllocText("\t", false, textPool);
		WriteText(textPool, mName);
		( *textPool )->AllocText("\r\n", false, textPool);

		for ( i = 0; i < depth; i++ )
			( *textPool )->AllocText("\t", false, textPool);
		( *textPool )->AllocText("{\r\n", false, textPool);
	}

	while ( mPair )
	{
		mPair->Write(textPool, depth + 1);
		mPair = mPair->GetNext();
	}

	while ( mSubGroup )
	{
		mSubGroup->Write(textPool, depth + 1);
		mSubGroup = mSubGroup->GetNext();
	}

	if ( depth >= 0 )
	{
		for ( i = 0; i < depth; i++ )
			( *textPool )->AllocText("\t", false, textPool);
		( *textPool )->AllocText("}\r\n", false, textPool);
	}

	return true;
}

GPValue *GPGroup::FindPair( const char *key )
{
	GPValue *pair = mPairs;

	while ( pair )
	{
		if ( stricmp(pair->GetName(), key) == 0 )
			return pair;

		pair = pair->GetNext();
	}

	return 0;
}

const char *GPGroup::FindPairValue( const char *key, const char *defaultVal )
{
	GPValue *pair = FindPair(key);

	if ( pair )
		return pair->GetTopValue();

	return defaultVal;
}

GenericParser2::GenericParser2() : mTextPool(0), mWriteable(false)
{
}

GenericParser2::~GenericParser2()
{
	Clean();
}

bool GenericParser2::Parse( char **dataPtr, bool cleanFirst, bool writeable )
{
	TextPool *topPool;

	if ( cleanFirst )
		Clean();

	if ( !mTextPool )
		mTextPool = new TextPool;

	SetWriteable(writeable);
	mTopLevel.SetWriteable(writeable);
	topPool = mTextPool;
	return mTopLevel.Parse(dataPtr, &topPool);
}

void GenericParser2::Clean()
{
	mTopLevel.Clean();

	CleanTextPool(mTextPool);
	mTextPool = 0;
}

bool GenericParser2::Write( TextPool *textPool )
{
	return mTopLevel.Write(&textPool, -1);
}

