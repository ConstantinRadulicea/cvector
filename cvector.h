//
// cvector.h
//
//      Written by RADULICEA Constantin D.P. . All rights reserved.
//
// Implentation of a dynamic vector, stack, string
//
// to-do: set error if memory allocation fail

#ifndef __CVECTOR_H__
#define __CVECTOR_H__

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "sort.h"
#include "base64.h"

#define MB 1048576      // 1 megabyte in bytes = 2^20

#define LOG_ERROR(_message_) fprintf(stderr,_message_);
#define LOG_ERROR_ON(_statement_,_condition_,_message_) do { if ((_statement_)==_condition_) fprintf(stderr,_message_); } while(0)
#define LOG_ERROR_ON_NULL(_statement_,_message_) LOG_ERROR_ON(_statement_,NULL,_message_)

#ifndef MIN(X, Y)
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#endif // !MIN

#ifndef MAX(X, Y)
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#endif // !MAX

/***********************************************cvector*************************************************************************/
static void print_int(const void* _Element) {
    printf("%d", *((int*)_Element));
}
typedef struct cvector
{
    void* data;         // pointer to the array conteining the array of elements
    size_t size;        // total elements conteined
    size_t capacity;    // maximum number of elements that can be conteined before allocate more memory
    size_t size_type;   // sizeof(element) in bytes
}cvector;

// Utility function to initialize the cvector
static int cvector_init(cvector* _Cvector, size_t _NumberOfElementsToReserve, size_t _SizeOfElement){
    if (_SizeOfElement <= 0) {
        return -2;
    }
    if (_NumberOfElementsToReserve > 0) {
        _Cvector->data = malloc(_SizeOfElement * _NumberOfElementsToReserve);
        if (!_Cvector->data && _NumberOfElementsToReserve != 0 && _SizeOfElement != 0) {
            LOG_ERROR_ON_NULL(_Cvector->data, "Failed to allocate memory in cvector_init !\n");
            return -1;
        }
    }
    else {
        _Cvector->data = NULL;
    }
    _Cvector->size = 0;
    _Cvector->capacity = _NumberOfElementsToReserve;
    _Cvector->size_type = _SizeOfElement;
    return 0;
}

// Utility function get a pointer to the buffer
static void* cvector_data(cvector* pt){
    return pt->data;
}

// Returns the number of elements in the cvector
static size_t cvector_size(cvector* pt) {
    return pt->size;
}

// Returns the maximum number of elements that the cvector can hold.
static size_t cvector_capacity(cvector* pt) {
    return pt->capacity;
}

// Returns the size of an element
static size_t cvector_size_type(cvector* pt) {
    return pt->size_type;
}

// Requests that the cvector capacity be at least enough to contain n elements
// return 0 on success, -1 on failure
static int cvector_reserve(cvector* pt, size_t _NumOfElements){
    void* temp;

    if (pt->size_type <= 0) {
        return -2;
    }
    if (pt->capacity >= _NumOfElements) {
        return 0;
    }
    if (_NumOfElements <= 0) {
        pt->data = NULL;
        pt->capacity = 0;
        free(pt->data);
        return 0;
    }

    if (pt->data == NULL) {
        return cvector_init(pt, _NumOfElements, pt->size_type);
    }

    temp = realloc(pt->data, pt->size_type * _NumOfElements);
    if (!temp) {
        free(pt->data);
        LOG_ERROR_ON_NULL(temp, "Failed to allocate memory in cvector_reserve !\n");
        return -1;
    }
    pt->data = temp;
    pt->capacity = _NumOfElements;
    return 0;
}

// Resizes the container so that it contains ‘n’  dropping the newest elements in the cvector.
static int cvector_resize(cvector* pt, size_t _NewSize) {
    int temp;
    if (_NewSize > pt->size) {
        temp = cvector_reserve(pt, _NewSize);
        if (temp != 0) {
            return temp;
        }
    }
    pt->size = _NewSize;
    return 0;
}

