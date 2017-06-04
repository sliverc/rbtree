#ifndef rb_example_h
#define rb_example_h

#include "rbtree.h"

struct book_s;
typedef struct book_s book_t;
struct book_s {
    char    isbn[14];
    char*   title;
    char*   author;
    char    color;
    book_t* parent;
    book_t* left;
    book_t* right;
};

#define bk_cmp_m(x, y) memcmp(x->isbn, y->isbn, 13)
rb_bind_decl_m(bk, book_t)

#endif
