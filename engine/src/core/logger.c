#include "logger.h"
#include <platform/platform.h>

//TODO temporary
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "asserts.h"

void report_assertion_failure(const char* expression, const char* message, const char* file, u32 line) {
    AG_FATAL("Assertion Failure: '%s', message: '%s', in file '%s', in line '%d'\n", expression, message, file, line);
}

b8 initialize_logging() {
    //TODO: create log file
    return TRUE;
}

void shutdown_logging() {
    //TODO: cleanup logging/write queued entries
}

void log_output(log_level level, const char* message, ...) {
    const char* level_strings[6] = {"[FATAL]: ", "[ERROR]: ", "[WARN]: ", "[INFO]: ", "[DEBUG]: ", "[TRACE]: "};
    b8 is_error = level < LOG_LEVEL_WARN;

    //TODO: Change That!  
    const i32 message_length = 32000;
    char out_message[message_length];
    memset(out_message, 0, sizeof(out_message)); //Zeros All The Memory 

    //A pointer to The Args
    __builtin_va_list arg_ptr;
    va_start(arg_ptr, message);
    vsnprintf(out_message, sizeof(out_message), message, arg_ptr);
    va_end(arg_ptr);

    //Add The Level To out message
    char final_message[message_length];
    sprintf(final_message, "%s%s\n", level_strings[level], out_message);

    // Platform-specific output
    if (is_error) {
        platform_console_write_error(final_message, level);
    }else {
            platform_console_write(final_message, level);
    }
}