// Reduces the capacity of the cvector to fit its size and destroys all elements beyond the capacity.
static int cvector_shrink_to_fit(cvector* pt){
    if (pt->size_type <= 0) {
        return -2;
    }
    if (cvector_size(pt) == cvector_capacity(pt)) {
        return 0;
    }
    if (cvector_size(pt) <= 0) {
        free(pt->data);
        pt->data = NULL;
        pt->size = 0;
    }
    else {
        void* temp = realloc(pt->data, pt->size_type * pt->size);
        if (!temp) {
            LOG_ERROR_ON_NULL(temp, "Failed to allocate memory in cvector_shrink_to_fit!\n");
            return -1;
        }
        pt->data = temp;
    }

    pt->capacity = pt->size;
    return 0;
}

// Utility function get the element at intex x
static void* cvector_at(cvector* pt, size_t _Index){
    return &((char*)pt->data)[_Index * pt->size_type];
}

// Utility function set the element at intex x
static void cvector_set(cvector* pt, size_t _Index, const void* _Element){
    memcpy(cvector_at(pt, _Index), _Element, cvector_size_type(pt));
}

// Utility function to insert a number of elements at a specific index
static int cvector_insert(cvector* pt, const void* _Elements, size_t _Pos, size_t _Count){
    char *pos_ptr;
    size_t old_size = cvector_size(pt);
    size_t new_size;
    int tempResponse;

    if (_Pos < old_size){
        new_size = old_size + _Count;
        cvector_resize(pt, new_size);
        pos_ptr = (char*)cvector_at(pt, _Pos);
        memmove(cvector_at(pt, _Pos + _Count), pos_ptr, (old_size - _Pos) * cvector_size_type(pt));
        memmove(pos_ptr, _Elements, _Count * cvector_size_type(pt));
    }
    else{
        new_size = _Pos + _Count;
        tempResponse = cvector_resize(pt, new_size);
        if (tempResponse != 0) {
            return tempResponse;
        }
        pos_ptr = (char*)cvector_at(pt, _Pos);
        memmove(pos_ptr, _Elements, _Count * cvector_size_type(pt));
    }
    return 0;
}

// Assigns a new value to the element, replacing its current contents.
static int cvector_assign(cvector* pt, const void* _Elements,size_t _Pos, size_t _Count) {
    int tempResponse;
    if (_Pos + _Count > cvector_size(pt)) {
        tempResponse = cvector_resize(pt, _Pos + _Count);
        if (tempResponse != 0) {
            return tempResponse;
        }
    }
    memcpy(cvector_at(pt, _Pos), _Elements, _Count * cvector_size_type(pt));
}

// Removes from the vector single or multiple elements
static void cvector_erase(cvector* pt, size_t _Pos, size_t _Count){
    if (_Pos >= cvector_size(pt)){
        return;
    }
    else if (_Pos + _Count >= cvector_size(pt)){
        cvector_resize(pt, _Pos);
    }
    else{
        memmove(cvector_at(pt, _Pos), cvector_at(pt, _Pos + _Count), (cvector_size(pt) - _Pos - _Count) * cvector_size_type(pt));
        cvector_resize(pt, cvector_size(pt) - _Count);
    }
}

// It is used to pop all the elements from the cvector
static void cvector_clear(cvector* pt) {
    cvector_resize(pt, 0);
}

// Utility function to check if the cvector is empty or not
static int cvector_empty(cvector* pt) {
    return cvector_size(pt) == 0;
}

// Utility function to check if the cvector is full or not
static int cvector_full(cvector* pt) {
    return cvector_size(pt) == cvector_capacity(pt);
}

// Utility function to add multiple elements to the end of the cvector
static void cvector_append(cvector* pt, const void* _Element, size_t _NumOfElements){
    if (cvector_size(pt) + _NumOfElements > cvector_capacity(pt)) cvector_reserve(pt, cvector_capacity(pt) + _NumOfElements + cvector_capacity(pt) / 8 + 8);
    memcpy(cvector_at(pt, cvector_size(pt)), _Element, cvector_size_type(pt) * _NumOfElements);
    cvector_resize(pt, cvector_size(pt) + _NumOfElements);
}

