#include "cvector.h"
#include "cstring.h"
#include <stdio.h>



#define MB 1048576      // 1 megabyte in bytes = 2^20

void cvector_print(cvector* pt, void (print_func)(const void* _element)) {
    size_t i;
    for (i = 0; i < cvector_size(pt); i++) {
        printf("[%d] = ", i);
        print_func(cvector_at(pt, i));
        printf("\n");
    }
}

void print_int(const void* _Element) {
    printf("%d", *((int*)_Element));
}
void cvector_print_int(cvector* pt) {
    cvector_print(pt, print_int);
}


void test_base64()
{
    char str3[] = "Ciao mi chiamo louis";
    int gg = 1;
    cstring temp_str, temp_str2;
    CSTRING_DEFAULT(temp_str);
    CSTRING_DEFAULT(temp_str2);
    cstring_init(&temp_str, 0, 1);
    cstring_init(&temp_str2, 0, 1);


    //for (size_t i = 0; i < 256; i++)
    //{
    //    printf("original int:    {%d}\n", i);
    //    cstring_Base64encode(&temp_str, &i, sizeof(i));
    //    printf("encoded:    {%s}\n", cstring_cstr(&temp_str));
    //    cstring_Base64decode(&temp_str2, cstring_data(&temp_str), cstring_size(&temp_str));
    //    printf("decoded int:    {%d}\n", *((int*)(temp_str2.data)));
    //    if (i != *((size_t*)(temp_str2.data))) break;
    //    printf("\n\n");
    //}
    printf("original:   {%s}\n", str3);
    cstring_Base64encode(&temp_str, str3, strlen(str3));
    printf("encoded:    {%s}\n", cstring_cstr(&temp_str));
    cstring_Base64decode(&temp_str2, cstring_data(&temp_str), cstring_size(&temp_str));
    printf("decoded:    {%s}\n", cstring_cstr(&temp_str2));
    cvector_free(&temp_str2);
    cvector_free(&temp_str);
}

int test_cvector()
{
    char str3[] = "Ciao mi chiamo louis! ";
    //Base64encode_left(str3, strlen(str3));
    printf("\n\n");
    char str[] = "Ciao mi chiamo louis! ";
    cvector str_vector;
    cstring temp_str, clone;
    CVECTOR_DEFAULT(str_vector);
    CSTRING_DEFAULT(temp_str);
    CSTRING_DEFAULT(clone);
    cvector_init(&str_vector, 0, sizeof(cstring));

    cstring_create(&temp_str, "mi");
    cstring_clone(&clone, &temp_str);
    cvector_push_back(&str_vector, &clone);
    CSTRING_DEFAULT(clone);

    cstring_create(&temp_str, "mi ");
    cstring_clone(&clone, &temp_str);
    cvector_push_back(&str_vector, &clone);
    CSTRING_DEFAULT(clone);

    cstring_create(&temp_str, "mi ch");
    cstring_clone(&clone, &temp_str);
    cvector_push_back(&str_vector, &clone);
    CSTRING_DEFAULT(clone);

    cstring_create(&temp_str, "Ciao mi chiamo louis! 12");
    cstring_clone(&clone, &temp_str);
    cvector_push_back(&str_vector, &clone);
    CSTRING_DEFAULT(clone);

    printf("%s\n", cstring_cstr(&temp_str));

    cstring str1, str2;
    CSTRING_DEFAULT(str1);
    CSTRING_DEFAULT(str2);
    cstring_create(&str1, str);
    cstring_create(&str2, str3);
    cstring_create(&temp_str, str3);


    printf("%s\n", cstring_cstr(&str1));
    printf("%s\n", cstring_cstr(&str2));
    printf("%d\n", cstring_compare(&str1, &str2));
    printf("%s\n", cstring_cstr(&temp_str));

    cvector_free(&str1);
    cvector_free(&str2);
    cvector_free_array(&str_vector);
    cvector_free(&str_vector);
    cstring_free(&temp_str);

    return 0;
}

void test_cvector_shift_left() {
    char str[] = "ciao sono louis";
    cstring str1, str2;
    CSTRING_DEFAULT(str1);
    CSTRING_DEFAULT(str2);
    cstring_create(&str1, str);
    
    cvector_shift_left(&str1, 0, 2);

    printf("%s", cstring_cstr(&str1));

    cstring_free(&str1);
}



int main() {
    test_cvector_shift_left();
    test_cvector();
    test_base64();
    return 0;
}

