#include "cstring.h"
#include "base64.h"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))



/***********************************************cstring*************************************************************************/


int cstring_init(cstring* _Cstring, size_t _NumberOfElements, size_t _SizeOfChar) {
    return cvector_init((cvector*)_Cstring, _NumberOfElements, _SizeOfChar);
}

char* cstring_data(cstring* pt) {
    return (char*)cvector_data((cvector*)pt);
}

size_t cstring_size(cstring* pt) {
    return cvector_size((cvector*)pt);
}

size_t cstring_capacity(cstring* pt) {
    return cvector_capacity((cvector*)pt);
}

// Returns the type size.
size_t cstring_size_type(cstring* pt) {
    return cvector_size_type((cvector*)pt);
}

int cstring_reserve(cstring* pt, size_t _NumOfElements) {
    return cvector_reserve((cvector*)pt, _NumOfElements);
}

int cstring_resize(cstring* pt, size_t _NewSize) {
    return cvector_resize((cvector*)pt, _NewSize);
}

int cstring_shrink_to_fit(cstring* pt) {
    return cvector_shrink_to_fit((cvector*)pt);
}

char* cstring_at(cvector* pt, size_t _Index) {
    return (char*)cvector_at((cvector*)pt, _Index);
}

int cstring_set(cstring* pt, size_t _Index, const void* _Element) {
    return cvector_set((cvector*)pt, _Index, _Element);
}

int cstring_insert(cstring* pt, const void* _Elements, size_t _Pos, size_t _Count) {
    return cvector_insert((cvector*)pt, _Elements, _Pos, _Count);
}

int cstring_assign(cstring* pt, const void* _Elements, size_t _Pos, size_t _Count) {
    return cvector_assign((cvector*)pt, _Elements, _Pos, _Count);
}

int cstring_create(cstring* _dst_string, const char* _String) {
    if (_dst_string == NULL) {
        return CVECTOR_ERROR_INVALID_PARAMETERS;
    }
    cstring_init(_dst_string, 0, sizeof(char));
    return cstring_assign(_dst_string, _String, 0, strlen(_String));
}

void cstring_erase(cstring* pt, size_t _Pos, size_t _Count) {
    cvector_erase((cvector*)pt, _Pos, _Count);
}

void cstring_clear(cstring* pt) {
    cvector_clear((cvector*)pt);
}

int cstring_empty(cstring* pt) {
    return cvector_empty((cvector*)pt);
}

int cstring_full(cstring* pt) {
    return cvector_full((cvector*)pt);
}

int cstring_append(cstring* pt, const char* _Characters, size_t _Strlen) {
    return cvector_append((cvector*)pt, _Characters, _Strlen);
}

int cstring_strcat_cstr(cstring* pt, const char* _String) {
    return cstring_append(pt, _String, strlen(_String));
}

int cstring_strcat(cstring* _Destination, cstring* _Source) {
    return cstring_append(_Destination, cstring_data(_Source), cstring_size(_Source));
}

char* cstring_front(cstring* pt) {
    return (char*)cvector_front((cvector*)pt);
}

char* cstring_back(cstring* pt) {
    return (char*)cvector_back((cvector*)pt);
}

int cstring_push_back(cstring* pt, const void* _Element) {
    return cvector_push_back((cvector*)pt, _Element);
}

void cstring_pop_back(cstring* pt) {
    cvector_pop_back((cvector*)pt);
}

void cstring_swap(cstring* _String_1, cstring* _String_2) {
    cvector_swap((cvector*)_String_1, (cvector*)_String_2);
}

int cstring_clone(cstring* _dst_String, cstring* _src_String) {
    return cvector_clone((cvector*)_dst_String, (cvector*)_src_String);
}

char* cstring_cstr(cstring* pt) {
    if (cstring_capacity(pt) <= cstring_size(pt)) cstring_reserve(pt, cstring_size(pt) + 1);
    //cstring_push_back(pt, "\0");
    if (cstring_resize(pt, cstring_size(pt) + 1) != CVECTOR_SUCCESS) {
        return NULL;
    }
    memset(cstring_back(pt), 0, cstring_size_type(pt));
    cstring_resize(pt, cstring_size(pt) - 1);
    return (char*)cstring_data(pt);
}

