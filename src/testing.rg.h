#include <stdio.h>
#include <string.h>

#begindef RB_A(condition)
{
    if(!(condition)) {
        fprintf(
            stderr,
            "%s:%d %s\n",
            __FILE__,
            __LINE__,
            #condition
        );
        return 1;
    }
}
#enddef

#include "rbtree.h"

#begindef TA(condition, ...)
{
    if(!(condition)) {
        fprintf(
            stderr,
            "%s:%d ",
            __FILE__,
            __LINE__
        );
        fprintf(stderr, __VA_ARGS__);
        fprintf(stderr, "\n");
        return 1;
    }
}
#enddef

#begindef T(test)
{
    int __testing_tmp_ret_ = test;
    if(__testing_tmp_ret_ != 0) {
        return __testing_tmp_ret_;
    }
}
#enddef

struct node_s;
typedef struct node_s node_t;
struct node_s {
    int     value;
    char    color;
    node_t* parent;
    node_t* left;
    node_t* right;
};

static
void
print_tree(int n, node_t* l, node_t* r) {
    int pv = -1;
    int lv = -1;
    int rv = -1;
    char* pc = "";
    char* lc = "";
    char* rc = "";
    n += 1;
    node_t* p = NULL;
    if(l != NULL) {
        lc = rb_is_black_m(rb_color_m(l)) ? "black" : "red";
        lv = rb_value_m(l);
        p = rb_parent_m(l);
    }
    if(r != NULL) {
        rc = rb_is_black_m(rb_color_m(r)) ? "black" : "red";
        rv = rb_value_m(r);
        p = rb_parent_m(r);
    }
    if(p != NULL) {
        pc = rb_is_black_m(rb_color_m(p)) ? "black" : "red";
        pv = rb_value_m(p);
    }
    printf(
        "level: %3d parent: %d (%s) left: %d (%s) right: %d (%s)\n",
        n,
        pv,
        pc,
        lv,
        lc,
        rv,
        rc
    );
    if(l != NULL)
        print_tree(n, rb_left_m(l), rb_right_m(l));
    if(r != NULL)
        print_tree(n, rb_left_m(r), rb_right_m(r));
}