// Utility function to get the first element in the cvector
static void* cvector_front(cvector* pt) {
    return cvector_at((cvector*)pt, 0);
}

// Utility function to get the last element in the cvector
static void* cvector_back(cvector* pt) {
    if (cvector_empty(pt)) return NULL;
    return cvector_at(pt, cvector_size(pt) - 1);
}

// Utility function to pop a top element from the cvector
static void cvector_pop_back(cvector* pt){
    if (cvector_empty(pt)) return;
    cvector_resize(pt, cvector_size(pt) - 1);
}

// Utility function to push back an element `x` to the cvector
static void cvector_push_back(cvector* pt, const void* _Element){
    cvector_append(pt, _Element, 1);
}

// Utility function to swap 2 vectors
static void cvector_swap(cvector* _Vector_1, cvector* _Vector_2){
    memswap((char*)_Vector_1, (char*)_Vector_2, sizeof(cvector));
}

static int cvector_isempty(cvector* _cvector) {
    if (cvector_size(_cvector) <= 0) {
        return 1;
    }
    return 0;
}

static int cvector_isvalid(cvector* _cvector) {
    if (cvector_capacity(_cvector) > 0 && cvector_data(_cvector) == NULL) {
        return 0;
    }
    if (cvector_size_type(_cvector) <= 0) {
        return 0;
    }
    if (cvector_size(_cvector) > cvector_capacity(_cvector)) {
        return 0;
    }
    return 1;
}

// return a cloned a string
static cvector cvector_clone(cvector* _Vector){
    cvector clone;
    int tempResponse;
    memset(&clone, 0, sizeof(clone));
    tempResponse = cvector_init(&clone, 0, cvector_size_type(_Vector));
    if (tempResponse != 0) {
        return clone;
    }
    tempResponse = cvector_insert(&clone, cvector_data(_Vector), 0, cvector_size(_Vector));
    if (tempResponse != 0) {
        return clone;
    }
    return clone;
}

static void cvector_print(cvector* pt, void (print_func)(const void* _element)) {
    size_t i;
    for (i = 0; i < cvector_size(pt); i++) {
        printf("[%d] = ", i);
        print_func(cvector_at(pt, i));
        printf("\n");
    }
}
static void cvector_print_int(cvector* pt) {
    cvector_print(pt, print_int);
}

// free the memory of the vector
static void cvector_free(cvector* pt) {
    free(pt->data);
    pt->data = NULL;
    pt->capacity = 0;
    pt->size = 0;
    pt->size_type = 0;
}

// free the memory of an array of vectors
static void cvector_free_array(cvector* pt) {
    size_t i;
    for (i = 0; i < cvector_size(pt); i++) {
        cvector_free((cvector*)cvector_at(pt, i));
    }
}


/***********************************************cstack*************************************************************************/
typedef cvector cstack;

// Utility function to initialize the stack
static int cstack_init(cstack* _Stack, size_t _NumberOfElements, size_t _SizeOfElement) {
    return cvector_init((cvector*)_Stack, _NumberOfElements, _SizeOfElement);
}

// Returns the number of elements in the stack
static size_t cstack_size(cstack* pt) {
    return cvector_size((cvector*)pt);
}

// Returns the maximum number of elements that the stack can hold.
static size_t cstack_capacity(cstack* pt) {
    return cvector_capacity((cvector*)pt);
}

// Requests that the stack capacity be at least enough to contain n elements
static void cstack_reserve(cstack* pt, size_t _NumOfElements) {
    cvector_reserve((cvector*)pt, _NumOfElements);
}

// Resizes the container so that it contains ‘n’  dropping the newest elements in the stack.
static int cstack_resize(cstack* pt, size_t _NewSize) {
    return cvector_resize((cvector*)pt, _NewSize);
}

