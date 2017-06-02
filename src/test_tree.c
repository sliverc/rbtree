#include "testing.h"

node_t* tree;

void
test_init(void)
{
    tree = NULL;
}

void
test_add(node_t* node)
{
    my_node_init(node);
    my_insert(&tree, node);
    my_check_tree(tree);
}

void
test_remove(node_t* node)
{
    my_delete_node(&tree, node);
    my_check_tree(tree);
}
