#ifndef _SORT_H_
#define _SORT_H_

#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#define _USE_MATH_DEFINES
#include <math.h>

#ifndef MIN(X, Y)
	#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#endif // !MIN

#ifndef MAX(X, Y)
	#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#endif // !MAX

//typedef enum bool{ false = 0, true = 1 };

/**************************************************************************************************************************/

/**************************************************************************************************************************/

// returns characters written excluding string termination character
//static int sprintFloat(char* str, float num, int32_t precision) {
//	uint32_t prec = 1;
//	for (; precision > 0; precision--) {
//		prec *= 10;
//	}
//	int32_t val_int = (int32_t)num;   // compute the integer part of the float
//	float val_float = (abs(num) - abs(val_int)) * prec;
//	int32_t val_fra = (int32_t)val_float;
//	return sprintf(str, "%ld.%ld", val_int, val_fra); //
//}
/**************************************************************************************************************************/

// _int_num_bits must be 2^n
// works only on Little-Endian architecture
// Return:
//	0: success
//	1: failure
static int abs_int(void* _int, size_t _int_num_bits) {
	intmax_t temp;

	if (_int_num_bits % 8) return EXIT_FAILURE;

	_int_num_bits = (_int_num_bits / 8);	// num of bits into num of bytes
	if (((unsigned char*)_int)[_int_num_bits - 1] >> 7 & 1) {	// if sign bit is 1 then it is a negative number
		temp = 1;
		for (int i = 0; i < _int_num_bits; i++)
		{
			//intmax_t ff = (unsigned char)(~((unsigned char*)_int)[i]);
			temp = (unsigned char)(~((unsigned char*)_int)[i]) + temp;
			((unsigned char*)_int)[i] = (unsigned char)temp;
			//ff = (unsigned char)(((unsigned char*)_int)[i]);
			temp = temp >> 8;
		}
	}
	return EXIT_SUCCESS;
	//printf("%d\n", (*(unsigned char*)_int) >> 7);
}
/**************************************************************************************************************************/

static int big_int_sum(void* _result, void* _int1, void* _int2, size_t _int_num_bits) {
	intmax_t temp;

	if (_int_num_bits % 8) return EXIT_FAILURE;

	_int_num_bits = (_int_num_bits / 8);	// num of bits into num of bytes


	return EXIT_SUCCESS;
}

/**************************************************************************************************************************/
// A little-endian system stores the least significant byte of a word at the
// smallest memory address and the most significant byte at the largest
// Return value:
//		1: the architecture is Little-Endian
//		0: the architecture is Big-Endian
static int isLittleEndian(void) {
	unsigned int i = 1;
	char* c = (char*)&i;

	if (*c) {
		return 1;
	}
	else {
		return 0;
	}
}

// A big-endian system stores the most significant byte of a word at the
// smallest memory address and the least significant byte at the largest
// Return value:
//		1: the architecture is Big-Endian
//		0: the architecture is Little-Endian
static int isBigEndian(void) {
	unsigned int i = 1;
	char* c = ((char*)&i) + sizeof(unsigned int) - 1;

	if (*c) {
		return 1;
	}
	else {
		return 0;
	}
}

/**************************************************************************************************************************/
// swap 2 blocks of memory
static void memswap(void* _a, void* _b, uintmax_t _size) {
	uintmax_t temp;
	while (_size >= sizeof(uintmax_t))		// multiple of uintmax_t
	{
		temp = *((uintmax_t*)_a);
		*((uintmax_t*)_a) = *((uintmax_t*)_b);
		*((uintmax_t*)_b) = temp;

		_a = ((uintmax_t*)_a) + 1;
		_b = ((uintmax_t*)_b) + 1;
		_size -= sizeof(uintmax_t);
	}
#if UINTMAX_MAX > UINT64_MAX
	if (_size >= sizeof(uint64_t))
	{
		temp = *((uint64_t*)_a);
		*((uint64_t*)_a) = *((uint64_t*)_b);
		*((uint64_t*)_b) = temp;

		_a = ((uint64_t*)_a) + 1;
		_b = ((uint64_t*)_b) + 1;
		_size -= sizeof(uint64_t);
	}
#endif
#if UINTMAX_MAX > UINT32_MAX
	if (_size >= sizeof(uint32_t))
	{
		temp = *((uint32_t*)_a);
		*((uint32_t*)_a) = *((uint32_t*)_b);
		*((uint32_t*)_b) = temp;

		_a = ((uint32_t*)_a) + 1;
		_b = ((uint32_t*)_b) + 1;
		_size -= sizeof(uint32_t);
	}
#endif
#if UINTMAX_MAX > UINT16_MAX
	if (_size >= sizeof(uint16_t))
	{
		temp = *((uint16_t*)_a);
		*((uint16_t*)_a) = *((uint16_t*)_b);
		*((uint16_t*)_b) = temp;

		_a = ((uint16_t*)_a) + 1;
		_b = ((uint16_t*)_b) + 1;
		_size -= sizeof(uint16_t);
	}
#endif
#if UINTMAX_MAX > UINT8_MAX
	while (_size >= sizeof(uint8_t))	// in case at this point _size > 1, we will use a loop to copy remaining bytes
	{
		temp = *((uint8_t*)_a);
		*((uint8_t*)_a) = *((uint8_t*)_b);
		*((uint8_t*)_b) = temp;

		_a = ((uint8_t*)_a) + 1;
		_b = ((uint8_t*)_b) + 1;
		_size -= sizeof(uint8_t);
	}
#endif
}
/**************************************************************************************************************************/
static void reverse(void* _arr, size_t _num_elem, size_t _size_m)
{
	if (_num_elem <= 1) return;
	size_t first = 0, last = (_num_elem - 1) * _size_m;
	while (first < last)
	{
		memswap((((char*)_arr) + first), (((char*)_arr) + last), _size_m);
		first += _size_m;
		last -= _size_m;
	}
}
/**************************************************************************************************************************/
static void reverseBytes(void* _buffer, size_t _num_of_bytes) {
	if (_num_of_bytes <= 1) return;
	char *first = (char*)_buffer, *last = ((((char*)_buffer) + _num_of_bytes) - 1);
	char temp;

	while (first < last)
	{
		temp = *first;
		*first = *last;
		*last = temp;

		first++;
		last--;
	}
}
/**************************************************************************************************************************/
static void randomize(void* _arr, size_t _num_elem, size_t _size_m) //Fisher–Yates shuffle algorithm
{
	size_t i, j;
	if (_num_elem <= 1) return;
	srand((unsigned int)time(NULL));
	_num_elem *= _size_m;
	for (i = _num_elem - _size_m; i > 0; i -= _size_m)
	{
		j = (rand() % ((i / _size_m) + 1)) * _size_m;
		memswap((((char*)_arr) + i), (((char*)_arr) + j), _size_m);
	}
}
/**************************************************************************************************************************/
//return a pointer of the first duplicate or NULL if no duplicates were found
static void* duplicates(void* _arr, size_t _num_elem, size_t _size_m)
{
	size_t i, j;
	_num_elem *= _size_m;
	for (i = 0; i < _num_elem - _size_m; i += _size_m)
	{
		for (j = i + _size_m; j < _num_elem; j += _size_m)
		{
			if (memcmp((((char*)_arr) + i), (((char*)_arr) + j), _size_m) == 0)
			{
				return (((char*)_arr) + i); /*(i / _size_m); */  // address of the duplicate
			}
		}
	}
	return NULL; //No duplicates
}
/**************************************************************************************************************************/
//Return value:
//	a pointer to the element.
//	NULL: _Key not found
static void* linearSearch(const void* _Key, const void* _Base, size_t _NumOfElements, size_t _sizeOfElement, int(_CompareFunction)(const void*, const void*))
{
	size_t i;
	_NumOfElements *= _sizeOfElement;

	for (i = 0; i < _NumOfElements; i += _sizeOfElement)
	{
		if (!_CompareFunction(_Key, ((const char*)_Base) + i)) return ((char*)_Base) + i;
	}
	return NULL;
}