// Reduces the capacity of the stack to fit its size and destroys all elements beyond the capacity.
static int cstack_shrink_to_fit(cstack* pt) {
    return cvector_shrink_to_fit((cvector*)pt);
}

// It is used to pop all the elements from the stack
static void cstack_clear(cstack* pt) {
    cvector_clear((cvector*)pt);
}

// Utility function to check if the stack is empty or not
static int cstack_empty(cstack* pt) {
    return cvector_empty((cvector*)pt);
}

// Utility function to check if the stack is full or not
static int cstack_full(cstack* pt) {
    return cvector_full((cvector*)pt);
}

// Utility function to return the top element of the stack
static void* cstack_top(cstack* pt) {
    return cvector_back((cvector*)pt);
}

// Utility function to add an element `x` to the stack
static void cstack_push(cstack* pt, const void* _Element) {
    cvector_push_back((cvector*)pt, _Element);
}

// Utility function to pop a top element from the stack
static void cstack_pop(cstack* pt) {
    cvector_pop_back((cvector*)pt);
}

// utility to free the memory allocated
static void cstack_free(cstack* pt) {
    cvector_free((cvector*)pt);
}


/***********************************************cstring*************************************************************************/

typedef cvector cstring;
// if you want to use wchar or other types of chars use cstring_init() setting the parameter _SizeOfChar accordingly to your needs.
// You may cast some functions to your type.

// Utility function to initialize the stack
static int cstring_init(cstring* _Cstring, size_t _NumberOfElements, size_t _SizeOfChar) {
    return cvector_init((cvector*)_Cstring, _NumberOfElements, _SizeOfChar);
}

// Utility function get a pointer to the buffer
static char* cstring_data(cstring* pt) {
    return (char*)cvector_data((cvector*)pt);
}

// Returns the number of elements in the cstring
static size_t cstring_size(cstring* pt) {
    return cvector_size((cvector*)pt);
}

// Returns the maximum number of elements that the cstring can hold.
static size_t cstring_capacity(cstring* pt) {
    return cvector_capacity((cvector*)pt);
}

// Returns the type size.
static size_t cstring_size_type(cstring* pt) {
    return cvector_size_type((cvector*)pt);
}

// Requests that the cstring capacity be at least enough to contain n elements
static int cstring_reserve(cstring* pt, size_t _NumOfElements) {
    return cvector_reserve((cvector*)pt, _NumOfElements);
}

// Resizes the container so that it contains ‘n’  dropping the newest elements in the cstring.
static int cstring_resize(cstring* pt, size_t _NewSize) {
    return cvector_resize((cvector*)pt, _NewSize);
}

// Reduces the capacity of the cstring to fit its size and destroys all elements beyond the capacity.
static int cstring_shrink_to_fit(cstring* pt) {
    return cvector_shrink_to_fit((cvector*)pt);
}

// Utility function get the element at intex x
static char* cstring_at(cvector* pt, size_t _Index) {
    return (char*)cvector_at((cvector*) pt, _Index);
}

// Utility function set the element at intex x
static void cstring_set(cstring* pt, size_t _Index, const void* _Element) {
    cvector_set((cvector*)pt, _Index, _Element);
}

// Utility function to insert a number of elements at a specific index
static int cstring_insert(cstring* pt, const void* _Elements, size_t _Pos, size_t _Count) {
    return cvector_insert((cvector*)pt, _Elements, _Pos, _Count);
}

// Assigns a new value to the string, replacing its current contents.
static int cstring_assign(cstring* pt, const void* _Elements, size_t _Pos, size_t _Count) {
    return cvector_assign((cvector*)pt, _Elements, _Pos, _Count);
}

// Take as input a 'C type' string and returns a cstring
static cstring cstring_create(const char* _String)
{
    cstring temp;
    cstring_init(&temp, 0, sizeof(char));
    cstring_assign(&temp, _String, 0, strlen(_String));
    return temp;
}

// Removes from the string single or multiple characters
static void cstring_erase(cstring* pt, size_t _Pos, size_t _Count) {
    cvector_erase((cvector*) pt, _Pos, _Count);
}

