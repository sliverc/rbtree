#include "testing.h"

#include <stdlib.h>

int
test_delete(int len, int* nodes, int count, int sum, int do_sum)
{
    node_t* mnodes = malloc(len * sizeof(node_t));
    node_t* tree = NULL;
    node_t* node;
    for(int i = 0; i < len; i++) {
        node = &mnodes[i];
        my_node_init(node);
        rb_value_m(node) = nodes[i];
        my_insert(&tree, node);
        my_check_tree(tree);
    }
    TA((
            rb_parent_m(node) != NULL ||
            rb_left_m(node) != NULL ||
            rb_right_m(node) != NULL ||
            rb_color_m(node) != 0
    ), "Node is not in a tree");
    node = &mnodes[len - 1];
    print_tree(0, tree, NULL);
    my_delete(&tree, node);
    TA((
        rb_parent_m(node) == NULL &&
        rb_left_m(node) == NULL &&
        rb_right_m(node) == NULL &&
        rb_color_m(node) == 0
    ), "Node not properly cleared");
    int tsum = 0;
    int elems = 0;
    recursive_sum(&tsum, &elems, tree);
    printf("\n%d == %d, %d == %d\n", count, elems, sum, tsum);
    TA(elems == count, "Iterator count failed");
    if(do_sum)
        TA(tsum == sum, "Iterator sum failed");
    int consistent = 0;
    free(mnodes);
    return consistent;
}

int
test_switch(int len, int* nodes, int sum, int do_sum)
{
    node_t* mnodes = malloc(len * sizeof(node_t));
    node_t* tree = NULL;
    node_t* node;
    node_t* x;
    node_t* y;

    for(int i = 0; i < len; i++) {
        node = &mnodes[i];
        my_node_init(node);
        rb_value_m(node) = nodes[i];
        my_insert(&tree, node);
        my_check_tree(tree);
    }
    TA((
            rb_parent_m(node) != NULL ||
            rb_left_m(node) != NULL ||
            rb_right_m(node) != NULL ||
            rb_color_m(node) != 0
    ), "Node is not in a tree");
    x = &mnodes[len - 1];
    y = &mnodes[len - 2];
    //print_tree(0, tree, NULL);
    _rb_switch_node_m(
        node_t,
        rb_parent_m,
        rb_left_m,
        rb_right_m,
        tree,
        x,
        y
    );
    int tsum = 0;
    int elems = 0;
    recursive_sum(&tsum, &elems, tree);
    /* print_tree(0, tree, NULL); *
     * printf("\n%d == %d, %d == %d\n", len, elems, sum, tsum); */
    TA(elems == len, "Iterator count failed");
    if(do_sum)
        TA(tsum == sum, "Iterator sum failed");
    int consistent = 0;
    free(mnodes);
    return consistent;
}

int main(void) {
    int i[2] = {0, 1};
    return test_delete(2, i, 1, 0, 1);
}
