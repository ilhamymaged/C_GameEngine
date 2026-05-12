#pragma once

#include <defines.h>

//Disable Assertions By Commenting out the below line
#define AGINA_ASSERTIONS_DEFINED

#ifdef AGINA_ASSERTIONS_DEFINED

#if _MSC_VER
#include <intrin.h>
#define debugBreak() __debugbreak()
#else
#define debugBreak() __builtin_trap()
#endif

AGINA_API void report_assertion_failure(const char* expression, const char* message, const char* file, u32 line);

#define AG_ASSERT(expr)                                                 \
{                                                                       \
    if (expr) {                                                         \
    }else {                                                             \
        report_assertion_failure(#expr, "", __FILE__, __LINE__);        \
        debugBreak();                                                   \
    }                                                                   \
}

#define AG_ASSERT_MSG(expr, message)                                    \
{                                                                       \
    if (expr) {                                                         \
    }else {                                                             \
        report_assertion_failure(#expr, message, __FILE__, __LINE__);   \
        debugBreak();                                                   \
    }                                                                   \
}

#ifdef _DEBUG
#define AG_ASSERT_DEBUG(expr)                                           \
{                                                                       \
    if (expr) {                                                         \
    }else {                                                             \
        report_assertion_failure(#expr, "", __FILE__, __LINE__);        \
        debugBreak();                                                   \
    }                                                                   \
}
#else
#define AG_ASSERT_DEBUG(expr)
#endif 

#else
#define AG_ASSERT(expr)
#define AG_ASSERT_MSG(expr, message)
#define AG_ASSERT_DEBUG(expr);
#endif