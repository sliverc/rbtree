#include "rbtree.h"
#include "testing.h"

struct node_s;
typedef struct node_s node_t;
struct node_s {
    char    color;
    node_t* parent;
    node_t* left;
    node_t* right;
};

rb_new_context_m(my, node_t)
#define my_color_m(x) (x)->color
#define my_parent_m(x) (x)->parent
#define my_left_m(x) (x)->left
#define my_right_m(x) (x)->right

static
int
test_set_tratis(node_t* node)
{
    rb_left_m(node) = node;
    TA(rb_left_m(node) == node, "Left was not set");

    rb_right_m(node) = node;
    TA(rb_right_m(node) == node, "Right was not set");

    rb_parent_m(node) = node;
    TA(rb_parent_m(node) == node, "Parent was not set");
    return 0;
}

static
int
assert_init_tratis(node_t* node)
{
    TA(rb_left_m(node) == NULL, "Left not NULL after init");
    TA(rb_right_m(node) == NULL, "Right not NULL after init");
    TA(rb_parent_m(node) == NULL, "Parent not NULL after init");
    return 0;
}

int
test_traits(void)
{
    node_t mnode;
    node_t* node = &mnode;

    T(test_set_tratis(node));

    rb_node_init_m(rb, node);
    T(assert_init_tratis(node));

    T(test_set_tratis(node));

    rb_node_init_cx_m(rb, node);
    T(assert_init_tratis(node));

    T(test_set_tratis(node));

    rb_node_init_cx_m(my, node);
    T(assert_init_tratis(node));

    T(test_set_tratis(node));

    rb_node_init_tr_m(
        node_t,
        rb_color_m,
        rb_parent_m,
        rb_left_m,
        rb_right_m,
        node
    );
    T(assert_init_tratis(node));
    return 0;
}
