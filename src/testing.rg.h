#ifndef rb_testing_h
#define rb_testing_h

#include "rbtree.h"
#include "qs.h"

#include <stdio.h>
#include <string.h>

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

#begindef BA(condition, ...)
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
        ret = 1;
        break;
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


#define my_cmp_m(x, y) rb_safe_value_cmp_m(x, y)
rb_bind_decl_m(my, node_t)

struct item_s;
typedef struct item_s item_t;
struct item_s {
    int     value;
    item_t* next;
};

qs_queue_bind_decl_m(qq, item_t)
qs_stack_bind_decl_m(qs, item_t)

static
void
print_tree(int n, node_t* l, node_t* r) {
    int pv = -91;
    int lv = -91;
    int rv = -91;
    char* pc = "";
    char* lc = "";
    char* rc = "";
    n += 1;
    node_t* p = my_nil_ptr;
    if(l != my_nil_ptr) {
        lc = rb_is_black_m(rb_color_m(l)) ? "black" : "red";
        lv = rb_value_m(l);
        p = rb_parent_m(l);
    }
    if(r != my_nil_ptr) {
        rc = rb_is_black_m(rb_color_m(r)) ? "black" : "red";
        rv = rb_value_m(r);
        p = rb_parent_m(r);
    }
    if(p != my_nil_ptr) {
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
    if(l != my_nil_ptr)
        print_tree(n, rb_left_m(l), rb_right_m(l));
    if(r != my_nil_ptr)
        print_tree(n, rb_left_m(r), rb_right_m(r));
}

static
void
recursive_sum(int* sum, int* elems, node_t* node)
{
    if(node == my_nil_ptr)
        return;
    *sum += rb_value_m(node);
    *elems += 1;
    recursive_sum(sum, elems, rb_left_m(node));
    recursive_sum(sum, elems, rb_right_m(node));
}
#endif // rb_testing_h
