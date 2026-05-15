#include "platform.h"
#include "core/input.h"

#if AGINA_PLATFORM_WINDOWS

#include <core/logger.h>

#include <windows.h>
#include <windowsx.h> 
#include <stdlib.h>

typedef struct internal_state {
    HINSTANCE  h_instance;
    HWND hwnd;
    HDC hdc;
    HGLRC gl_context;
} internal_state;

// Clock
static f64 clock_frequency;
static LARGE_INTEGER start_time;

LRESULT CALLBACK win32_process_message(HWND hwnd, u32 message, WPARAM w_param, LPARAM l_param);

// WGL EXTENSIONS
typedef HGLRC(WINAPI* wgl_create_context_attribs_arb)(HDC, HGLRC, const int*);
static wgl_create_context_attribs_arb wglCreateContextAttribsARB = NULL;

typedef const char* (WINAPI* wgl_get_extensions_string_ext)();
static wgl_get_extensions_string_ext wglGetExtensionsStringEXT = NULL;

static b8 load_wgl_extensions(HDC hdc) {
    // Create a temporary context to load WGL extension functions
    PIXELFORMATDESCRIPTOR pfd = { 0 };
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;

    int pf = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pf, &pfd);

    HGLRC temp_ctx = wglCreateContext(hdc);
    if (!temp_ctx) {
        AG_FATAL("Failed to create temporary OpenGL context for WGL extension loading");
        return FALSE;
    }

    wglMakeCurrent(hdc, temp_ctx);

    wglCreateContextAttribsARB = (wgl_create_context_attribs_arb)wglGetProcAddress("wglCreateContextAttribsARB");
    wglGetExtensionsStringEXT = (wgl_get_extensions_string_ext)wglGetProcAddress("wglGetExtensionsStringEXT");

    if (!wglCreateContextAttribsARB) {
        AG_FATAL("Failed to load wglCreateContextAttribsARB");
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(temp_ctx);
        return FALSE;
    }

    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(temp_ctx);
    return TRUE;
}

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

    state->hdc = GetDC(state->hwnd);
    if (!load_wgl_extensions(state->hdc)) {
        AG_FATAL("Failed to load WGL extensions");
        return FALSE;
    }

    #define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
    #define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
    #define WGL_CONTEXT_PROFILE_MASK_ARB  0x9126
    #define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001

    int attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 6,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };

    state->gl_context = wglCreateContextAttribsARB(state->hdc, 0, attribs);
    if (!state->gl_context) {
        AG_FATAL("Failed to create modern OpenGL context");
        return FALSE;
    }

    wglMakeCurrent(state->hdc, state->gl_context);

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

void platform_swap_buffers(platform_state* plat_state) {
    internal_state* state = (internal_state*)plat_state->internal_state;
    SwapBuffers(state->hdc);
}

b8 platform_pump_messages(platform_state* plat_state) {
    MSG message;
    while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE)) {
        if (message.message == WM_QUIT) {
            return FALSE;
        }
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
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return (f64)now.QuadPart * clock_frequency;
}

void platform_sleep(u64 ms) {
    Sleep(ms);
}

LRESULT CALLBACK win32_process_message(HWND hwnd, u32 message, WPARAM w_param, LPARAM l_param) {
    switch (message) {
        
        case WM_ERASEBKGND: 
            return 1; // Don't Worry About Drawing To thte screen
        
        case WM_CLOSE: // TODO: fire an Event To The Application To Quit
            PostQuitMessage(0); 

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_SIZE: {
            RECT r;
            GetClientRect(hwnd, &r);
            u32 width = r.right - r.left;
            u32 height = r.bottom - r.top;

            input_process_resize(width, height);
        } break;

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP: {
            b8 pressed = (message == WM_KEYDOWN || message == WM_SYSKEYDOWN);
            keys key = (u16)w_param;
            input_process_key(key, pressed);
        }   break;

        case WM_MOUSEMOVE: {
            i32 x_position = GET_X_LPARAM(l_param);
            i32 y_position = GET_Y_LPARAM(l_param);

            input_process_mouse_move(x_position, y_position);
        }break;

        case WM_MOUSEWHEEL: {
            i32 z_delta =  GET_WHEEL_DELTA_WPARAM(w_param);
            if (z_delta != 0) {
                // Flatten the input to an OS-independent (-1, 1)
                z_delta = (z_delta < 0) ? -1 : 1;\
                input_process_mouse_wheel(z_delta);
            }
        }break;

        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP: {
            b8 pressed = message == WM_LBUTTONDOWN || message == WM_RBUTTONDOWN || message == WM_MBUTTONDOWN;
              buttons mouse_button = BUTTON_MAX_BUTTONS;
            switch (message) {
                case WM_LBUTTONDOWN:
                case WM_LBUTTONUP:
                    mouse_button = BUTTON_LEFT;
                    break;
                case WM_MBUTTONDOWN:
                case WM_MBUTTONUP:
                    mouse_button = BUTTON_MIDDLE;
                    break;
                case WM_RBUTTONDOWN:
                case WM_RBUTTONUP:
                    mouse_button = BUTTON_RIGHT;
                    break;
            }

            // Pass over to the input subsystem.
            if (mouse_button != BUTTON_MAX_BUTTONS) {
                input_process_button(mouse_button, pressed);
            }
        } break;
    }

    //The Default Window Process in the usual
    return DefWindowProcA(hwnd, message, w_param, l_param);
}

#endif
