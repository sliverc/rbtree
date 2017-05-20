#include "rbtree.h"

struct node_s;
typedef struct node_s node_t;
struct node_s {
    node_t* left;
    node_t* right;
    node_t* parnet;
};

int test_traits(void)
{
    return 0;
}