//to use only if the _Base is a sorted array
//Return value:
//	a pointer to the element.
//	NULL: _Key not found
static void* binarySearch(const void* _Key, const void* _Base, size_t _NumOfElements, size_t _sizeOfElement, int(_CompareFunction)(const void* _Key, const void* _Element))
{
	if (_NumOfElements == 0 || _sizeOfElement == 0) return NULL;
	size_t sizeNewArray, temp;
	int cmpResult;
	char* newArray;
	newArray = (char*)_Base;
	sizeNewArray = _NumOfElements;

	//Check if the _Key is conteined in the interval of the array
	cmpResult = _CompareFunction(_Key, &newArray[0]);
	if (cmpResult > 0)
	{
		newArray = &newArray[_sizeOfElement];
		sizeNewArray -= 1;
	}
	else if (cmpResult < 0) return NULL;
	else return &newArray[0];

	if (sizeNewArray == 0) return NULL;
	cmpResult = _CompareFunction(_Key, &newArray[(sizeNewArray - 1) * _sizeOfElement]);
	if (cmpResult > 0) return NULL;
	else if (cmpResult < 0) sizeNewArray -= 1;
	else return &newArray[(sizeNewArray - 1) * _sizeOfElement];

	//binary search the array
	while (sizeNewArray != 0)
	{
		temp = sizeNewArray / 2;
		cmpResult = _CompareFunction(_Key, &newArray[temp * _sizeOfElement]);
		//printf("%d\n", *((int*)(&newArray[temp * _sizeOfElement])));
		if (cmpResult > 0)
		{
			newArray = &newArray[(temp + 1) * _sizeOfElement];
			sizeNewArray = sizeNewArray - temp;
		}
		else if (cmpResult < 0)
		{
			sizeNewArray = sizeNewArray - temp;
		}
		else return &newArray[temp * _sizeOfElement];
	}
	return NULL;
}
/**************************************************************************************************************************/
//static void* interpSearch(const void* _Key, const void* _Base, size_t _NumOfElements, size_t _sizeOfElement, int(_CompareFunction)(const void* _Key, const void* _Element))
////int interpSearch(int a[], int N, int x)
//{
//	size_t leftPivot, rightPivot, middlePivot;
//	leftPivot = 0;
//	rightPivot = _NumOfElements - 1;
//	middlePivot = 0;
//	_CompareFunction(_Key, ((char*)_Base) + leftPivot * _sizeOfElement);
//	if (*_Key >= _Base[leftPivot] && *_Key <= _Base[rightPivot]) {
//		do
//		{
//			m = s + (d - s) * (x - a[s]) / (a[d] - a[s]);
//			if (x > a[m]) s = m + 1;
//			else
//				d = m - 1;
//		} while (s < d && a[m] != x && x <= a[d] && x >= a[s] && a[d] != a[s]);
//	}
//
//	return m;//apel a[m]==x
//}




/***************************************************************************/
// Returns the number of elements removed
static size_t remove_value(void* _arr, void* _key, size_t _num_elem, size_t size_m, int (*compar)(const void*, const void*))
{
	size_t i;
	char* paste_pivot;

	paste_pivot = NULL;
	_num_elem *= size_m;
	for (i = 0; i < _num_elem; i += size_m)
	{
		if (compar(_key, (((char*)_arr) + i)) == 0)
		{
			if (paste_pivot == NULL)
			{
				paste_pivot = (((char*)_arr) + i);
			}
		}
		else if (paste_pivot != NULL)
		{
			memcpy(paste_pivot, (((char*)_arr) + i), size_m);
			paste_pivot += size_m;
		}
	}
	if (paste_pivot == NULL) return 0;
	return ((((size_t)(((char*)_arr) + i)) - ((size_t)paste_pivot)) / size_m);
}

static size_t remove_value_int(int* _arr, int _value, size_t _num_elem)
{
	size_t i;
	int* paste_pivot;

	paste_pivot = NULL;
	for (i = 0; i < _num_elem; i++)
	{
		if (_arr[i] == _value)
		{
			if (paste_pivot == NULL)
			{
				paste_pivot = &_arr[i];
			}
		}
		else if (paste_pivot != NULL)
		{
			*paste_pivot = _arr[i];
			paste_pivot++;
		}
	}
	if (paste_pivot == NULL) return 0;
	return (((size_t)&_arr[i]) - ((size_t)paste_pivot)) / sizeof(int);
}

