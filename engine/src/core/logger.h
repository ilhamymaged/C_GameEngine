#pragma once

#include <defines.h>

#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1

#if AGINA_RELEASE == 1
#define LOG_DEBUG_ENABLED 0
#define LOG_TRACE_ENABLED 0
#endif

typedef enum log_level
{
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR = 1,
    LOG_LEVEL_WARN = 2,
    LOG_LEVEL_INFO = 3,
    LOG_LEVEL_DEBUG = 4, 
    LOG_LEVEL_TRACE = 5  
} log_level;

b8 initialize_logging();
void shutdown_logging();

AGINA_API void log_output(log_level level, const char* message, ...);

//Always There
#define AG_FATAL(message, ...) log_output(LOG_LEVEL_FATAL, message, ##__VA_ARGS__);
#define AG_ERROR(message, ...) log_output(LOG_LEVEL_ERROR, message, ##__VA_ARGS__);

#if LOG_WARN_ENABLED == 1
#define AG_WARN(message, ...) log_output(LOG_LEVEL_WARN, message, ##__VA_ARGS__);
#else
#define AG_WARN(message, ...);
#endif

#if LOG_INFO_ENABLED == 1
#define AG_INFO(message, ...) log_output(LOG_LEVEL_INFO, message, ##__VA_ARGS__);
#else
#define AG_INFO(message, ...);
#endif

#if LOG_DEBUG_ENABLED == 1
#define AG_DEBUG(message, ...) log_output(LOG_LEVEL_DEBUG, message, ##__VA_ARGS__);
#else
#define AG_DEBUG(message, ...);
#endif

#if LOG_TRACE_ENABLED == 1
#define AG_TRACE(message, ...) log_output(LOG_LEVEL_TRACE, message, ##__VA_ARGS__);
#else 
#define AG_TRACE(message, ...);
#endif
