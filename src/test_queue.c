#include "testing.h"

item_t* queue = NULL;

void
test_queue_init(void)
{
    queue = NULL;
}

int
test_enqueue(item_t* item)
{
    qq_enqueue(&queue, item);
    return 0;
}

int
test_dequeue(int item)
{
    item_t *tmp = NULL;
    qq_dequeue(&queue, &tmp);
    TA(item == rb_value_m(tmp), "Wrong item dequeued");
    return 0;
}

int
test_queue_null(void)
{
    return queue != NULL;
}

int
test_queue_iter(int* values)
{
    (void)(values);
    qs_queue_iter_decl_cx_m(qq, iter, elem);
    int i = 0;
    rb_for_m(qq, queue, iter, elem) {
        if(rb_value_m(elem) != values[i])
            return 1;
        i += 1;
    }
    return 0;
}
