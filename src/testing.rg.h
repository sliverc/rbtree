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

#begindef T(test)
{
    int __testing_tmp_ret_ = test;
    if(__testing_tmp_ret_ != 0)
        return __testing_tmp_ret_;
}
#enddef
