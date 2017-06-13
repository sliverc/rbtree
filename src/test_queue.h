struct item_s;
typedef struct item_s item_t;
struct item_s {
    int     value;
    item_t* next;
};

int
test_enqueue(item_t* item);
int
test_dequeue(int item);
void
test_queue_init(void);
int
test_queue_null(void);
int
test_queue_iter(int* values);