// remove an interval of elements, extremes included, return number of total elements
static size_t remove_elem(void* _arr, size_t _num_elem, size_t _size_m, size_t _pos_1, size_t _pos_2)
{
	if (_pos_1 > _pos_2 || _pos_2 >= _num_elem) return 0;
	size_t moved_mem;
	_pos_1 *= _size_m;
	_pos_2 += 1;
	_pos_2 *= _size_m;
	_num_elem *= _size_m;
	moved_mem = MIN(_pos_2 - _pos_1, _num_elem - _pos_2);

	while (moved_mem > 0)
	{
		memcpy((((char*)_arr) + _pos_1), (((char*)_arr) + _pos_2), moved_mem);
		_pos_1 += moved_mem;
		_pos_2 += moved_mem;
		moved_mem = MIN(_pos_2 - _pos_1, _num_elem - _pos_2);
	}
	return (_pos_2 - _pos_1) / _size_m;
}

static void* memshift_r(void* _arr, size_t _size, size_t _shift)
{
	char* copy_pivot, * paste_pivot;
	size_t mem_chunk;

	mem_chunk = MIN(_size, _shift);
	copy_pivot = (char*)_arr + (_size - mem_chunk);
	paste_pivot = (char*)_arr + _size + _shift - mem_chunk;

	while (mem_chunk > 0)
	{
		memcpy(paste_pivot, copy_pivot, mem_chunk);
		mem_chunk = MIN(copy_pivot - (char*)_arr, mem_chunk);
		copy_pivot -= mem_chunk;
		paste_pivot -= mem_chunk;
	}
	return (void*)((char*)_arr + _size);
}

static void* memshift_l(void* _arr, size_t _size, size_t _shift)
{
	char* copy_pivot, * paste_pivot;
	size_t mem_chunk;

	mem_chunk = MIN(_size, _shift);
	copy_pivot = (char*)_arr;
	paste_pivot = (char*)_arr - _shift;

	while (mem_chunk > 0)
	{
		memcpy(paste_pivot, copy_pivot, mem_chunk);
		mem_chunk = MIN((char*)_arr + _size - copy_pivot - 1, mem_chunk);
		copy_pivot += mem_chunk;
		paste_pivot += mem_chunk;
	}
	return (void*)((char*)_arr - _size);
}

static void* memmove_2(void* destination, const void* source, size_t num_bytes) {
	if (destination < source) {
		if ((char*)destination + num_bytes > (char*)source) {
			return memshift_l((void*)source, num_bytes, (char*)source - (char*)destination);
		}
	}
	else if (destination > source) {
		if ((char*)source + num_bytes > (char*)destination) {
			return memshift_r((void*)source, num_bytes, (char*)destination - (char*)source);
		}
	}
	else if (destination == source) {
		return (void*)source;
	}
	return memcpy(destination, source, num_bytes);
}
/**************************************************************************************************************************/
static char isDigitOnly(char* _str) {
	if (!_str) return 0;
	if (!(*_str)) return 0;

	while (_str) {
		if (*_str < '0' || *_str > '9') {
			return 0;
		}
	}
	return 1;
}
/**************************************************************************************************************************/
static char isAlpha(char* _str) {
	if (!_str) return 0;
	if (!(*_str)) return 0;

	while (_str) {
		if (!(*_str >= 'A' && *_str <= 'Z') || !(*_str >= 'a' && *_str <= 'z')) {
			return 0;
		}
	}
	return 1;
}
/**************************************************************************************************************************/
/* Function to print an array */
static void printArray_int(int arr[], size_t size)
{
	size_t i;
	for (i = 0; i < size; i++)
	{
		printf("[%lu] = %d, ", i, arr[i]);
		printf("\n");
	}
	printf("\n\n");
}

static void printArray_short(short arr[], size_t size)
{
	size_t i;
	for (i = 0; i < size; i++)
	{
		printf("[%lu] = %d, ", i, arr[i]);
		printf("\n");
	}
	printf("\n\n");
}

static void printArray_g_s(void *_arr, size_t _n_elem, size_t _size_m)
{
	size_t i, n_elem;
	char *arr;
	arr = (char*)_arr;
	n_elem = _n_elem * _size_m;
	for (i = 0; i < n_elem; i += _size_m)
	{
		printf("[%lu] = %d, ", (i / _size_m), arr[i]);
		printf("\n");
	}
	printf("\n\n");
}
/**************************************************************************************************************************/
/* Function to print an array */
static void fillArray_int(int arr[], size_t size)
{
	size_t i;
	for (i = 0; i < size; i++)
	{
		arr[i] = i;
	}
}
static void fillArray_short(short arr[], size_t size)
{
	size_t i;
	for (i = 0; i < size; i++)
	{
		arr[i] = i;
	}
}
/**************************************************************************************************************/

//return value	meaning
// < 0	The element pointed to by p1 goes before the element pointed to by p2
// 	 0	The element pointed to by p1 is equivalent to the element pointed to by p2
// > 0	The element pointed to by p1 goes after the element pointed to by p2
static inline int compare_int(const void* a, const void* b)
{
	if (*(int*)a < *(int*)b) return -1;
	if (*(int*)a == *(int*)b) return 0;
	if (*(int*)a > *(int*)b) return 1;
}
static inline int compare_short(const void* a, const void* b)
{
	if (*(short*)a < *(short*)b) return -1;
	if (*(short*)a == *(short*)b) return 0;
	if (*(short*)a > *(short*)b) return 1;
}
static inline int compare_float(const void* a, const void* b)
{
	if (*(float*)a < *(float*)b) return -1;
	if (*(float*)a == *(float*)b) return 0;
	if (*(float*)a > *(float*)b) return 1;
}
static inline int compare_double(const void* a, const void* b)
{
	if (*(double*)a < *(double*)b) return -1;
	if (*(double*)a == *(double*)b) return 0;
	if (*(double*)a > *(double*)b) return 1;
}
static inline int compare_char(const void* a, const void* b)
{
	if (*(char*)a < *(char*)b) return -1;
	if (*(char*)a == *(char*)b) return 0;
	if (*(char*)a > *(char*)b) return 1;
}
static inline int compare_size_t(const void* a, const void* b)
{
	if (*(size_t*)a < *(size_t*)b) return -1;
	if (*(size_t*)a == *(size_t*)b) return 0;
	if (*(size_t*)a > *(size_t*)b) return 1;
}
static inline int eaven_first(void* _a, void* _b)
{
	if ((*(int*)_a % 2) == (*(int*)_b % 2))
	{
		if (*(int*)_a < *(int*)_b) return -1;
		if (*(int*)_a == *(int*)_b) return 0;
		if (*(int*)_a > *(int*)_b) return 1;
	}
	else
	{
		if ((*(int*)_a % 2) == 1)
		{
			return -1;
		}
		if ((*(int*)_b % 2) == 1)
		{
			return 1;
		}
	}
}

