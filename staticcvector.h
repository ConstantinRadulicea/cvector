#ifndef __STATICCVECTOR_H__
#define __STATICCVECTOR_H__
#include "cvector.h"

#define STATICCVECTOR_SUCCESS						CVECTOR_SUCCESS
#define STATICCVECTOR_ERROR_INVALID_PARAMETERS		CVECTOR_ERROR_INVALID_PARAMETERS
#define STATICCVECTOR_ERROR_EXEEDED_MAX_CAPACITY	10

#define STATICCVECTOR_DEFAULT(vec) CVECTOR_DEFAULT(vec)

typedef cvector staticcvector;

// Utility function to initialize the staticcvector
int staticcvector_init(staticcvector* _Cvector, void* buf, size_t _NumberOfElementsToReserve, size_t _SizeOfElement);

// Utility function get a pointer to the buffer
void* staticcvector_data(staticcvector* pt);

// Returns the number of elements in the staticcvector
size_t staticcvector_size(staticcvector* pt);

// Returns the maximum number of elements that the staticcvector can hold.
size_t staticcvector_capacity(staticcvector* pt);

// Resizes the container so that it contains ‘n’  dropping the newest elements in the staticcvector.
int staticcvector_resize(staticcvector* pt, size_t _NewSize);

// Utility function get the element at intex x
void* staticcvector_at(staticcvector* pt, size_t _Index);

// Utility function to insert a number of elements at a specific index
int staticcvector_insert(staticcvector* pt, const void* _Elements, size_t _Pos, size_t _Count);

// Assigns a new value to the element, replacing its current contents.
int staticcvector_assign(staticcvector* pt, const void* _Elements, size_t _Pos, size_t _Count);

// Utility function set the element at index _Index
int staticcvector_set(staticcvector* pt, size_t _Index, const void* _Element);

// Removes from the vector single or multiple elements
void staticcvector_erase(staticcvector* pt, size_t _Pos, size_t _Count);

// It is used to pop all the elements from the staticcvector
void staticcvector_clear(staticcvector* pt);

// Utility function to check if the staticcvector is empty or not
int staticcvector_empty(staticcvector* pt);

// Utility function to check if the staticcvector is full or not
int staticcvector_full(staticcvector* pt);

// Utility function to add multiple elements to the end of the staticcvector
int staticcvector_append(staticcvector* pt, const void* _Element, size_t _NumOfElements);

// Utility function to get the first element in the staticcvector
void* staticcvector_front(staticcvector* pt);

// Utility function to get the last element in the staticcvector
void* staticcvector_back(staticcvector* pt);

// Utility function to pop a top element from the staticcvector
void staticcvector_pop_back(staticcvector* pt);

// Utility function to push back an element `x` to the staticcvector
int staticcvector_push_back(staticcvector* pt, const void* _Element);

// Utility function to swap 2 vectors
void staticcvector_swap(staticcvector* _Vector_1, staticcvector* _Vector_2);

// returns total elements shifted
size_t staticcvector_shift_left(staticcvector* pt, size_t start_index, size_t positions_to_shift);

int staticcvector_isvalid(staticcvector* _cvector);


#endif
