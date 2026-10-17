#include <stdio.h>

extern "C"
{
size_t FS_FileRead( void *ptr, size_t size, size_t nmemb, FILE *stream );
FILE *FS_FileOpen( const char *filename, const char *mode );
int FS_FileClose( FILE *stream );
int FS_FileSeek( FILE *file, int offset, int whence );
}

size_t FS_FileRead( void *ptr, size_t size, size_t nmemb, FILE *stream )
{
	size_t count;

	count = fread(ptr, size, nmemb, stream);
	return count;
}

size_t FS_FileWrite( const void *ptr, size_t size, size_t nmemb, FILE *stream )
{
	return fwrite(ptr, size, nmemb, stream);
}

FILE *FS_FileOpen( const char *filename, const char *mode )
{
	FILE *file;

	file = fopen(filename, mode);
	return file;
}

int FS_FileClose( FILE *stream )
{
	return fclose(stream);
}

int FS_FileSeek( FILE *file, int offset, int whence )
{
	int result;

	result = fseek(file, offset, whence);
	return result;
}
