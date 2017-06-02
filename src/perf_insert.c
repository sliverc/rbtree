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
        rb_value_m(node) = rand() / 8;
    }
    fprintf(stderr, "rbtree\n");
    printf("\"rbtree\"\n");
    start = clock();
    for(int i = 0; i < MSIZE; i++) {
        my_insert(&tree, &mnodes[i]);
        if(((i + 1) % 10000) == 0) {
            end = clock();
            cpu_time_used = (double) (end - start);
            printf("%d %f\n", i, cpu_time_used);
            start = clock();
        }
    }
    fprintf(stderr, "prepare: ");
    tree = NULL;
    for(int i = 0; i < MSIZE; i++) {
        node = &mnodes[i];
        my_node_init(node);
    }
    fprintf(stderr, "sglib\n");
    printf("\n\n\"sglib\"\n");
    start = clock();
    for(int i = 0; i < MSIZE; i++) {
        sglib_node_t_add(&tree, &mnodes[i]);
        if(((i + 1) % 10000) == 0) {
            end = clock();
            cpu_time_used = (double) (end - start);
            printf("%d %f\n", i, cpu_time_used);
            start = clock();
        }
    }
    printf("\n\n");
    return 0;
}
