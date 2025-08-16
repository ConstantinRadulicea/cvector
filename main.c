#include "cvector.h"
#include "cstring.h"
#include "binarysearchtree.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "PPP_custom.h"


#define MB 1048576      // 1 megabyte in bytes = 2^20

int cmp_int(int* num1, int* num2, void* ctx) {
    if (*num1 > *num2) {
        return 1;
    }
    else if (*num1 < *num2) {
        return -1;
    }
    else {
        return 0;
    }
}

int print_int2(binarysearchtree_node *node, void* ctx) {
    printf("%d\n", *((int*)(node->data)));
    //if (*((int*)(node->data)) == 5245)
    //{
    //    return 1;
    //}
    return 0;
}

free_node_routine(binarysearchtree_node* node, void* ctx) {
    print_int2(node, ctx);
    free(node->data);
    free(node);
}


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

    CSTRING_DEFAULT(temp_str);
    cstring_create(&temp_str, "mi");
    cvector_push_back(&str_vector, &temp_str);

    CSTRING_DEFAULT(temp_str);
    cstring_create(&temp_str, "mi ");
    cvector_push_back(&str_vector, &temp_str);

    CSTRING_DEFAULT(temp_str);
    cstring_create(&temp_str, "mi ch");
    cvector_push_back(&str_vector, &temp_str);

    CSTRING_DEFAULT(temp_str);
    cstring_create(&temp_str, "Ciao mi chiamo louis! 12");
    cvector_push_back(&str_vector, &temp_str);
    CSTRING_DEFAULT(temp_str);

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

    printf("%s\n", cstring_cstr(&str1));

    cstring_free(&str1);
}


void test_binarysearchtree_iterator2() {
    binarysearchtree tree;
    binarysearchtree_node* node, * node1;
    int* num, * num1, tempnum;
    binarysearchtree_init(&tree, cmp_int, NULL);


    srand(time(NULL));   // Initialization, should only be called once.

    //for (size_t i = 0; i < 100; i++)
    //{
    //    num = (int*)malloc(sizeof(int));
    //    node = (binarysearchtree_node*)malloc(sizeof(binarysearchtree_node));
    //    *num = rand();      // Returns a pseudo-random integer between 0 and RAND_MAX
    //    binarysearchtree_node_init(node, num);
    //    binarysearchtree_insert(&tree, node);
    //}


    num = (int*)malloc(sizeof(int));
    node = (binarysearchtree_node*)malloc(sizeof(binarysearchtree_node));
    (*num) = 2852;
    binarysearchtree_node_init(node, num);
    printf("node: %p num: %p num_val: %d\n", node, num, *num);
    binarysearchtree_insert(&tree, node);


    num = (int*)malloc(sizeof(int));
    node = (binarysearchtree_node*)malloc(sizeof(binarysearchtree_node));
    (*num) = 2211;
    binarysearchtree_node_init(node, num);
    printf("node: %p num: %p num_val: %d\n", node, num, *num);
    binarysearchtree_insert(&tree, node);


    num = (int*)malloc(sizeof(int));
    node = (binarysearchtree_node*)malloc(sizeof(binarysearchtree_node));
    (*num) = 9330;
    binarysearchtree_node_init(node, num);
    printf("node: %p num: %p num_val: %d\n", node, num, *num);
    binarysearchtree_insert(&tree, node);


    num = (int*)malloc(sizeof(int));
    node = (binarysearchtree_node*)malloc(sizeof(binarysearchtree_node));
    (*num) = 2579;
    binarysearchtree_node_init(node, num);
    printf("node: %p num: %p num_val: %d\n", node, num, *num);
    binarysearchtree_insert(&tree, node);


    num = (int*)malloc(sizeof(int));
    node = (binarysearchtree_node*)malloc(sizeof(binarysearchtree_node));
    (*num) = 7585;
    binarysearchtree_node_init(node, num);
    printf("node: %p num: %p num_val: %d\n", node, num, *num);
    binarysearchtree_insert(&tree, node);


    num = (int*)malloc(sizeof(int));
    node = (binarysearchtree_node*)malloc(sizeof(binarysearchtree_node));
    (*num) = 5576;
    binarysearchtree_node_init(node, num);
    printf("node: %p num: %p num_val: %d\n", node, num, *num);
    binarysearchtree_insert(&tree, node);


    num = (int*)malloc(sizeof(int));
    node = (binarysearchtree_node*)malloc(sizeof(binarysearchtree_node));
    (*num) = 9938;
    binarysearchtree_node_init(node, num);
    printf("node: %p num: %p num_val: %d\n", node, num, *num);
    binarysearchtree_insert(&tree, node);


    num = (int*)malloc(sizeof(int));
    node = (binarysearchtree_node*)malloc(sizeof(binarysearchtree_node));
    (*num) = 8292;
    binarysearchtree_node_init(node, num);
    printf("node: %p num: %p num_val: %d\n", node, num, *num);
    binarysearchtree_insert(&tree, node);


    num = (int*)malloc(sizeof(int));
    node = (binarysearchtree_node*)malloc(sizeof(binarysearchtree_node));
    (*num) = 4882;
    binarysearchtree_node_init(node, num);
    printf("node: %p num: %p num_val: %d\n", node, num, *num);
    binarysearchtree_insert(&tree, node);


    num = (int*)malloc(sizeof(int));
    node = (binarysearchtree_node*)malloc(sizeof(binarysearchtree_node));
    (*num) = 5245;
    binarysearchtree_node_init(node, num);
    printf("node: %p num: %p num_val: %d\n", node, num, *num);
    binarysearchtree_insert(&tree, node);

    num = (int*)malloc(sizeof(int));
    node = (binarysearchtree_node*)malloc(sizeof(binarysearchtree_node));
    (*num) = 9000;
    binarysearchtree_node_init(node, num);
    printf("node: %p num: %p num_val: %d\n", node, num, *num);
    binarysearchtree_insert(&tree, node);

    printf("binarysearchtree_inorder_iterator\n");
    binarysearchtree_inorder_iterator(&tree, print_int2, NULL);
    printf("binarysearchtree_preorder_iterator\n");
    binarysearchtree_preorder_iterator(&tree, print_int2, NULL);
    printf("binarysearchtree_postorder_iterator\n");
    binarysearchtree_postorder_iterator(&tree, print_int2, NULL);

    printf("\nMAX num: ");
    print_int2(binarysearchtree_max(&tree), NULL);

    printf("\nMIN num: ");
    print_int2(binarysearchtree_min(&tree), NULL);

    // letete head
    tempnum = 2852;
    printf("\ndelete num: ");
    print_int2(binarysearchtree_delete(&tree, &tempnum), NULL);
    binarysearchtree_inorder_iterator(&tree, print_int2, NULL);

    tempnum = 7585;
    printf("\nfound num: ");
    print_int2(binarysearchtree_find(&tree, &tempnum), NULL);
    binarysearchtree_inorder_iterator(&tree, print_int2, NULL);

    tempnum = 7585;
    printf("\ndelete num: ");
    print_int2(binarysearchtree_delete(&tree, &tempnum), NULL);
    binarysearchtree_inorder_iterator(&tree, print_int2, NULL);

    tempnum = 5576;
    printf("\ndelete num: ");
    print_int2(binarysearchtree_delete(&tree, &tempnum), NULL);
    binarysearchtree_inorder_iterator(&tree, print_int2, NULL);
    binarysearchtree_delete_tree(&tree, free_node_routine, NULL);
}