// It is used to pop all the elements from the cstring
static void cstring_clear(cstring* pt) {
    cvector_clear((cvector*)pt);
}

// Utility function to check if the cstring is empty or not
static int cstring_empty(cstring* pt) {
    return cvector_empty((cvector*)pt);
}

// Utility function to check if the cstring is full or not
static int cstring_full(cstring* pt) {
    return cvector_full((cvector*)pt);
}

// Utility function to add multiple elements to the end of the string
static void cstring_append(cstring* pt, const char* _Characters, size_t _Strlen) {
    cvector_append((cvector*)pt, _Characters, _Strlen);
}

//// Utility function to add the _Source str to the end of the _Destination str
static void cstring_strcat_cstr(cstring* pt, const char* _String) {
    cstring_append(pt, _String, strlen(_String));
}

//// Utility function to add the _Source str to the end of the _Destination str
static void cstring_strcat(cstring* _Destination, cstring* _Source) {
    cstring_append(_Destination, cstring_data(_Source), cstring_size(_Source));
}

// Utility function to get the first element in the string
static char* cstring_front(cstring* pt) {
    return (char*)cvector_front((cvector*) pt);
}

// Utility function to get the last element in the string
static char* cstring_back(cstring* pt) {
    return (char*)cvector_back((cvector*)pt);
}

// Utility function to add an element `x` to the cstring
static void cstring_push_back(cstring* pt, const void* _Element) {
    cvector_push_back((cvector*)pt, _Element);
}

// Utility function to pop a top element from the cstring
static void cstring_pop_back(cstring* pt) {
    cvector_pop_back((cvector*)pt);
}

// Utility function to swap 2 vectors
static void cstring_swap(cstring* _String_1, cstring* _String_2) {
    cvector_swap((cvector*)_String_1, (cvector*)_String_2);
}

// return a cloned a string
static cstring cstring_clone(cstring* _String) {
    return cvector_clone((cvector*) _String);
}

// Utility function that return the string with a null termination character
static char* cstring_cstr(cstring* pt) {
    if (cstring_capacity(pt) <= cstring_size(pt)) cstring_reserve(pt, cstring_size(pt) + 1);
    //cstring_push_back(pt, "\0");
    cstring_resize(pt, cstring_size(pt) + 1);
    memset(cstring_back(pt), 0, cstring_size_type(pt));
    cstring_resize(pt, cstring_size(pt) - 1);
    return (char*)cstring_data(pt);
}

// Returns a newly constructed string object with its value initialized to a copy of a substring of this object.
static cstring cstring_substr(cstring* pt, size_t pos, size_t len) {
    cstring temp;
    cstring_init(&temp, 0, cstring_size_type(pt));

    if (pos >= cstring_size(pt));
    else{
        cstring_assign(&temp, cstring_at(pt, pos), 0, MIN(cstring_size(pt) - pos, len));
    }
    return temp;
}

// Function that compare 2 strings and return: 1 (s1 > s2), 0 (s1 == s2), -1 (s1 < s2)
static int cstring_compare(cvector* _String_1, cvector* _String_2)
{
    if (cstring_size(_String_1) > cstring_size(_String_2)) return 1;
    else if (cstring_size(_String_1) < cstring_size(_String_2)) return -1;
    return memcmp(cstring_data(_String_1), cstring_data(_String_2), cstring_size(_String_1));
}

// Searches the string pt for the first occurrence of the sequence specified by its arguments. retunr [(size_t)-1] on fail
static size_t cstring_find(cstring* pt, const char* _String, size_t pos, size_t _StringLen) {
    if (pos + _StringLen > cstring_size(pt)) return (size_t)-1;
    size_t max_index = cstring_size(pt) - _StringLen;
    size_t i;
    for (i = pos; i <= max_index; i++)
    {
        if (memcmp(_String, cstring_at(pt, i), _StringLen) == 0) {
            return i;
        }
    }
    return (size_t)-1;
}

