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
