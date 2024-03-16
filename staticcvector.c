#include "staticcvector.h"
#include "memswap.h"


// Utility function to initialize the staticcvector
int staticcvector_init(staticcvector* _Cvector, void* buf, size_t _NumberOfElementsToReserve, size_t _SizeOfElement) {
	if (_Cvector == NULL || _SizeOfElement == 0) {
		return STATICCVECTOR_ERROR_INVALID_PARAMETERS;
	}

	if (buf == NULL && _NumberOfElementsToReserve != (size_t)0) {
		return STATICCVECTOR_ERROR_INVALID_PARAMETERS;
	}

	_Cvector->data = buf;
	_Cvector->size = (size_t)0;
	_Cvector->capacity = _NumberOfElementsToReserve;
	_Cvector->size_type = _SizeOfElement;

	return STATICCVECTOR_SUCCESS;
}

// Utility function get a pointer to the buffer
void* staticcvector_data(staticcvector* pt) {
	return cvector_data((cvector*)pt);
}

// Returns the number of elements in the staticcvector
size_t staticcvector_size(staticcvector* pt) {
	return cvector_size((cvector*)pt);
}

// Returns the maximum number of elements that the staticcvector can hold.
size_t staticcvector_capacity(staticcvector* pt) {
	return cvector_capacity((cvector*)pt);
}

// Resizes the container so that it contains ‘n’  dropping the newest elements in the staticcvector.
int staticcvector_resize(staticcvector* pt, size_t _NewSize) {
	if (_NewSize > staticcvector_capacity(pt)) {
		return STATICCVECTOR_ERROR_EXEEDED_MAX_CAPACITY;
	}
	pt->size = _NewSize;
	return STATICCVECTOR_SUCCESS;
}

// Utility function get the element at intex x
void* staticcvector_at(staticcvector* pt, size_t _Index) {
	return cvector_at((cvector*)pt, _Index);
}

// Utility function to insert a number of elements at a specific index
int staticcvector_insert(staticcvector* pt, const void* _Elements, size_t _Pos, size_t _Count) {
	if (staticcvector_size(pt) + _Count > staticcvector_capacity(pt)) {
		return STATICCVECTOR_ERROR_EXEEDED_MAX_CAPACITY;
	}
	return cvector_insert((cvector*)pt, _Elements, _Pos, _Count);
}

// Assigns a new value to the element, replacing its current contents.
int staticcvector_assign(staticcvector* pt, const void* _Elements, size_t _Pos, size_t _Count) {
	if (staticcvector_size(pt) + _Count > staticcvector_capacity(pt)) {
		return STATICCVECTOR_ERROR_EXEEDED_MAX_CAPACITY;
	}
	return cvector_assign((cvector*)pt, _Elements, _Pos, _Count);
}

// Utility function set the element at index _Index
int staticcvector_set(staticcvector* pt, size_t _Index, const void* _Element) {
	return staticcvector_assign(pt, _Element, _Index, (size_t)1);
}

// Removes from the vector single or multiple elements
void staticcvector_erase(staticcvector* pt, size_t _Pos, size_t _Count) {
	cvector_erase((cvector*)pt, _Pos, _Count);
}

// It is used to pop all the elements from the staticcvector
void staticcvector_clear(staticcvector* pt) {
	cvector_clear((cvector*)pt);
}

// Utility function to check if the staticcvector is empty or not
int staticcvector_empty(staticcvector* pt) {
	return cvector_empty((cvector*)pt);
}

// Utility function to check if the staticcvector is full or not
int staticcvector_full(staticcvector* pt) {
	return cvector_full((cvector*)pt);
}

// Utility function to add multiple elements to the end of the staticcvector
int staticcvector_append(staticcvector* pt, const void* _Element, size_t _NumOfElements) {
	if (staticcvector_size(pt) + _NumOfElements > staticcvector_capacity(pt)) {
		return STATICCVECTOR_ERROR_EXEEDED_MAX_CAPACITY;
	}
	return cvector_append((cvector*)pt, _Element, _NumOfElements);
}

// Utility function to get the first element in the staticcvector
void* staticcvector_front(staticcvector* pt) {
	return cvector_front((cvector*)pt);
}

// Utility function to get the last element in the staticcvector
void* staticcvector_back(staticcvector* pt) {
	return cvector_back((cvector*)pt);
}

// Utility function to pop a top element from the staticcvector
void staticcvector_pop_back(staticcvector* pt) {
	cvector_pop_back((cvector*)pt);
}

// Utility function to push back an element `x` to the staticcvector
int staticcvector_push_back(staticcvector* pt, const void* _Element) {
	if (staticcvector_size(pt) + (size_t)1 > staticcvector_capacity(pt)) {
		return STATICCVECTOR_ERROR_EXEEDED_MAX_CAPACITY;
	}
	return cvector_push_back((cvector*)pt, _Element);
}

// Utility function to swap 2 vectors
void staticcvector_swap(staticcvector* _Vector_1, staticcvector* _Vector_2) {
	memswap((void*)_Vector_1, (void*)_Vector_2, sizeof(staticcvector));
}

// returns total elements shifted
size_t staticcvector_shift_left(staticcvector* pt, size_t start_index, size_t positions_to_shift) {
	return cvector_shift_left((cvector*)pt, start_index, positions_to_shift);
}

int staticcvector_isvalid(staticcvector* _cvector) {
	return cvector_isvalid((cvector*)_cvector);
}
