#include "testing.h"
#include "sglib.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#define MSIZE 10000000

node_t mnodes[MSIZE];

SGLIB_DEFINE_RBTREE_PROTOTYPES(
    node_t,
    left,
    right,
    color,
    rb_value_cmp_m
)
SGLIB_DEFINE_RBTREE_FUNCTIONS(
    node_t,
    left,
    right,
    color,
    rb_value_cmp_m
)

int
main(void)
{
    srand(time(NULL));
    node_t* tree;
    my_tree_init(&tree);
    node_t* node;
    node_t* key;
    clock_t start, end;
    double cpu_time_used = 0.1;
    (void)(cpu_time_used);
    fprintf(stderr, "preheat: ");
    for(int i = 0; i < 200000000; i++)
        cpu_time_used = cpu_time_used * cpu_time_used;
    fprintf(stderr, "%d\n", (int) cpu_time_used);
    fprintf(stderr, "prepare: ");
    for(int i = 0; i < MSIZE; i++) {
        node = &mnodes[i];
        my_node_init(node);
    }
    for(int i = 0; i < MSIZE; i++) {
        node = &mnodes[i];
        rb_value_m(node) = rand() / 8;
        while(my_insert(&tree, node) != 0)
            rb_value_m(node) = rand() / 8;
    }
    fprintf(stderr, "rbtree_delete_node\n");
    printf("\"rbtree_delete_node\"\n");
    start = clock();
    for(int i = 0; i < MSIZE; i++) {
        node = &mnodes[i];
        my_delete_node(&tree, node);
        if(((i + 1) % 10000) == 0) {
            end = clock();
            cpu_time_used = (double) (end - start);
            printf("%d %f\n", i, cpu_time_used);
            start = clock();
        }
    }
    fprintf(stderr, "prepare: ");
    assert(tree = my_nil_ptr);
    for(int i = 0; i < MSIZE; i++) {
        node = &mnodes[i];
        if(rb_value_m(node) != 0)
            my_insert(&tree, node);
    }
    fprintf(stderr, "rbtree_delete\n");
    printf("\n\n\"rbtree_delete\"\n");
    start = clock();
    for(int i = 0; i < MSIZE; i++) {
        key = &mnodes[i];
        my_find(tree, key, &node);
        my_delete_node(&tree, node);
        if(((i + 1) % 10000) == 0) {
            end = clock();
            cpu_time_used = (double) (end - start);
            printf("%d %f\n", i, cpu_time_used);
            start = clock();
        }
    }
    assert(tree = my_nil_ptr);
    fprintf(stderr, "prepare: ");
    tree = NULL;
    for(int i = 0; i < MSIZE; i++) {
        node = &mnodes[i];
        rb_color_m(node) = 0;
        rb_parent_m(node) = NULL;
        rb_left_m(node) = NULL;
        rb_right_m(node) = NULL;
        sglib_node_t_add(&tree, node);
    }
    fprintf(stderr, "sglib\n");
    printf("\n\n\"sglib\"\n");
    start = clock();
    for(int i = 0; i < MSIZE; i++) {
        node = &mnodes[i];
        sglib_node_t_delete(&tree, node);
        if(((i + 1) % 10000) == 0) {
            end = clock();
            cpu_time_used = (double) (end - start);
            printf("%d %f\n", i, cpu_time_used);
            start = clock();
        }
    }
    assert(tree == NULL);
    printf("\n\n");
    return 0;
}