int cstring_substr(cstring* _dst_string, cstring* _src_string, size_t pos, size_t len) {
    int tempResponse;

    if (_dst_string == NULL || _src_string == NULL) {
        return CVECTOR_ERROR_INVALID_PARAMETERS;
    }

    cstring_init(_dst_string, 0, cstring_size_type(_src_string));
    if (pos >= cstring_size(_src_string)) {
        return CVECTOR_SUCCESS;
    }
    else {
        tempResponse = cstring_assign(_dst_string, cstring_at(_src_string, pos), 0, MIN(cstring_size(_src_string) - pos, len));
        if (tempResponse != CVECTOR_SUCCESS) {
            memset(_dst_string, 0, sizeof(cstring));
            return tempResponse;
        }
    }
    return CVECTOR_SUCCESS;
}

int cstring_compare(cvector* _String_1, cvector* _String_2)
{
    if (cstring_size(_String_1) > cstring_size(_String_2)) return 1;
    else if (cstring_size(_String_1) < cstring_size(_String_2)) return -1;
    return memcmp(cstring_data(_String_1), cstring_data(_String_2), cstring_size(_String_1));
}

size_t cstring_find(cstring* pt, const char* _String, size_t pos, size_t _StringLen) {
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

size_t cstring_matching_chars(cstring* _Source, cvector* _String2, size_t _SourceOffset) {
    size_t i;
    for (i = 0; i < cstring_size(_Source) - _SourceOffset; i++) {
        if (memcmp(cstring_at(_Source, i + _SourceOffset), cstring_at(_String2, i), cstring_size_type(_Source)) != 0) break;
    }
    return i;
}

int cstring_Base64encode(cstring* _Encoded, const void* _ToEncode, size_t _NunOfBytesToEncode)
{
    cstring* encoded_data = _Encoded;
    size_t encoded_size = Base64encode_len(_NunOfBytesToEncode);
    int tempResponse;

    if (_Encoded == NULL || _ToEncode == NULL) {
        return CVECTOR_ERROR_INVALID_PARAMETERS;
    }

    tempResponse = cstring_reserve(encoded_data, encoded_size / cstring_size_type(encoded_data) + ((encoded_size % cstring_size_type(encoded_data)) > 0));
    if (tempResponse != CVECTOR_SUCCESS) {
        return tempResponse;
    }

    encoded_size = Base64encode(cstring_data(encoded_data), _ToEncode, _NunOfBytesToEncode);

    tempResponse = cstring_resize(encoded_data, encoded_size / cstring_size_type(encoded_data) + ((encoded_size % cstring_size_type(encoded_data)) > 0));
    if (tempResponse != CVECTOR_SUCCESS) {
        return tempResponse;
    }

    return CVECTOR_SUCCESS;
}

int cstring_Base64decode(cstring* _Decoded, const char* _ToDecode, size_t _NunOfBytesToDecode)
{
    cstring* decoded_data = _Decoded;
    size_t nprbytes, decoded_size;
    int tempResponse;

    if (_Decoded == NULL || _ToDecode == NULL) {
        return CVECTOR_ERROR_INVALID_PARAMETERS;
    }

    nprbytes = Base64Decode_nprbytes(_ToDecode, _NunOfBytesToDecode);
    decoded_size = Base64decode_len(nprbytes);

    tempResponse = cstring_reserve(decoded_data, decoded_size / cstring_size_type(decoded_data) + ((decoded_size % cstring_size_type(decoded_data)) > 0));
    if (tempResponse != CVECTOR_SUCCESS) {
        return tempResponse;
    }

    decoded_size = Base64decode(cstring_data(decoded_data), _ToDecode, nprbytes);


    tempResponse = cstring_resize(decoded_data, decoded_size / cstring_size_type(decoded_data) + ((decoded_size % cstring_size_type(decoded_data)) > 0));
    if (tempResponse != CVECTOR_SUCCESS) {
        return tempResponse;
    }

    return CVECTOR_SUCCESS;
}

void cstring_free(cstring* pt) {
    cvector_free((cvector*)pt);
}