// return the number of matching characters starting from the offset, stops at the first difference
static size_t cstring_matching_chars(cstring* _Source, cvector* _String2, size_t _SourceOffset) {
    size_t i;
    for (i = 0; i < cstring_size(_Source) - _SourceOffset; i++) {
        if (memcmp(cstring_at(_Source, i + _SourceOffset), cstring_at(_String2, i), cstring_size_type(_Source)) != 0) break;
    }
    return i;
}

// Compares a string with an array of substrings and returns the best substring match
static cstring* cstring_best_match(cstring* _Source, cvector* _ArrayOfStrings, size_t _SourceOffset)
{
    if (_SourceOffset >= cstring_size(_Source)) return NULL;
    size_t i;
    cstring* match, *iter;
    match = NULL;
    for (i = 0; i < cvector_size(_ArrayOfStrings); i++)
    {
        iter = (cstring*)cvector_at(_ArrayOfStrings, i);
        if (memcmp(cstring_data(_Source) + _SourceOffset, cstring_data(iter), cstring_size(iter)) == 0)
        {
            if (match == NULL) match = iter;
            else if (cstring_size(iter) > cstring_size(match)) match = iter;
        }
    }
    return match;
}

static cstring cstring_Base64encode(const void* _ToEncode, size_t _NunOfBytesToEncode)
{
    cstring encoded_data;
    size_t encoded_size;
    cstring_init(&encoded_data, Base64encode_len(_NunOfBytesToEncode), sizeof(char));
    encoded_size = Base64encode(cstring_data(&encoded_data), _ToEncode, _NunOfBytesToEncode);
    cstring_resize(&encoded_data, encoded_size);
    return encoded_data;
}
static cstring cstring_Base64decode(const char* _ToDecode, size_t _NunOfBytesToDecode)
{
    cstring decoded_data;
    size_t nprbytes, decoded_size;

    nprbytes = Base64Decode_nprbytes(_ToDecode, _NunOfBytesToDecode);
    decoded_size = Base64decode_len(nprbytes);
    cstring_init(&decoded_data, decoded_size, sizeof(char));
    decoded_size = Base64decode(cstring_data(&decoded_data), _ToDecode, nprbytes);
    cstring_resize(&decoded_data, decoded_size);
    cstring_shrink_to_fit(&decoded_data);
    return decoded_data;
}

static void cstring_Base64encode_s(cstring* _Encoded, const void* _ToEncode, size_t _NunOfBytesToEncode)
{
    cstring* encoded_data = _Encoded;
    size_t encoded_size = Base64encode_len(_NunOfBytesToEncode);
    cstring_reserve(encoded_data, encoded_size / cstring_size_type(encoded_data) + ((encoded_size % cstring_size_type(encoded_data)) > 0));
    encoded_size = Base64encode(cstring_data(encoded_data), _ToEncode, _NunOfBytesToEncode);
    cstring_resize(encoded_data, encoded_size / cstring_size_type(encoded_data) + ((encoded_size % cstring_size_type(encoded_data)) > 0));
}
static void cstring_Base64decode_s(cstring* _Decoded, const char* _ToDecode, size_t _NunOfBytesToDecode)
{
    cstring* decoded_data = _Decoded;
    size_t nprbytes, decoded_size;

    nprbytes = Base64Decode_nprbytes(_ToDecode, _NunOfBytesToDecode);
    decoded_size = Base64decode_len(nprbytes);
    cstring_reserve(decoded_data, decoded_size / cstring_size_type(decoded_data) + ((decoded_size % cstring_size_type(decoded_data)) > 0));
    decoded_size = Base64decode(cstring_data(decoded_data), _ToDecode, nprbytes);
    cstring_resize(decoded_data, decoded_size / cstring_size_type(decoded_data) + ((decoded_size % cstring_size_type(decoded_data)) > 0));
}

// utility to free the memory allocated
static void cstring_free(cstring* pt) {
    cvector_free((cvector*)pt);
}

#endif // !__CVECTOR_H__
