#include "platform.h"

#if AGINA_PLATFORM_WINDOWS

#include <core/logger.h>

#include <windows.h>
#include <windowsx.h> 
#include <stdlib.h>

typedef struct internal_state {
    HINSTANCE  h_instance;
    HWND hwnd;
} internal_state;

// Clock
static f64 clock_frequency;
static large_integer start_time;

LRESULT CALLBACK win32_process_message(HWND hwnd, u32 message, WPARAM w_param, LPARAM l_param);

b8 platform_startup(platform_state* plat_state, const char* application_name, i32 x, i32 y, i32 width, i32 height) {
    
    plat_state->internal_state = malloc(sizeof(internal_state));
    internal_state* state = (internal_state*)plat_state->internal_state;

    //Give me a handle of this application that's executing this code.
    state->h_instance = GetModuleHandleA(0); 

    HICON icon = LoadIcon(state->h_instance, IDI_APPLICATION);
    WNDCLASSA wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = win32_process_message;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = state->h_instance;
    wc.hIcon = icon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL; //Transparent
    wc.lpszClassName = "agina_window_class";

    if (!RegisterClassA(&wc)) {
        MessageBoxA(0, "Window registration failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return FALSE;
    }

    //Create Window
    u32 client_x = x;
    u32 client_y = y;
    u32 client_width = width;
    u32 client_height = height;

    u32 window_x = client_x;
    u32 window_y = client_y;
    u32 window_width = client_width;
    u32 window_height = client_height;  

    u32 window_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
    u32 window_ex_style = WS_EX_APPWINDOW;

    window_style |= WS_MAXIMIZEBOX;
    window_style |= WS_MINIMIZEBOX;
    window_style |= WS_THICKFRAME;

    RECT border_rect = {0, 0, 0, 0};
    AdjustWindowRectEx(&border_rect, window_style, 0, window_ex_style);

    window_x += border_rect.left;
    window_y += border_rect.top;
    
    window_width += border_rect.right - border_rect.left;
    window_height += border_rect.bottom - border_rect.top;

    HWND handle = CreateWindowExA(   
        window_ex_style, "agina_window_class", application_name, 
        window_style, window_x, window_y, window_width, window_height, 
        0, 0, state->h_instance, 0);

    if (handle == 0) {
        MessageBoxA(NULL, "Window creation failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        AG_FATAL("window creation failed");
        return FALSE;
    } else {
        state->hwnd = handle;
    }

    b32 should_activate = 1; //TODO: if the window should't accept input, this should be false.
    i32 show_window_command_flags = should_activate ? SW_SHOW : SW_SHOWNOACTIVATE;
    ShowWindow(state->hwnd, show_window_command_flags);

    // Clock Setup
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    clock_frequency = 1.0 / (f64)frequency.QuadPart;
    QueryPerformanceCounter(&start_time);
    return TRUE;
}

void platform_shutdown(platform_state* plat_state) {
    internal_state* state = (internal_state*)plat_state->internal_state;

    if(state->hwnd) {
        DestroyWindow(state->hwnd);
        state->hwnd = 0;
    }
}

b8 platform_pump_messages(platform_state* plat_state) {
    MSG message;
    while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }

    return TRUE;
}

void* platform_allocate(u64 size, b8 aligned) {
    return malloc(size); // TODO: Memory Allocators
}

void  platform_free(void* block, b8 aligned) {
    free(block); // TODO: Memory Allocators
}

void* platform_zero_memory(void* block, u64 size) {
    return memset(block, 0, size); // TODO: Memory Allocators
}

void* platform_copy_memory(void* dest, const void* source, u64 size) {
    return memcpy(dest, source, size); // TODO: Memory Allocators
}

void* platform_set_memory(void* dest, i32 value, u64 size) {
    return memset(dest, value, size); // TODO: Memory Allocators
}

void platform_console_write(const char* message, u8 color) { 
    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    //FATAL, ERROR, WARN, INFO, DEBUG, TRACE
    static u8 levels[6] = {64, 4, 6, 2, 1, 8};
    SetConsoleTextAttribute(console_handle, levels[color]);

    OutputDebugStringA(message);
    u64 length = strlen(message);
    LPDWORD number_written = 0;
    WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), message, (DWORD)length, number_written, 0);
}

void platform_console_write_error(const char* message, u8 color) {
    HANDLE console_handle = GetStdHandle(STD_ERROR_HANDLE);
    //FATAL, ERROR, WARN, INFO, DEBUG, TRACE
    static u8 levels[6] = {64, 4, 6, 2, 1, 8};
    SetConsoleTextAttribute(console_handle, levels[color]);

    OutputDebugStringA(message);
    u64 length = strlen(message);
    LPDWORD number_written = 0;
    WriteConsoleA(GetStdHandle(STD_ERROR_HANDLE), message, (DWORD)length, number_written, 0);
}

f64 platform_get_absolute_time() {
     large_integer now;
     queryperformancecounter(&now);
     return (f64)now.quadpart * clock_frequency;
}

void platform_sleep(u64 ms) {
    Sleep(ms);
}

LRESULT CALLBACK win32_process_message(HWND hwnd, u32 message, WPARAM w_param, LPARAM l_param) {
    switch (message) {
        
        case WM_ERASEBKGND: 
            return 1; // Don't Worry About Drawing To thte screen
        
        case WM_CLOSE: // TODO: fire an Event To The Application To Quit 
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_SIZE: {
            // RECT r;
            // GetClientRect(hwnd, &r);
            // u32 width = r.right - r.left;
            // u32 height = r.bottom - r.top;

            // TODO: Fire A message for Window Resizing 
        } break;

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP: {
            // b8 pressed = (message == WM_KEYDOWN || message == WM_SYSKEYDOWN);
            // TODO: Input processing
        }   break;

        case WM_MOUSEMOVE: {
            // i32 x_position = GET_X_LPARAM(l_param);
            // i32 y_position = GET_Y_LPARAM(l_param);
            // TODO: Mouse Input processing
        }break;

        case WM_MOUSEWHEEL: {
            // i32 z_delta =  GET_WHEEL_DELTA_WPARAM(w_param);
            // if (z_delta != 0) {
            //     //Flatten The input to an os_independant range (-1, 1)
            //     z_delta = (z_delta < 0) ? -1 : 1;
            // }
            // TODO: Mouse Wheel Input processing
        }break;

        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP: {
            //b8 pressed = msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN;
            // TODO: input processing.
        } break;
    }

    //The Default Window Process in the usual
    return DefWindowProcA(hwnd, message, w_param, l_param);
}

#endif
