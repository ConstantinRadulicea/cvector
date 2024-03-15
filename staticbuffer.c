#include "staticbuffer.h"
#include "memswap.h"


// Utility function to initialize the staticbuffer
int staticbuffer_init(staticbuffer* _Cvector, void* buf, size_t _NumberOfElementsToReserve) {
	if (_Cvector == NULL) {
		return STATICBUFFER_ERROR_INVALID_PARAMETERS;
	}

	if (buf == NULL && _NumberOfElementsToReserve != (size_t)0) {
		return STATICBUFFER_ERROR_INVALID_PARAMETERS;
	}

	_Cvector->data = buf;
	_Cvector->size = (size_t)0;
	_Cvector->capacity = _NumberOfElementsToReserve;
	_Cvector->size_type = (size_t)1;

	return STATICBUFFER_SUCCESS;
}

// Utility function get a pointer to the buffer
void* staticbuffer_data(staticbuffer* pt) {
	return cvector_data((cvector*)pt);
}

// Returns the number of elements in the staticbuffer
size_t staticbuffer_size(staticbuffer* pt) {
	return cvector_size((cvector*)pt);
}

// Returns the maximum number of elements that the staticbuffer can hold.
size_t staticbuffer_capacity(staticbuffer* pt) {
	return cvector_capacity((cvector*)pt);
}

// Resizes the container so that it contains ‘n’  dropping the newest elements in the staticbuffer.
int staticbuffer_resize(staticbuffer* pt, size_t _NewSize) {
	if (_NewSize > staticbuffer_capacity(pt)) {
		return STATICBUFFER_ERROR_EXEEDED_MAX_CAPACITY;
	}
	pt->size = _NewSize;
	return STATICBUFFER_SUCCESS;
}

// Utility function get the element at intex x
void* staticbuffer_at(staticbuffer* pt, size_t _Index) {
	return cvector_at((cvector*)pt, _Index);
}

// Utility function to insert a number of elements at a specific index
int staticbuffer_insert(staticbuffer* pt, const void* _Elements, size_t _Pos, size_t _Count) {
	if (staticbuffer_size(pt) + _Count > staticbuffer_capacity(pt)) {
		return STATICBUFFER_ERROR_EXEEDED_MAX_CAPACITY;
	}
	return cvector_insert((cvector*)pt, _Elements, _Pos, _Count);
}

// Assigns a new value to the element, replacing its current contents.
int staticbuffer_assign(staticbuffer* pt, const void* _Elements, size_t _Pos, size_t _Count) {
	if (staticbuffer_size(pt) + _Count > staticbuffer_capacity(pt)) {
		return STATICBUFFER_ERROR_EXEEDED_MAX_CAPACITY;
	}
	return cvector_assign((cvector*)pt, _Elements, _Pos, _Count);
}

// Utility function set the element at index _Index
int staticbuffer_set(staticbuffer* pt, size_t _Index, const void* _Element) {
	return staticbuffer_assign(pt, _Element, _Index, (size_t)1);
}

// Removes from the vector single or multiple elements
void staticbuffer_erase(staticbuffer* pt, size_t _Pos, size_t _Count) {
	cvector_erase((cvector*)pt, _Pos, _Count);
}

// It is used to pop all the elements from the staticbuffer
void staticbuffer_clear(staticbuffer* pt) {
	cvector_clear((cvector*)pt);
}

// Utility function to check if the staticbuffer is empty or not
int staticbuffer_empty(staticbuffer* pt) {
	return cvector_empty((cvector*)pt);
}

// Utility function to check if the staticbuffer is full or not
int staticbuffer_full(staticbuffer* pt) {
	return cvector_full((cvector*)pt);
}

// Utility function to add multiple elements to the end of the staticbuffer
int staticbuffer_append(staticbuffer* pt, const void* _Element, size_t _NumOfElements) {
	if (staticbuffer_size(pt) + _NumOfElements > staticbuffer_capacity(pt)) {
		return STATICBUFFER_ERROR_EXEEDED_MAX_CAPACITY;
	}
	return cvector_append((cvector*)pt, _Element, _NumOfElements);
}

// Utility function to get the first element in the staticbuffer
void* staticbuffer_front(staticbuffer* pt) {
	return cvector_front((cvector*)pt);
}

// Utility function to get the last element in the staticbuffer
void* staticbuffer_back(staticbuffer* pt) {
	return cvector_back((cvector*)pt);
}

// Utility function to pop a top element from the staticbuffer
void staticbuffer_pop_back(staticbuffer* pt) {
	cvector_pop_back((cvector*)pt);
}

// Utility function to push back an element `x` to the staticbuffer
int staticbuffer_push_back(staticbuffer* pt, const void* _Element) {
	if (staticbuffer_size(pt) + (size_t)1 > staticbuffer_capacity(pt)) {
		return STATICBUFFER_ERROR_EXEEDED_MAX_CAPACITY;
	}
	return cvector_push_back((cvector*)pt, _Element);
}

// Utility function to swap 2 vectors
void staticbuffer_swap(staticbuffer* _Vector_1, staticbuffer* _Vector_2) {
	memswap((void*)_Vector_1, (void*)_Vector_2, sizeof(staticbuffer));
}

// returns total elements shifted
size_t staticbuffer_shift_left(staticbuffer* pt, size_t start_index, size_t positions_to_shift) {
	return cvector_shift_left((cvector*)pt, start_index, positions_to_shift);
}

int staticbuffer_isvalid(staticbuffer* _cvector) {
	return cvector_isvalid((cvector*)_cvector);
}
