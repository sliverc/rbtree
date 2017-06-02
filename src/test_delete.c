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
        node_t* tree;
        my_tree_init(&tree);
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
                rb_parent_m(node) != my_nil_ptr ||
                rb_left_m(node) != my_nil_ptr ||
                rb_right_m(node) != my_nil_ptr ||
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
