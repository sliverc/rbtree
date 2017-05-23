#include "testing.h"

#include <stdlib.h>

#define MSIZE 1000000

node_t mnodes[MSIZE];

int
main(void)
{
    node_t* tree = NULL;
    node_t* node;
    for(int i = 0; i < MSIZE; i++) {
        node = &mnodes[i];
        my_node_init(node);
        rb_value_m(node) = rand();
    }
    for(int i = 0; i < MSIZE; i++)
        my_insert(&tree, &mnodes[i]);
    return 0;
}
