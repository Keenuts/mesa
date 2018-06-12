#ifndef H_VIRGL_MACRO
#define H_VIRGL_MACRO

#include "util/macros.h"

#ifdef DEBUG
#define TRACE_IN() fprintf(stderr, "--> %s\n", __func__)
#define TRACE_OUT(...) \
        do {    \
            char *fmt = "<-- %s (%d)\n"; \
            fprintf(stderr, fmt, __func__ __VA_OPT__(,) __VA_ARGS__ , 0); \
        } while (0)
#else
#define TRACE_IN()
#define TRACE_OUT(...)
#endif

#define UNUSED_PARAMETER(Param) (void)(Param)
#define RETURN(...)         \
    TRACE_OUT(__VA_ARGS__); \
    return __VA_ARGS__

#endif
