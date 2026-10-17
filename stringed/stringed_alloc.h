// stringed_alloc.h -- StringEd string type.
#pragma once
#include <string>
#include <stdlib.h>
#include <new>

// Strings and map nodes come from the C heap, not the engine allocators.
template<class T> class se_alloc
{
public:
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef T *pointer;
	typedef const T *const_pointer;
	typedef T &reference;
	typedef const T &const_reference;
	typedef T value_type;

	template<class U> struct rebind { typedef se_alloc<U> other; };

	se_alloc() {}
	template<class U> se_alloc( const se_alloc<U> & ) {}

	pointer allocate( size_type n ) { return (pointer)malloc( n * sizeof(T) ); }
	void deallocate( pointer p, size_type n ) { free( p ); }
	void construct( pointer p, const T &val ) { new( p ) T( val ); }
	void destroy( pointer p ) { p->~T(); }
	size_type max_size() const { return (size_type)0x7fffffff; }
	pointer address( reference x ) const { return &x; }
	const_pointer address( const_reference x ) const { return &x; }
	bool operator==( const se_alloc & ) const { return true; }
	bool operator!=( const se_alloc & ) const { return false; }
};
// retail string _Grow/_Tidy call malloc/free (0x4abea0), not operator new
typedef std::basic_string< char, std::char_traits<char>, se_alloc<char> > se_string;
