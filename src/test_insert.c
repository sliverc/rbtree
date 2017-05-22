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

//               .---.   rotate_right   .---.
//               | y |     ------->     | x |
//               .---.                  .---.
//              /     ∖                /     ∖
//         .---'     .-'-.        .---'      .'--.
//         | x |     | C |        | A |      | y |
//         .---.     '---'        '---'      .---.
//        /     ∖                           /     ∖
//     .-'-.    .'--.                    .-'-.    .'--.
//     | A |    | B |      <------       | B |    | C |
//     '---'    '---'    rotate_left     '---'    '---'

#define Ny 0
#define Nx 1
#define NA 2
#define NB 3
#define NC 4
#define Nr 5
#define node_m(x) (&n[x])

int
assert_right(node_t n[5]) {
    TA(
        rb_left_m(node_m(Nx))   == node_m(NA),
        "Not rotated right"
    );
    TA(
        rb_right_m(node_m(Nx))  == node_m(Ny),
        "Not rotated right"
    );
    TA(
        rb_left_m(node_m(Ny))   == node_m(NB),
        "Not rotated right"
    );
    TA(
        rb_right_m(node_m(Ny))  == node_m(NC),
        "Not rotated right"
    );
    TA(
        rb_parent_m(node_m(NA)) == node_m(Nx),
        "Not rotated right"
    );
    TA(
        rb_parent_m(node_m(Ny)) == node_m(Nx),
        "Not rotated right"
    );
    TA(
        rb_parent_m(node_m(NB)) == node_m(Ny),
        "Not rotated right"
    );
    TA(
        rb_parent_m(node_m(NC)) == node_m(Ny),
        "Not rotated right"
    );
    return 0;
}


int
assert_left(node_t n[6]) {
    TA(
        rb_left_m(node_m(Ny))   == node_m(Nx),
        "Not rotated left"
    );
    TA(
        rb_right_m(node_m(Ny))  == node_m(NC),
        "Not rotated left"
    );
    TA(
        rb_left_m(node_m(Nx))   == node_m(NA),
        "Not rotated left"
    );
    TA(
        rb_right_m(node_m(Nx))   == node_m(NB),
        "Not rotated left"
    );
    TA(
        rb_parent_m(node_m(Nx)) == node_m(Ny),
        "Not rotated left"
    );
    TA(
        rb_parent_m(node_m(NC)) == node_m(Ny),
        "Not rotated left"
    );
    TA(
        rb_parent_m(node_m(NA)) == node_m(Nx),
        "Not rotated left"
    );
    TA(
        rb_parent_m(node_m(NB)) == node_m(Nx),
        "Not rotated left"
    );
    return 0;
}

int
test_rotate(void)
{
    node_t n[6];
    node_t* tree = node_m(Nx);
    node_t* node = node_m(Nx);
    for(int i = 0; i < 6; i++) {
        rb_value_m(node_m(i)) = i;
        rb_node_init_m(my, node_m(i));
    }
    /* Setup nodes */
    rb_left_m(node_m(Nx))   = node_m(NA);
    rb_right_m(node_m(Nx))  = node_m(Ny);

    rb_left_m(node_m(Ny))   = node_m(NB);
    rb_right_m(node_m(Ny))  = node_m(NC);

    rb_parent_m(node_m(NA)) = node_m(Nx);
    rb_parent_m(node_m(NB)) = node_m(Ny);
    rb_parent_m(node_m(NC)) = node_m(Ny);
    rb_parent_m(node_m(Ny)) = node_m(Nx);
    T(assert_right(n));
    TA(
        rb_parent_m(node_m(Nx)) == NULL,
        "Not rotated right"
    );
    _rb_rotate_left_m(my, tree, node);
    TA(
        tree == node_m(Ny),
        "Y should be root"
    );
    T(assert_left(n));
    TA(
        rb_parent_m(node_m(Ny))  == NULL,
        "Not rotated left"
    );
    tree = node_m(Ny);
    node = node_m(Ny);
    _rb_rotate_right_m(my, tree, node);
    TA(
        tree == node_m(Nx),
        "X should be root"
    );
    T(assert_right(n));
    TA(
        rb_parent_m(node_m(Nx)) == NULL,
        "Not rotated right"
    );
    /* Test with extra root */
    rb_parent_m(node_m(Nx)) = node_m(Nr);
    rb_left_m(node_m(Nr))   = node_m(Nx);
    T(assert_right(n));
    tree = node_m(Nr);
    node = node_m(Nx);
    TA(
        tree == node_m(Nr),
        "r should be root"
    );
    _rb_rotate_left_m(my, tree, node);
    TA(
        tree == node_m(Nr),
        "r should be root"
    );
    T(assert_left(n));
    node = node_m(Ny);
    _rb_rotate_right_m(my, tree, node);
    TA(
        tree == node_m(Nr),
        "r should be root"
    );
    T(assert_right(n));
    return 0;
}

int main(void)
{
    return test_insert_static();
}
