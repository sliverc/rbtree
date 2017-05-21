#include "testing.h"

rb_new_context_m(my, node_t)
#define my_cmp_m(x, y) rb_value_cmp_m(x, y)

int
test_insert_static(void)
{
    node_t mnodes[5];
    node_t* tree = NULL;
    node_t* node;
    for(int i = 0; i < 5; i++) {
        node = &mnodes[i];
        rb_value_m(node) = i;
        rb_node_init_m(my, node);
        TA(rb_color_m(node) == RB_WHITE, "Node not white after init");
    }
    node = &mnodes[0];
    rb_insert_m(my, tree, node);
    TA(rb_is_root_m(rb_color_m(node)), "Node not root after first insert");
    TA(rb_is_black_m(rb_color_m(node)), "Node not black after first insert");

    node = &mnodes[1];
    rb_value_m(node) = 0;
    rb_insert_m(my, tree, node);
    TA(
        rb_is_white_m(rb_color_m(node)),
        "Equal node should not have been inserted"
    );
    TA(tree == &mnodes[0], "Equal node should not have been inserted");

    rb_value_m(node) = 1;
    for(int i = 0; i < 2; i++) {
        rb_node_init_m(my, tree);
        rb_node_init_m(my, node);
        tree = NULL;
        rb_insert_m(my, tree, &mnodes[0]);
        rb_insert_m(my, tree, node);
        TA(
            rb_is_red_m(rb_color_m(node)),
            "The first insert node should be red"
        );
        TA(
            rb_right_m(tree) == node,
            "The node should have been inserted on the right"
        );
    }

    rb_node_init_m(my, tree);
    rb_node_init_m(my, node);
    tree = NULL;
    rb_insert_m(my, tree, &mnodes[0]);
    rb_value_m(node) = -1;
    rb_insert_m(my, tree, node);
    TA(
        rb_is_red_m(rb_color_m(node)),
        "The first insert node should be red"
    );
    TA(
        rb_left_m(tree) == node,
        "The node should have been inserted on the left"
    );

    rb_node_init_m(my, tree);
    rb_node_init_m(my, node);
    rb_value_m(node) = 1;
    tree = NULL;
    rb_insert_m(my, tree, &mnodes[1]);
    rb_insert_m(my, tree, &mnodes[0]);
    rb_insert_m(my, tree, &mnodes[2]);
    TA(
        rb_left_m(tree) == &mnodes[0],
        "The smaller node should be in the left"
    );
    TA(
        rb_right_m(tree) == &mnodes[2],
        "The bigger node should be in the left"
    );
    return 0;
}
