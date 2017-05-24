#include "testing.h"
#include "sglib.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#define MSIZE 1000000

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
    node_t* tree = NULL;
    node_t* node;
    clock_t start, end;
    double cpu_time_used = 0;
    (void)(cpu_time_used);
     
    for(int i = 0; i < MSIZE; i++) {
        node = &mnodes[i];
        my_node_init(node);
        rb_value_m(node) = rand();
    }
    printf("\"clock\" \"rbtree\" \"rbtree (log)\"\n");
    start = clock();
    for(int i = 0; i < MSIZE; i++) {
        my_insert(&tree, &mnodes[i]);
        if((i % 10000) == 0) {
            end = clock();
            cpu_time_used = (double) (end - start);
            printf("%d %f %f\n", i, cpu_time_used, log(cpu_time_used));
            start = clock();
        }
    }
    tree = NULL;
    for(int i = 0; i < MSIZE; i++) {
        node = &mnodes[i];
        my_node_init(node);
    }
    printf("\n\n\"clock\" \"sglib\" \"sglib (log)\"\n");
    start = clock();
    for(int i = 0; i < MSIZE; i++) {
        sglib_node_t_add(&tree, &mnodes[i]);
        if((i % 10000) == 0) {
            end = clock();
            cpu_time_used = (double) (end - start);
            printf("%d %f %f\n", i, cpu_time_used, log(cpu_time_used));
            start = clock();
        }
    }
    return 0;
}