//QuickSort
static void quickSort(void* _arr, size_t _n_elem, size_t _size_m, int (*compar)(const void*, const void*))
{
	if (_n_elem <= 1) return;
	char* arr, *pivot;
	size_t i, n_elem, pivot_index;

	pivot_index = 0;
	n_elem = _n_elem * _size_m;
	arr = (char*)_arr;
	pivot = &arr[n_elem - _size_m];

	for (i = 0; i < n_elem - _size_m; i += _size_m)
	{
		if (compar(&arr[i], pivot) < 0)
		{
			if (i > pivot_index)
			{
				memswap(&arr[i], &arr[pivot_index], _size_m);
			}
			pivot_index += _size_m;
		}
	}
	memswap(pivot, &arr[pivot_index], _size_m);

	quickSort(arr, pivot_index / _size_m, _size_m, compar);
	quickSort(arr + pivot_index + _size_m, (n_elem - pivot_index - _size_m) / _size_m, _size_m, compar);
}

static void quickSort_s(void* _arr, size_t _n_elem, size_t _size_m, int (*compar)(void*, const void*, const void*), void* _context)
{
	//if (_n_elem < 11)
	//{
	//	my_sort_mix_s(_arr, _n_elem, _size_m, compar, _context);
	//	return;
	//}
	
	if (_n_elem <= 1) return;
	char* arr, * pivot;
	size_t i, n_elem, pivot_index;

	pivot_index = 0;
	n_elem = _n_elem * _size_m;
	arr = (char*)_arr;
	pivot = &arr[n_elem - _size_m];

	for (i = 0; i < n_elem - _size_m; i += _size_m)
	{
		if (compar(_context , &arr[i], pivot) < 0)
		{
			if (i > pivot_index)
			{
				memswap(&arr[i], &arr[pivot_index], _size_m);
			}
			pivot_index += _size_m;
		}
	}
	memswap(pivot, &arr[pivot_index], _size_m);

	quickSort_s(arr, pivot_index / _size_m, _size_m, compar, _context);
	quickSort_s(arr + pivot_index + _size_m, (n_elem - pivot_index - _size_m) / _size_m, _size_m, compar, _context);
}

static void quickSort_int(int *_arr, size_t _n_elem)
{
	if (_n_elem <= 1) return;

	int *pivot = &_arr[_n_elem - 1];
	int temp;
	size_t i, pivot_index = 0;

	for (i = 0; i < _n_elem - 1; i++)
	{
		if (_arr[i] < *pivot)
		{
			if (i > pivot_index)
			{
				temp = _arr[i];
				_arr[i] = _arr[pivot_index];
				_arr[pivot_index] = temp;
			}
			pivot_index++;
		}
	}
	temp = *pivot;
	*pivot = _arr[pivot_index];
	_arr[pivot_index] = temp;

	quickSort_int(_arr, pivot_index);
	quickSort_int(_arr + pivot_index+1, _n_elem - pivot_index - 1);
}

static void bubbleSort(void* _arr, size_t _n_elem, size_t _size_m, int (*compar)(const void*, const void*))
{
	if (_n_elem <= 1) return;

	char ord, * arr;
	size_t i, first_order;
	arr = (char*)_arr;
	first_order = _n_elem * _size_m;

	do {
		ord = 1;
		for (i = 0; i < first_order - _size_m; i += _size_m)
		{
			if (compar(&arr[i], &arr[i + _size_m]) > 0)
			{
				memswap(&arr[i], &arr[i + _size_m], _size_m);
				ord = 0;
			}
		}
		first_order -= _size_m;
	} while (!ord);
}

static void bubbleSort_s(void* _arr, size_t _n_elem, size_t _size_m, int (*compar)(void*, const void*, const void*), void* _context)
{
	if (_n_elem <= 1) return;

	char ord, * arr;
	size_t i, first_order;
	arr = (char*)_arr;
	first_order = _n_elem * _size_m;

	do {
		ord = 1;
		for (i = 0; i < first_order - _size_m; i += _size_m)
		{
			if (compar(_context, &arr[i], &arr[i + _size_m]) > 0)
			{
				memswap(&arr[i], &arr[i + _size_m], _size_m);
				ord = 0;
			}
		}
		first_order -= _size_m;
	} while (!ord);
}

//return 1 on fail or 0 on success
static int shellSort(void* _arr, size_t _n_elem, size_t _size_m, int (*compar)(const void*, const void*))
{
	//size_t cmp, swaps;
	//cmp = 0;
	//swaps = 0;
	size_t i, j, gap;
	size_t buf[4];

	char* arr, *pivot;
	arr = (char*)_arr;

	if (sizeof(buf) >= _size_m) {
		pivot = (char*)&buf;
	}
	else {
		pivot = (char*)malloc(_size_m);
		if (!pivot) return 1;
	}

	// Start with a big gap, then reduce the gap
	for (gap = _n_elem / 2; gap > 0; gap /= 2)
	{
		// Do a gapped insertion sort for this gap size.
		// The first gap elements a[0..gap-1] are already in gapped order
		// keep adding one more element until the entire array is
		// gap sorted
		for (i = gap; i < _n_elem; i++)
		{
			// add a[i] to the elements that have been gap sorted
			// save a[i] in temp and make a hole at position i
			memcpy(pivot, &arr[i * _size_m], _size_m);

			// shift earlier gap-sorted elements up until the correct
			// location for a[i] is found
			//cmp++;
			for (j = i; j >= gap && compar(&arr[(j - gap) * _size_m], pivot) > 0; j -= gap)
			{
				memswap(&arr[j * _size_m], &arr[(j - gap) * _size_m], _size_m);
				//swaps++;
			}

			//  put temp (the original a[i]) in its correct location
			memcpy(&arr[j * _size_m], pivot, _size_m);
			//swaps++;
		}
	}
	free(pivot);
	//printf("\n\nComparations: %llu \t Swaps: %llu\n\n", cmp, swaps);
	return 0;
}