void test_binarysearchtree_iterator() {
    binarysearchtree tree;
    binarysearchtree_node* node, * node1;
    int* num, * num1;
    binarysearchtree_init(&tree, cmp_int, NULL);


    srand(time(NULL));   // Initialization, should only be called once.

    for (size_t i = 0; i < 1000; i++)
    {
        num = (int*)malloc(sizeof(int));
        node = (binarysearchtree_node*)malloc(sizeof(binarysearchtree_node));
        *num = rand();      // Returns a pseudo-random integer between 0 and RAND_MAX
        //*num = i;
        printf("node: %p num: %p num_val: %d\n", node, num, *num);
        binarysearchtree_node_init(node, num);
        binarysearchtree_insert(&tree, node);
    }

    binarysearchtree_inorder_iterator(&tree, print_int2, NULL);
    binarysearchtree_delete_tree(&tree, free_node_routine, NULL);
}

void test_binarysearchtree_delete_tree() {
    binarysearchtree tree;
    binarysearchtree_node* node, * node1;
    int* num, * num1;
    binarysearchtree_init(&tree, cmp_int, NULL);


    srand(time(NULL));   // Initialization, should only be called once.

    for (size_t i = 0; i < 100000; i++)
    {
        num = (int*)malloc(sizeof(int));
        node = (binarysearchtree_node*)malloc(sizeof(binarysearchtree_node));
        *num = rand();      // Returns a pseudo-random integer between 0 and RAND_MAX
        //*num = i;
        printf("node: %p num: %p num_val: %d\n", node, num, *num);
        binarysearchtree_node_init(node, num);
        binarysearchtree_insert(&tree, node);
    }
    binarysearchtree_delete_tree(&tree, free_node_routine, NULL);
}



int main() {
    //test_cvector_shift_left();
    //test_cvector();
    //test_base64();
    //test_binarysearchtree_iterator();
    //test_binarysearchtree_iterator2();
    //test_binarysearchtree_delete_tree();

    //ppp_test_encode_decode();
    //ppp_stream_test();
    //ppp_custom_test_receive();
	tcp_test_1();
    return 0;
}

