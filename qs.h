// =============
// Queue / Stack
// =============
//
// Queue and stack with a rbtree-style interface. Both queue and stack use one
// next pointer and can be described as linked lists, except that the queue is
// actually an ring.
//
// Installation
// ============
//
// Copy qs.h into your source.
//
// Development
// ===========
//
// See `README.rst`_
//
// .. _`README.rst`: https://github.com/ganwell/rbtree
//
// API
// ===
//
// qs_queue_bind_decl_m(context, type) alias rb_bind_decl_cx_m
//    Bind the qs_queue function declarations for *type* to *context*. Usually
//    used in a header.
//
// qs_queue_bind_impl_m(context, type)
//    Bind the qs_queue function implementations for *type* to *context*.
//    Usually used in a c-file. This variant uses the standard rb_*_m traits.
//
// qs_queue_bind_impl_cx_m(context, type)
//    Bind the qs_queue function implementations for *type* to *context*.
//    Usually used in a c-file. This variant uses cx##_*_m traits, which means
//    you have to define them.
//
// Then the following functions will be available.
//
// cx##_enqueue(type** queue, type* item)
//    Enqueue an item to the queue.
//
// cx##_dequeue(type** queue, type** item)
//    Dequeue an item from the queue. Queue will be set to NULL when empty.
//
// cx##_iter_init(type* tree, cx##_iter_t* iter, type** elem)
//    Initializes *elem* to point to the first element in the queue. Use
//    qs_queue_iter_decl_m to declare *iter* and *elem*. If the queue is empty
//    *elem* will be a NULL-pointer.
//
// cx##_iter_next(cx##_iter_t* iter, type** elem)
//    Move *elem* to the next element in the queue. *elem* will point to
//    NULL at the end.
//
// You can use rb_for_m from rbtree.h with qs.
//
// Implementation
// ==============
//
// Includes
// --------
//
// .. code-block:: cpp
//
#include <assert.h>
//
// Traits
// ------
//
// .. code-block:: cpp
//
#define qs_next_m(x) (x)->next

// Queue
// -----
//
// Common arguments
//
// next
//    Get the next item of the queue
//
//
// .. code-block:: text
//
//    .---.        .---.
//    | 2 |<-next--| 1 |
//    '---'        '---'
//      |next        ^
//      v        next|
//    .---.        .---.
//    | 3 |--next->| 4 |<--queue--
//    '---'        '---'
//
// qs_enqueue_m
// ------------
//
// Bound: cx##_enqueue
//
// Enqueues an item to the queue.
//
// queue
//    Beginning of the queue
//
// item
//    Item to enqueue.
//
// .. code-block:: cpp
//
#define qs_enqueue_m( \
        next, \
        queue, \
        item \
) \
{ \
    assert(next(item) == NULL && "Item already in use"); \
    if(queue == NULL) { \
        next(item) = item; \
    } else { \
        next(item) = next(queue); \
        next(queue) = item; \
    } \
    queue = item; \
} \


// qs_dequeue_m
// ------------
//
// Bound: cx##_dequeue
//
// Dequeue an item from the queue. Returns the first item in the queue (FIFO).
//
// queue
//    Beginning of the queue
//
// item
//    Item dequeued.
//
// .. code-block:: cpp
//
#define qs_dequeue_m( \
        next, \
        queue, \
        item \
) \
{ \
    assert(item == NULL && "Item should be NULL"); \
    assert(queue != NULL && "Cannot dequeue from empty queue"); \
    item = next(queue); \
    if(next(queue) == queue) \
        queue = NULL; \
    else \
        next(queue) = next(item); \
    next(item) = NULL; \
} \


