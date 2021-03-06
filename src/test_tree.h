struct node_s;
typedef struct node_s node_t;
struct node_s {
    int     value;
    char    color;
    node_t* parent;
    node_t* left;
    node_t* right;
};
void
test_init(void);
int
test_add(node_t* node);
void
test_remove_node(node_t* node);
int
test_remove(node_t* node);
int
test_replace_node(node_t* old, node_t* new);
int
test_replace(node_t* old, node_t* new);
int
test_find(node_t* node);
int
test_size(void);
int
test_tree_nil(void);
