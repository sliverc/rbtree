#include "testing.h"
#include "sglib.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#define MSIZE 100

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
    int r;
    srand(time(NULL));
    node_t* tree;
    my_tree_init(&tree);
    node_t* node;
    node_t* memb;
    node_t  repl;
    my_node_init(&repl);
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
    fprintf(stderr, "replace_node\n");
    printf("\"replace_node\"\n");
    for(int i = 1; i < MSIZE; i++) {
        node = &mnodes[i];
        rb_value_m(node) = rand() / 8;
        while(my_insert(&tree, node) != 0)
            rb_value_m(node) = rand() / 8;
        r = rand() % i;
        node = &mnodes[r];
        start = clock();
        for(int n = 0; n < 10000; n++) {
            my_replace_node(&tree, node, &repl);
            my_replace_node(&tree, &repl, node);
        }
        end = clock();
        cpu_time_used = (double) (end - start);
        printf("%d %f\n", i, cpu_time_used);
    }
    fprintf(stderr, "prepare: ");
    for(int i = 0; i < MSIZE; i++) {
        node = &mnodes[i];
        rb_color_m(node) = 0;
        rb_parent_m(node) = NULL;
        rb_left_m(node) = NULL;
        rb_right_m(node) = NULL;
    }
    tree = NULL;
    fprintf(stderr, "delete_add\n");
    printf("\n\n\"delete_add\"\n");
    for(int i = 0; i < MSIZE; i++) {
        node = &mnodes[i];
        rb_value_m(node) = rand() / 8;
        while(sglib_node_t_add_if_not_member(
                &tree, node, &memb
        ) == 0)
            rb_value_m(node) = rand() / 8;
        start = clock();
        if(i == 0)
            r = 0;
        else
            r = rand() % i;
        node = &mnodes[r];
        start = clock();
        for(int n = 0; n < 10000; n++) {
            sglib_node_t_delete(&tree, node);
            sglib_node_t_add(&tree, node);
            sglib_node_t_delete(&tree, node);
            sglib_node_t_add(&tree, node);
        }
        end = clock();
        cpu_time_used = (double) (end - start);
        printf("%d %f\n", i, cpu_time_used);
    }
    printf("\n\n");
    return 0;
}
