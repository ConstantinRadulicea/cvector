#ifndef __STATICBUFFER_H__
#define __STATICBUFFER_H__
#include "cvector.h"

#define STATICBUFFER_SUCCESS					CVECTOR_SUCCESS
#define STATICBUFFER_ERROR_INVALID_PARAMETERS	CVECTOR_ERROR_INVALID_PARAMETERS
#define STATICBUFFER_ERROR_EXEEDED_MAX_CAPACITY 10

#define STATICBUFFER_DEFAULT(vec) vec.data=NULL; vec.size=0; vec.capacity=0; vec.size_type=0

typedef cvector staticbuffer;

// Utility function to initialize the staticbuffer
int staticbuffer_init(staticbuffer* _Cvector, void* buf, size_t _NumberOfElementsToReserve);

// Utility function get a pointer to the buffer
void* staticbuffer_data(staticbuffer* pt);

// Returns the number of elements in the staticbuffer
size_t staticbuffer_size(staticbuffer* pt);

// Returns the maximum number of elements that the staticbuffer can hold.
size_t staticbuffer_capacity(staticbuffer* pt);

// Resizes the container so that it contains ‘n’  dropping the newest elements in the staticbuffer.
int staticbuffer_resize(staticbuffer* pt, size_t _NewSize);

// Utility function get the element at intex x
void* staticbuffer_at(staticbuffer* pt, size_t _Index);

// Utility function to insert a number of elements at a specific index
int staticbuffer_insert(staticbuffer* pt, const void* _Elements, size_t _Pos, size_t _Count);

// Assigns a new value to the element, replacing its current contents.
int staticbuffer_assign(staticbuffer* pt, const void* _Elements, size_t _Pos, size_t _Count);

// Utility function set the element at index _Index
int staticbuffer_set(staticbuffer* pt, size_t _Index, const void* _Element);

// Removes from the vector single or multiple elements
void staticbuffer_erase(staticbuffer* pt, size_t _Pos, size_t _Count);

// It is used to pop all the elements from the staticbuffer
void staticbuffer_clear(staticbuffer* pt);

// Utility function to check if the staticbuffer is empty or not
int staticbuffer_empty(staticbuffer* pt);

// Utility function to check if the staticbuffer is full or not
int staticbuffer_full(staticbuffer* pt);

// Utility function to add multiple elements to the end of the staticbuffer
int staticbuffer_append(staticbuffer* pt, const void* _Element, size_t _NumOfElements);

// Utility function to get the first element in the staticbuffer
void* staticbuffer_front(staticbuffer* pt);

// Utility function to get the last element in the staticbuffer
void* staticbuffer_back(staticbuffer* pt);

// Utility function to pop a top element from the staticbuffer
void staticbuffer_pop_back(staticbuffer* pt);

// Utility function to push back an element `x` to the staticbuffer
int staticbuffer_push_back(staticbuffer* pt, const void* _Element);

// Utility function to swap 2 vectors
void staticbuffer_swap(staticbuffer* _Vector_1, staticbuffer* _Vector_2);

// returns total elements shifted
size_t staticbuffer_shift_left(staticbuffer* pt, size_t start_index, size_t positions_to_shift);

int staticbuffer_isvalid(staticbuffer* _cvector);


#endif
