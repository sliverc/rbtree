#include "testing.h"

node_t* tree;

void
test_init(void)
{
    my_tree_init(&tree);
}

int
test_add(node_t* node)
{
    int ret;
    my_node_init(node);
    ret = my_insert(&tree, node);
    my_check_tree(tree);
    if(ret != 0) {
        assert(!(
                rb_parent_m(node) != my_nil_ptr ||
                rb_left_m(node) != my_nil_ptr ||
                rb_right_m(node) != my_nil_ptr ||
                rb_color_m(node) != RB_BLACK
        ) && "Node should be black");
    }
    return ret;
}

int
test_remove(node_t* key)
{
    int ret = my_delete(&tree, key);
    my_check_tree(tree);
    return ret;
}

void
test_remove_node(node_t* node)
{
    my_delete_node(&tree, node);
    my_check_tree(tree);
}

int
test_find(node_t* node)
{
    node_t* out;
    int ret;
    ret = my_find(tree, node, &out);
    if(ret == 0) {
        TA(rb_value_m(node) == rb_value_m(out), "Did not find right node")
    }
    return ret;
}

int
test_replace_node(node_t* old, node_t* new)
{
    int ret = my_replace_node(&tree, old, new);
    my_check_tree(tree);
    return ret;
}

int
test_replace(node_t* old, node_t* new)
{
    int ret = my_replace(&tree, old, new);
    my_check_tree(tree);
    return ret;
}

int
test_size(void)
{
    return my_size(tree);
}

int
test_tree_nil(void)
{
    return tree == my_nil_ptr;
}