static int is_sorted(void* _arr, size_t _n_elem, size_t _size_m, int (*compar)(const void*, const void*))
{
	size_t i;
	char* arr = (char*)_arr;
	_n_elem = _n_elem * _size_m;
	for (i = 0; i < _n_elem - _size_m; i += _size_m)
	{
		if (compar(&arr[i], &arr[i + _size_m]) > 0)
		{
			return 0;
		}
	}
	return 1;
}

static int is_sorted_int(int* _arr, size_t _n_elem)
{
	size_t i;
	for (i = 0; i < _n_elem-1; i++)
	{
		if (_arr[i] > _arr[i+1])
		{
			return 0;
		}
	}
	return 1;
}
/**************************************************************************************************************/
static int is_prime(size_t num)
{
	size_t pivot, divizor = 3;

	if (num == 1) return 0;
	if (num == 2) return 1;
	if ((num % 2) == 0) return 0;

	pivot = (size_t)sqrt(num);

	while (divizor <= pivot)
	{
		if ((num % divizor) == 0) return 0;
		divizor += 2;
	}
	return 1;
}

static int is_armstrong(int num)
{
	size_t j, nr_cifre = 0;
	int temp, suma;
	temp = num;
	while (temp != 0)
	{
		temp = temp / 10;
		nr_cifre++;
	}

	suma = 0;
	temp = num;
	for (j = 0; j < nr_cifre; j++)
	{
		suma += pow((temp % 10), nr_cifre);
		temp = temp / 10;
	}
	if (suma == num) return 1;
	return 0;
}

static int is_vocal(char _c)
{
	size_t i;
	char vocals[] = { 'A', 'E', 'I', 'O', 'U'};
	for (i = 0; i < sizeof(vocals); i++)
	{
		if (_c == vocals[i] || _c == (vocals[i] + 32))
		{
			return 1;
		}
	}
	return 0;
}

static void to_upper(char* _str)
{
	size_t j = 0;
	while (_str[j])
	{
		_str[j] = (char)toupper(_str[j]);
		j++;
	}
}

static int is_palindrom_str(const char* str, size_t len) {
	size_t first, last;
	first = 0;
	last = len-1;

	while (first < last) {
		if (str[first] != str[last]) return 0;
		first++;
		last--;
	}
	return 1;
}

/***************************************************************************************************************************************************/

//size_t context[2]; context[0] = 0;//comparisions context[1] = 0; //swaps
static int compare_int_count(void *_context, void *a, void* b)
{
	((size_t*)(_context))[0] += 1;	//comparisions
	if (*(int*)a < *(int*)b)
	{
		((size_t*)(_context))[1] += 1;	//swaps
		return -1;
	}
	((size_t*)(_context))[0] += 1;	//comparisions
	if (*(int*)a == *(int*)b)
	{
		return 0;
	}
	((size_t*)(_context))[0] += 1;	//comparisions
	if (*(int*)a > *(int*)b)
	{
		return 1;
	}
}
/***************************************************************************************************************************************************/

static void bubblesort2_left_right_int(int* _arr, size_t _n_elem, size_t _size_m)
{
	size_t i, pivot_pos;
	pivot_pos = 0;
	size_t cmp, swaps;
	cmp = 0;
	swaps = 0;

	while (_n_elem > 1)
	{
		pivot_pos = 0;
		//printf("%d, ", _arr[pivot_pos]);
		for (size_t i = 1; i < _n_elem; i++)
		{
			//printf("%d, ", _arr[i]);
			cmp++;
			if (_arr[pivot_pos] < _arr[i])
			{
				if (pivot_pos < (i-1))
				{
					swaps++;
					//printf("\t%d < %d | pivod: %d  i-1: %d | swap %d  %d\n", _arr[pivot_pos], _arr[i], pivot_pos, i - 1, _arr[pivot_pos], _arr[i - 1]);
					memswap(&_arr[pivot_pos], &_arr[i-1], _size_m);
				}
				pivot_pos = i;
			}
		}
		//printf("\n");
		if (pivot_pos < _n_elem-1)
		{
			//printf("\t%d < %d swap %d  %d\n", _arr[pivot_pos], _arr[_n_elem-1], _arr[pivot_pos], _arr[_n_elem - 1]);
			swaps++;
			memswap(&_arr[pivot_pos], &_arr[_n_elem-1], _size_m);
		}

		_n_elem--;
	}
	printf("Comparations: %d \t Swaps: %d\n\n", cmp, swaps);
}
/***************************************************************************************************************************************************/

//#define MY_SORT_CNT
//#define MY_SORT_STEPS
static void bubbleSort2_left_right(void* _arr, size_t _n_elem, size_t _size_m, int (*compar)(const void*, const void*))
{
#ifdef MY_SORT_CNT
	size_t cmp, swaps;
	cmp = 0;
	swaps = 0;
#endif // MY_SORT_CNT

	size_t i, pivot_pos, right_swap, right_side;
	//_n_elem *= _size_m;
	right_side = _n_elem * _size_m;

	while (right_side > _size_m)
	{
		right_swap = 0;
		pivot_pos = 0;
		for (size_t i = _size_m; i < right_side; i += _size_m)
		{

#ifdef MY_SORT_CNT
			//printf("%d, ", ((int*)_arr)[pivot_pos / sizeof(int)]);
			cmp++;
#endif // MY_SORT_CNT
			if (compar(&((char*)_arr)[pivot_pos], &((char*)_arr)[i]) < 0)
			{
				if (pivot_pos < (i - _size_m))
				{
#ifdef MY_SORT_CNT
					swaps++;
					//printf("  \t\t%d < %d | pivot: %d  i-1: %d | swap %d  %d\n", ((int*)_arr)[pivot_pos / sizeof(int)], ((int*)_arr)[i / sizeof(int)], pivot_pos, i / sizeof(int) - 1, ((int*)_arr)[pivot_pos / sizeof(int)], ((int*)_arr)[i / sizeof(int) - 1]);
#endif // MY_SORT_CNT
					memswap(&((char*)_arr)[pivot_pos], &((char*)_arr)[i - _size_m], _size_m);
					right_swap = i - _size_m;
				}
				pivot_pos = i;
			}
		}
		if (pivot_pos < right_side - _size_m)
		{
#ifdef MY_SORT_CNT
			swaps++;
			//printf("  \t\t%d < %d swap %d  %d\n", ((int*)_arr)[pivot_pos / sizeof(int)], ((int*)_arr)[right_side / sizeof(int) - 1], ((int*)_arr)[pivot_pos / sizeof(int)], ((int*)_arr)[right_side / sizeof(int) - 1]);
#endif // MY_SORT_CNT
			memswap(&((char*)_arr)[pivot_pos], &((char*)_arr)[right_side - _size_m], _size_m);
			right_swap = right_side - _size_m;
		}
#ifdef MY_SORT_CNT
		//printf("pivot_pos: %d\n\n", pivot_pos / _size_m);
#endif // MY_SORT_CNT
		right_side = right_swap;
	}
#ifdef MY_SORT_CNT
	printf("\n\nComparations: %llu \t Swaps: %llu\n\n", cmp, swaps);
#endif // MY_SORT_CNT
}
/***************************************************************************************************************************************************/