// qs_queue_bind_decl_m
// --------------------
//
// Alias: qs_queue_bind_decl_cx_m
//
// Bind queue functions to a context. This only generates declarations.
//
// cx
//    Name of the new context.
//
// type
//    The type of the items of the queue.
//
// .. code-block:: cpp
//
#define qs_queue_bind_decl_m(cx, type) \
    typedef type cx##_iter_t; \
    typedef type cx##_type_t; \
    void \
    cx##_enqueue( \
            type** queue, \
            type* item \
    ); \
    void \
    cx##_dequeue( \
            type** queue, \
            type** item \
    ); \
    void \
    cx##_iter_init( \
            type* queue, \
            cx##_iter_t** iter, \
            type** elem \
    ); \
    void \
    cx##_iter_next( \
            cx##_iter_t* iter, \
            type** elem \
    ); \


#define qs_queue_bind_decl_cx_m(cx, type) qs_queue_bind_decl_m(cx, type)

// qs_queue_bind_impl_m
// ---------------------
//
// Bind queue functions to a context. This only generates implementations.
//
// qs_queue_bind_impl_m uses qs_next_m. qs_queue_bind_impl_cx_m uses
// cx##_next_m.
//
// cx
//    Name of the new context.
//
// type
//    The type of the items of the queue.
//
// .. code-block:: cpp
//
#define _qs_queue_bind_impl_tr_m(cx, type, next) \
    void \
    cx##_enqueue( \
            type** queue, \
            type* item \
    ) qs_enqueue_m( \
            next, \
            *queue, \
            item \
    ) \
    void \
    cx##_dequeue( \
            type** queue, \
            type** item \
    ) qs_dequeue_m( \
            next, \
            *queue, \
            *item \
    ) \
    void \
    cx##_iter_init( \
            type* queue, \
            cx##_iter_t** iter, \
            type** elem \
    ) \
    { \
        (void)(iter); \
        qs_queue_iter_init_m( \
            next, \
            queue, \
            *iter, \
            *elem \
        ); \
    } \
    void \
    cx##_iter_next( \
            cx##_iter_t* iter, \
            type** elem \
    ) \
    { \
        (void)(iter); \
        qs_queue_iter_next_m( \
            next, \
            iter, \
            *elem \
        ) \
    } \


#define qs_queue_bind_impl_cx_m(cx, type) \
    _qs_queue_bind_impl_tr_m(cx, type, cx##_next_m) \


#define qs_queue_bind_impl_m(cx, type) \
    _qs_queue_bind_impl_tr_m(cx, type, qs_next_m) \


#define qs_queue_bind_cx_m(cx, type) \
    qs_queue_bind_decl_cx_m(cx, type) \
    qs_queue_bind_impl_cx_m(cx, type) \


#define qs_queue_bind_m(cx, type) \
    qs_queue_bind_decl_m(cx, type) \
    qs_queue_bind_impl_m(cx, type) \


// qs_queue_iter_decl_m
// ---------------------
//
// Also: qs_queue_iter_decl_cx_m
//
// Declare iterator variables.
//
// iter
//    The new iterator variable.
//
// elem
//    The pointer to the current element.
//
// .. code-block:: cpp
//
#define qs_queue_iter_decl_m(type, iter, elem) \
    type* iter = NULL; \
    type* elem = NULL; \


#define qs_queue_iter_decl_cx_m(cx, iter, elem) \
    cx##_type_t* iter = NULL; \
    cx##_type_t* elem = NULL; \


// qs_queue_iter_init_m
// ---------------------
//
// Bound: cx##_iter_init
//
// Initialize iterator. It will point to the first element.
//
// queue
//    The queue.
//
// iter
//    The iterator.
//
// elem
//    The pointer to the current element.
//
//
// .. code-block:: cpp
//
#define qs_queue_iter_init_m(next, queue, iter, elem) \
{ \
    iter = queue; \
    elem = next(queue); \
} \


// qs_queue_iter_next_m
// --------------------
//
// Bound: cx##_iter_next
//
// Initialize iterator. It will point to the first element. The element will be
// NULL, if the iteration is at the end.
//
// queue
//    The queue.
//
// elem
//    The pointer to the current element.
//
// .. code-block:: cpp
//
#define qs_queue_iter_next_m( \
        next, \
        queue, \
        elem \
) \
{ \
    if(elem == queue) \
        elem = NULL; \
    else \
        elem = next(elem); \
} \

