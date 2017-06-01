#include "testing.h"

#include <stdlib.h>

int
test_delete(int len, int* nodes, int* sorted, int count, int sum, int do_sum)
{
    (void)(sorted);
    int ret = 0;
    int i = 0;
    node_t* mnodes = malloc(len * sizeof(node_t));
    do {
        node_t* tree = NULL;
        node_t* node = NULL;
        for(i = 0; i < len; i++) {
            node = &mnodes[i];
            my_node_init(node);
            rb_value_m(node) = nodes[i];
            //printf("%d ", nodes[i]);
            my_insert(&tree, node);
            my_check_tree(tree);
        }
        //printf("\n");
        BA((
                rb_parent_m(node) != NULL ||
                rb_left_m(node) != NULL ||
                rb_right_m(node) != NULL ||
                rb_color_m(node) != 0
        ), "Node is not in a tree");
        node = &mnodes[len - 1];
        //print_tree(0, tree, NULL);
        my_delete_node(&tree, node);
        int tsum = 0;
        int elems = 0;
        my_check_tree(tree);
        recursive_sum(&tsum, &elems, tree);
        BA(elems == count, "Iterator count failed");
        //printf("\n%d == %d, %d == %d\n", count, elems, sum, tsum);
        if(do_sum)
            BA(tsum == sum, "Iterator sum failed");
        rb_iter_decl_cx_m(my, iter, elem);
        i = 0;
        tsum = 0;
        int fail = 0;
        rb_for_cx_m(my, tree, iter, elem) {
            if(sorted[i] != rb_value_m(elem))
                fail = 1;
            tsum += rb_value_m(elem);
            BA(i < count, "Iterator count failed");
            i += 1;
        }
        BA(fail == 0, "Not correctly sorted");
        BA(i == count, "Iterator count failed");
        if(do_sum)
            BA(tsum == sum, "Iterator sum failed");
    } while(0);
    free(mnodes);
    return ret;
}

int
test_switch(int len, int* nodes, int sum, int do_sum)
{
    int ret = 0;
    node_t* mnodes = malloc(len * sizeof(node_t));
    int* expect_in_order = malloc(len * sizeof(int));
    int* actual_in_order = malloc(len * sizeof(int));
    do {
        int value;
        int i;
        node_t* tree = NULL;
        node_t* node = NULL;
        node_t* x;
        node_t* y;

        for(int i = 0; i < len; i++) {
            node = &mnodes[i];
            my_node_init(node);
            rb_value_m(node) = nodes[i];
            my_insert(&tree, node);
            my_check_tree(tree);
        }
        BA((
                rb_parent_m(node) != NULL ||
                rb_left_m(node) != NULL ||
                rb_right_m(node) != NULL ||
                rb_color_m(node) != 0
        ), "Node is not in a tree");
        x = &mnodes[len - 1];
        y = &mnodes[len - 2];
        rb_iter_decl_cx_m(my, iter, elem);
        i = 0;
        rb_for_cx_m(my, tree, iter, elem) {
            expect_in_order[i] = rb_value_m(elem);
            i += 1;
        }
        value = rb_value_m(x);
        rb_value_m(x) = rb_value_m(y);
        rb_value_m(y) = value;
        {
            _rb_switch_node_m(
                node_t,
                NULL,
                rb_parent_m,
                rb_left_m,
                rb_right_m,
                tree,
                x,
                y
            );
        }
        i = 0;
        rb_for_cx_m(my, tree, iter, elem) {
            actual_in_order[i] = rb_value_m(elem);
            i += 1;
        }
        BA(i == len, "Iterator count failed");
        BA(
            memcmp(expect_in_order, actual_in_order, len * sizeof(int)) == 0,
            "Not in correct order"
        );
        int tsum = 0;
        int elems = 0;
        recursive_sum(&tsum, &elems, tree);
        BA(elems == len, "Iterator count failed");
        if(do_sum)
            BA(tsum == sum, "Iterator sum failed");
    } while(0);
    free(actual_in_order);
    free(expect_in_order);
    free(mnodes);
    return ret;
}

int
main(void)
{
    int nodes[5] = {0, 1, 2, -2, -1};
    int sorted[4] = {-2, 0, 1, 2};
    return test_delete(5, nodes, sorted, 4, 1, 1);

}