static void bubbleSort2_right_left(void* _arr, size_t _n_elem, size_t _size_m, int (*compar)(const void*, const void*))
{
#ifdef MY_SORT_STEPS
	size_t cmp, swaps;
	cmp = 0;
	swaps = 0;
#endif // MY_SORT_STEPS

	size_t i, pivot_pos, left_side, left_swap, right_side;
	left_side = 0;
	right_side = _n_elem * _size_m;

	while (left_side < right_side - _size_m)
	{
		left_swap = right_side;

		pivot_pos = right_side - _size_m;
		for (size_t i = right_side - 2* _size_m; i >= left_side; i -= _size_m)
		{
#ifdef MY_SORT_STEPS
			printf("Pivot: %d, pos: %d\n", ((int*)_arr)[pivot_pos / _size_m], pivot_pos / _size_m);
			printf("%d, ", ((int*)_arr)[i / _size_m]);
#endif // MY_SORT_STEPS
			if (compar(&((char*)_arr)[pivot_pos], &((char*)_arr)[i]) > 0)
			{
#ifdef MY_SORT_STEPS
				printf("  \t\t%d > %d\n", ((int*)_arr)[pivot_pos / _size_m], ((int*)_arr)[i / _size_m]);
#endif // MY_SORT_STEPS
				if (pivot_pos > (i + _size_m))
				{
#ifdef MY_SORT_STEPS
					printf("  \t\t%d > %d    swap: %d  %d\n", ((int*)_arr)[pivot_pos / _size_m], ((int*)_arr)[i / _size_m], ((int*)_arr)[pivot_pos / _size_m], ((int*)_arr)[i / _size_m + 1]);
#endif // MY_SORT_STEPS
					memswap(&((char*)_arr)[pivot_pos], &((char*)_arr)[i + _size_m], _size_m);
					left_swap = i + _size_m;
				}

				pivot_pos = i;
				//printf("pivot_pos: %d\n", pivot_pos);
			}
			if (i == 0) break;
		}
		if (pivot_pos > left_side)
		{
#ifdef MY_SORT_STEPS
			printf("  \t\tswap: %d  %d\n", ((int*)_arr)[pivot_pos / _size_m], ((int*)_arr)[left_side / _size_m]);
#endif // MY_SORT_STEPS
			memswap(&((char*)_arr)[pivot_pos], &((char*)_arr)[left_side], _size_m);
			left_swap = left_side + _size_m;
		}
		left_side = left_swap;
#ifdef MY_SORT_STEPS
		printf("left_side: %d\n", left_side);
		printf("right_side: %d\n\n", right_side);
#endif // MY_SORT_STEPS
	}
#ifdef MY_SORT_STEPS
	printf("\n\nComparations: %llu \t Swaps: %llu\n\n", cmp, swaps);
#endif // MY_SORT_STEPS
}
/***************************************************************************************************************************************************/

