#ifndef __CSTRING_H__
#define __CSTRING_H__

#include "cvector.h"

#define CSTRING_DEFAULT(vec) vec.data=NULL; vec.size=0; vec.capacity=0; vec.size_type=0

typedef cvector cstring;
// if you want to use wchar or other types of chars use cstring_init() setting the parameter _SizeOfChar accordingly to your needs.
// You may cast some functions to your type.


// Utility function to initialize the stack
int cstring_init(cstring* _Cstring, size_t _NumberOfElements, size_t _SizeOfChar);

// Utility function get a pointer to the buffer
char* cstring_data(cstring* pt);

// Returns the number of elements in the cstring
size_t cstring_size(cstring* pt);

// Returns the maximum number of elements that the cstring can hold.
size_t cstring_capacity(cstring* pt);

// Returns the type size.
size_t cstring_size_type(cstring* pt);

// Requests that the cstring capacity be at least enough to contain n elements
int cstring_reserve(cstring* pt, size_t _NumOfElements);

// Resizes the container so that it contains ‘n’  dropping the newest elements in the cstring.
int cstring_resize(cstring* pt, size_t _NewSize);

// Reduces the capacity of the cstring to fit its size and destroys all elements beyond the capacity.
int cstring_shrink_to_fit(cstring* pt);

// Utility function get the element at intex x
char* cstring_at(cvector* pt, size_t _Index);

// Utility function set the element at intex x
int cstring_set(cstring* pt, size_t _Index, const void* _Element);

// Utility function to insert a number of elements at a specific index
int cstring_insert(cstring* pt, const void* _Elements, size_t _Pos, size_t _Count);

// Assigns a new value to the string, replacing its current contents.
int cstring_assign(cstring* pt, const void* _Elements, size_t _Pos, size_t _Count);

// Take as input a 'C type' string and copies it into _dst_string
int cstring_create(cstring* _dst_string, const char* _cstr);

// Removes from the string single or multiple characters
void cstring_erase(cstring* pt, size_t _Pos, size_t _Count);

// It is used to pop all the elements from the cstring
void cstring_clear(cstring* pt);

// Utility function to check if the cstring is empty or not
int cstring_empty(cstring* pt);

// Utility function to check if the cstring is full or not
int cstring_full(cstring* pt);

// Utility function to add multiple elements to the end of the string
int cstring_append(cstring* pt, const char* _Characters, size_t _Strlen);

//// Utility function to add the _Source str to the end of the _Destination str
int cstring_strcat_cstr(cstring* pt, const char* _String);

//// Utility function to add the _Source str to the end of the _Destination str
int cstring_strcat(cstring* _Destination, cstring* _Source);

// Utility function to get the first element in the string
char* cstring_front(cstring* pt);

// Utility function to get the last element in the string
char* cstring_back(cstring* pt);

// Utility function to add an element `x` to the cstring
int cstring_push_back(cstring* pt, const void* _Element);

// Utility function to pop a top element from the cstring
void cstring_pop_back(cstring* pt);

// Utility function to swap 2 vectors
void cstring_swap(cstring* _String_1, cstring* _String_2);

// return a cloned a string
int cstring_clone(cstring* _dst_String, cstring* _src_String);

// Utility function that return the string with a null termination character
char* cstring_cstr(cstring* pt);

// Returns a newly constructed string object with its value initialized to a copy of a substring of this object.
int cstring_substr(cstring* _dst_string, cstring* _src_string, size_t pos, size_t len);

// Function that compare 2 strings and return: 1 (s1 > s2), 0 (s1 == s2), -1 (s1 < s2)
int cstring_compare(cvector* _String_1, cvector* _String_2);

// Searches the string pt for the first occurrence of the sequence specified by its arguments. returns [(size_t)-1] on fail
size_t cstring_find(cstring* pt, const char* _String, size_t pos, size_t _StringLen);

// return the number of matching characters starting from the offset, stops at the first difference
size_t cstring_matching_chars(cstring* _Source, cvector* _String2, size_t _SourceOffset);

int cstring_Base64encode(cstring* _Encoded, const void* _ToEncode, size_t _NunOfBytesToEncode);

int cstring_Base64decode(cstring* _Decoded, const char* _ToDecode, size_t _NunOfBytesToDecode);

// utility to free the memory allocated
void cstring_free(cstring* pt);

#endif