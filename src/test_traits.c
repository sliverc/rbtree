#include "testing.h"

#define mx_color_m(x) (x)->color
#define mx_parent_m(x) (x)->parent
#define mx_left_m(x) (x)->left
#define mx_right_m(x) (x)->right
#define mx_cmp_m(x, y) rb_value_cmp_m(x, y)

rb_bind_cx_m(mx, node_t)

#define mz_cmp_m(x, y) rb_value_cmp_m(x, y)
rb_bind_m(mz, node_t)


static
int
test_set_traits(node_t* node)
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
assert_init_traits(node_t* node)
{
    TA(rb_left_m(node) == NULL, "Left not nil after init");
    TA(rb_right_m(node) == NULL, "Right not nil after init");
    TA(rb_parent_m(node) == NULL, "Parent not nil after init");
    return 0;
}

int
test_traits(void)
{
    node_t mnode;
    node_t* node = &mnode;

    T(test_set_traits(node));

    my_node_init(node);
    T(assert_init_traits(node));

    T(test_set_traits(node));

    rb_node_init_m(
        rb_color_m,
        rb_parent_m,
        rb_left_m,
        rb_right_m,
        node
    );
    T(assert_init_traits(node));
    return 0;
}