//#define MY_SORT_CNT
//#define MY_SORT_STEPS
static void bubbleSort2(void* _arr, size_t _n_elem, size_t _size_m, int (*compar)(const void*, const void*))
{
#ifdef MY_SORT_CNT
	size_t cmp, swaps;
	cmp = 0;
	swaps = 0;
#endif // DEMY_SORT_CNT

	size_t i, pivot_pos, left_side, left_swap, right_side, right_swap;
	left_side = 0;
	right_side = _n_elem * _size_m;

	while (left_side < right_side - _size_m && right_side > 1)
	{
		right_swap = left_side;
		pivot_pos = left_side;
#ifdef MY_SORT_STEPS
		printf("\n**********************Left To right*************************\n\n");
#endif // MY_SORT_STEPS
		for (size_t i = left_side + _size_m; i < right_side; i += _size_m)
		{
#ifdef MY_SORT_CNT
			cmp++;
#endif // DEMY_SORT_CNT
#ifdef MY_SORT_STEPS
			printf("%d, ", ((int*)_arr)[pivot_pos / sizeof(int)]);
#endif // MY_SORT_STEPS
			if (compar(&((char*)_arr)[pivot_pos], &((char*)_arr)[i]) < 0)
			{
				if (pivot_pos < (i - _size_m))
				{
#ifdef MY_SORT_CNT
				swaps++;
#endif // DEMY_SORT_CNT
#ifdef MY_SORT_STEPS
					printf("  \t\t%d < %d | pivot: %d  i-1: %d | swap %d  %d\n", ((int*)_arr)[pivot_pos / sizeof(int)], ((int*)_arr)[i / sizeof(int)], pivot_pos, i / sizeof(int) - 1, ((int*)_arr)[pivot_pos / sizeof(int)], ((int*)_arr)[i / sizeof(int) - 1]);
#endif // MY_SORT_STEPS
					memswap(&((char*)_arr)[pivot_pos], &((char*)_arr)[i - _size_m], _size_m);
					right_swap = i - _size_m;
				}
				pivot_pos = i;
			}
		}
		if (pivot_pos < right_side - _size_m)
		{
#ifdef MY_SORT_CNT
			swaps++;
#endif // DEMY_SORT_CNT
#ifdef MY_SORT_STEPS
			printf("  \t\t%d > %d swap %d  %d\n", ((int*)_arr)[pivot_pos / sizeof(int)], ((int*)_arr)[right_side / sizeof(int) - 1], ((int*)_arr)[pivot_pos / sizeof(int)], ((int*)_arr)[right_side / sizeof(int) - 1]);
#endif // MY_SORT_STEPS
			memswap(&((char*)_arr)[pivot_pos], &((char*)_arr)[right_side - _size_m], _size_m);
			right_swap = right_side - _size_m;
		}
#ifdef MY_SORT_STEPS
		printf("pivot_pos: %d\n\n", pivot_pos / _size_m);
#endif // MY_SORT_STEPS
		right_side = right_swap;

#ifdef MY_SORT_STEPS
		printf("\n\nComparations: %llu \t Swaps: %llu\n\n", cmp, swaps);
#endif // MY_SORT_STEPS

		/*****************************************************************************************************************/
#ifdef MY_SORT_STEPS
		printf("\n**********************Right to left*************************\n\n");
#endif // MY_SORT_STEPS
		if (right_side < 2 * _size_m) continue;
		left_swap = right_side;
		pivot_pos = right_side - _size_m;
		for (size_t i = right_side - 2 * _size_m; i >= left_side; i -= _size_m)
		{
#ifdef MY_SORT_STEPS
			printf("Pivot: %d, pos: %d\n", ((int*)_arr)[pivot_pos / _size_m], pivot_pos / _size_m);
			printf("%d, ", ((int*)_arr)[i / _size_m]);
#endif // MY_SORT_STEPS
#ifdef MY_SORT_CNT
			cmp++;
#endif // DEMY_SORT_CNT
			if (compar(&((char*)_arr)[pivot_pos], &((char*)_arr)[i]) > 0)
			{
#ifdef MY_SORT_STEPS
				printf("  \t\t%d > %d\n", ((int*)_arr)[pivot_pos / _size_m], ((int*)_arr)[i / _size_m]);
#endif // MY_SORT_STEPS
				if (pivot_pos > (i + _size_m))
				{
#ifdef MY_SORT_CNT
					swaps++;
#endif // DEMY_SORT_CNT
#ifdef MY_SORT_STEPS
					printf("  \t\t%d > %d    swap: %d  %d\n", ((int*)_arr)[pivot_pos / _size_m], ((int*)_arr)[i / _size_m], ((int*)_arr)[pivot_pos / _size_m], ((int*)_arr)[i / _size_m + 1]);
#endif // MY_SORT_STEPS
					memswap(&((char*)_arr)[pivot_pos], &((char*)_arr)[i + _size_m], _size_m);
					left_swap = i + _size_m;
				}
				pivot_pos = i;
				//printf("pivot_pos: %d\n", pivot_pos);
			}
			if (i == 0) break;
		}
		if (pivot_pos > left_side)
		{
#ifdef MY_SORT_STEPS
			printf("  \t\tswap: %d  %d\n", ((int*)_arr)[pivot_pos / _size_m], ((int*)_arr)[left_side / _size_m]);
#endif // MY_SORT_STEPS
#ifdef MY_SORT_CNT
			swaps++;
#endif // DEMY_SORT_CNT
			memswap(&((char*)_arr)[pivot_pos], &((char*)_arr)[left_side], _size_m);
			left_swap = left_side + _size_m;
		}
		left_side = left_swap;
#ifdef MY_SORT_STEPS
		printf("left_side: %d\n", left_side);
		printf("right_side: %d\n\n", right_side);
#endif // MY_SORT_STEPS
	}
#ifdef MY_SORT_CNT
printf("\n\nComparations: %llu \t Swaps: %llu\n\n", cmp, swaps);
#endif // MY_SORT_CNT
}
/***************************************************************************************************************************************************/

//#define MY_SORT_CNT
//#define MY_SORT_STEPS
static void bubbleSort2_s(void* _arr, size_t _n_elem, size_t _size_m, int (*compar)(void* _context, const void*, const void*), void* _context)
{
#ifdef MY_SORT_CNT
	size_t cmp, swaps;
	cmp = 0;
	swaps = 0;
#endif // DEMY_SORT_CNT

	size_t i, pivot_pos, left_side, left_swap, right_side, right_swap;
	left_side = 0;
	right_side = _n_elem * _size_m;

	while (left_side < right_side - _size_m && right_side > 1)
	{
		right_swap = left_side;
		pivot_pos = left_side;
#ifdef MY_SORT_STEPS
		printf("\n**********************Left To right*************************\n\n");
#endif // MY_SORT_STEPS
		for (size_t i = left_side + _size_m; i < right_side; i += _size_m)
		{
#ifdef MY_SORT_CNT
			cmp++;
#endif // DEMY_SORT_CNT
#ifdef MY_SORT_STEPS
			printf("%d, ", ((int*)_arr)[pivot_pos / sizeof(int)]);
#endif // MY_SORT_STEPS
			if (compar(_context , &((char*)_arr)[pivot_pos], &((char*)_arr)[i]) < 0)
			{
				if (pivot_pos < (i - _size_m))
				{
#ifdef MY_SORT_CNT
					swaps++;
#endif // DEMY_SORT_CNT
#ifdef MY_SORT_STEPS
					printf("  \t\t%d < %d | pivot: %d  i-1: %d | swap %d  %d\n", ((int*)_arr)[pivot_pos / sizeof(int)], ((int*)_arr)[i / sizeof(int)], pivot_pos, i / sizeof(int) - 1, ((int*)_arr)[pivot_pos / sizeof(int)], ((int*)_arr)[i / sizeof(int) - 1]);
#endif // MY_SORT_STEPS
					memswap(&((char*)_arr)[pivot_pos], &((char*)_arr)[i - _size_m], _size_m);
					right_swap = i - _size_m;
				}
				pivot_pos = i;
			}
		}
		if (pivot_pos < right_side - _size_m)
		{
#ifdef MY_SORT_CNT
			swaps++;
#endif // DEMY_SORT_CNT
#ifdef MY_SORT_STEPS
			printf("  \t\t%d > %d swap %d  %d\n", ((int*)_arr)[pivot_pos / sizeof(int)], ((int*)_arr)[right_side / sizeof(int) - 1], ((int*)_arr)[pivot_pos / sizeof(int)], ((int*)_arr)[right_side / sizeof(int) - 1]);
#endif // MY_SORT_STEPS
			memswap(&((char*)_arr)[pivot_pos], &((char*)_arr)[right_side - _size_m], _size_m);
			right_swap = right_side - _size_m;
		}
#ifdef MY_SORT_STEPS
		printf("pivot_pos: %d\n\n", pivot_pos / _size_m);
#endif // MY_SORT_STEPS
		right_side = right_swap;

#ifdef MY_SORT_STEPS
		printf("\n\nComparations: %llu \t Swaps: %llu\n\n", cmp, swaps);
#endif // MY_SORT_STEPS

		/*****************************************************************************************************************/
#ifdef MY_SORT_STEPS
		printf("\n**********************Right to left*************************\n\n");
#endif // MY_SORT_STEPS
		if (right_side < 2 * _size_m) continue;
		left_swap = right_side;
		pivot_pos = right_side - _size_m;
		for (size_t i = right_side - 2 * _size_m; i >= left_side; i -= _size_m)
		{
#ifdef MY_SORT_STEPS
			printf("Pivot: %d, pos: %d\n", ((int*)_arr)[pivot_pos / _size_m], pivot_pos / _size_m);
			printf("%d, ", ((int*)_arr)[i / _size_m]);
#endif // MY_SORT_STEPS
#ifdef MY_SORT_CNT
			cmp++;
#endif // DEMY_SORT_CNT
			if (compar(_context , &((char*)_arr)[pivot_pos], &((char*)_arr)[i]) > 0)
			{
#ifdef MY_SORT_STEPS
				printf("  \t\t%d > %d\n", ((int*)_arr)[pivot_pos / _size_m], ((int*)_arr)[i / _size_m]);
#endif // MY_SORT_STEPS
				if (pivot_pos > (i + _size_m))
				{
#ifdef MY_SORT_CNT
					swaps++;
#endif // DEMY_SORT_CNT
#ifdef MY_SORT_STEPS
					printf("  \t\t%d > %d    swap: %d  %d\n", ((int*)_arr)[pivot_pos / _size_m], ((int*)_arr)[i / _size_m], ((int*)_arr)[pivot_pos / _size_m], ((int*)_arr)[i / _size_m + 1]);
#endif // MY_SORT_STEPS
					memswap(&((char*)_arr)[pivot_pos], &((char*)_arr)[i + _size_m], _size_m);
					left_swap = i + _size_m;
				}

				pivot_pos = i;
				//printf("pivot_pos: %d\n", pivot_pos);
			}
			if (i == 0) break;
		}
		if (pivot_pos > left_side)
		{
#ifdef MY_SORT_STEPS
			printf("  \t\tswap: %d  %d\n", ((int*)_arr)[pivot_pos / _size_m], ((int*)_arr)[left_side / _size_m]);
#endif // MY_SORT_STEPS
#ifdef MY_SORT_CNT
			swaps++;
#endif // DEMY_SORT_CNT
			memswap(&((char*)_arr)[pivot_pos], &((char*)_arr)[left_side], _size_m);
			left_swap = left_side + _size_m;
		}
		left_side = left_swap;
#ifdef MY_SORT_STEPS
		printf("left_side: %d\n", left_side);
		printf("right_side: %d\n\n", right_side);
#endif // MY_SORT_STEPS
	}
#ifdef MY_SORT_CNT
	printf("\n\nComparations: %llu \t Swaps: %llu\n\n", cmp, swaps);
#endif // MY_SORT_CNT
}
/**************************************************************************************************************/

static void test_sort()
{
	int msec;
	size_t i, n_nums;
	int *arr, *arr_copy;
	clock_t before, difference;

	n_nums = 9000;
	arr = (int*)malloc(n_nums * sizeof(*arr));
	arr_copy = (int*)malloc(n_nums * sizeof(*arr));
	if (!arr_copy) return;
	if (!arr) return;

	for (i = 0; i < n_nums; i++) {
		arr_copy[i] = i;
	}
	randomize(arr_copy, n_nums, sizeof(*arr_copy));
	if (is_sorted(arr_copy, n_nums, sizeof(*arr), compare_int)) {
		printf("ERROR: Array already sorted");
	}


	printf("bubbleSort2\n");
	memcpy(arr, arr_copy, n_nums * sizeof(*arr));
	if (is_sorted(arr, n_nums, sizeof(*arr_copy), compare_int)) {
		printf("ERROR: Array already sorted");
	}
	before = clock();
	bubbleSort2(arr, n_nums, sizeof(*arr_copy), compare_int);
	difference = clock() - before;
	msec = difference;
	if (!is_sorted(arr, n_nums, sizeof(*arr_copy), compare_int)) {
		printf("ERROR: Array not sorted");
	}
	printf("Time [clocks]: %d\n", msec);



	printf("bubbleSort2_left_right\n");
	memcpy(arr, arr_copy, n_nums * sizeof(*arr));
	if (is_sorted(arr, n_nums, sizeof(*arr_copy), compare_int)) {
		printf("ERROR: Array already sorted");
	}
	before = clock();
	bubbleSort2_left_right(arr, n_nums, sizeof(*arr_copy), compare_int);
	difference = clock() - before;
	msec = difference;
	if (!is_sorted(arr, n_nums, sizeof(*arr_copy), compare_int)) {
		printf("ERROR: Array not sorted");
	}
	printf("Time [clocks]: %d\n", msec);



	printf("shellShort\n");
	memcpy(arr, arr_copy, n_nums * sizeof(*arr));
	if (is_sorted(arr, n_nums, sizeof(*arr_copy), compare_int)) {
		printf("ERROR: Array already sorted");
	}
	before = clock();
	shellSort(arr, n_nums, sizeof(*arr_copy), compare_int);
	difference = clock() - before;
	msec = difference;
	if (!is_sorted(arr, n_nums, sizeof(*arr_copy), compare_int)) {
		printf("ERROR: Array not sorted");
	}
	printf("Time [clocks]: %d\n", msec);

	//printf("bubbleSort\n");
	//memcpy(arr, arr_copy, n_nums * sizeof(*arr));
	//before = clock();
	//bubbleSort(arr, n_nums, sizeof(*arr_copy), compare_int);
	//difference = clock() - before;
	//msec = difference;
	//if (!is_sorted(arr, n_nums, sizeof(*arr_copy), compare_int)) {
	//	printf("ERROR: Array not sorted");
	//}
	//printf("Time [clocks]: %d\n", msec);

	//printf("quickSort\n");
	//memcpy(arr, arr_copy, n_nums * sizeof(*arr));
	//before = clock();
	//quickSort(arr, n_nums, sizeof(*arr), compare_int);
	//difference = clock() - before;
	//msec = difference;
	//if (!is_sorted(arr, n_nums, sizeof(*arr), compare_int)) {
	//	printf("ERROR: Array not sorted");
	//}
	//printf("Time [clocks]: %d\n", msec);



	free(arr);
	free(arr_copy);
}
#endif // !_SORT_H_