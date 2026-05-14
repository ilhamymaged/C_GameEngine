#include "application.h"

#include "logger.h"

#include "platform/platform.h"
#include "core/AGmemory.h"
#include "core/event.h"
#include "core/input.h"
#include "core/clock.h"

#include <game_types.h>

typedef struct application_state {
    game* game_instance;
    b8 is_running;
    b8 is_suspended;
    platform_state plat_state;
    i16 width;
    i16 height;
    clock clock;
    f64 last_time;
}application_state;

static b8 initialized = FALSE;
static application_state app_state;

b8 application_create(game* game_instance) {
    if (initialized) {
        AG_ERROR("application_create called more than once");
        return FALSE;
    }

    app_state.game_instance = game_instance;

    // Initialize subSystems
    initialize_logging();
    input_initialize();

    // TODO: Remove This
    AG_INFO("Hello! From Agina");

    app_state.is_running = TRUE;
    app_state.is_suspended = FALSE;

    if(!event_initialize()) {
        AG_ERROR("Event system failed initialization. Application cannot continue.");
        return FALSE;
    }

    if (!platform_startup(
        &app_state.plat_state,
        game_instance->app_config.name, 
        game_instance->app_config.start_pos_x, 
        game_instance->app_config.start_pos_y, 
        game_instance->app_config.start_width, 
        game_instance->app_config.start_height)) {
            return FALSE;
    }

    //Init The Game
    if (!app_state.game_instance->init(app_state.game_instance)) {
        AG_FATAL("Game Failed to initialize");
        return FALSE;
    }

    // TODO: Event System
    app_state.game_instance->on_resize(app_state.game_instance, app_state.width, app_state.height);

    initialized = TRUE;
    
    return TRUE;
}

b8 application_run() {
    clock_start(&app_state.clock);
    clock_update(&app_state.clock);
    app_state.last_time = app_state.clock.elapsed;
    f64 running_time = 0;
    u8 frame_count = 0;
    f64 target_frame_seconds = 1.0f / 60;

    AG_INFO(get_memory_usage_str());

    while(app_state.is_running) {
        if(!platform_pump_messages(&app_state.plat_state)) {
            app_state.is_running = FALSE;
        }

       if (!app_state.is_suspended) {
            clock_update(&app_state.clock);
            f64 current_time = app_state.clock.elapsed;
            f64 delta = (current_time - app_state.last_time);
            f64 frame_start_time = platform_get_absolute_time();

            if (!app_state.game_instance->update(app_state.game_instance, (f32)delta)) {
                AG_FATAL("Game update failed, shutting down.");
                app_state.is_running = FALSE;
                break;
            }

            // Call the game's render routine.
            if (!app_state.game_instance->render(app_state.game_instance, (f32)delta)) {
                AG_FATAL("Game render failed, shutting down.");
                app_state.is_running = FALSE;
                break;
            }

            // Figure out how long the frame took and, if below
            f64 frame_end_time = platform_get_absolute_time();
            f64 frame_elapsed_time = frame_end_time - frame_start_time;
            running_time += frame_elapsed_time;
            f64 remaining_seconds = target_frame_seconds - frame_elapsed_time;

            if (remaining_seconds > 0) {
                u64 remaining_ms = (remaining_seconds * 1000);

                // If there is time left, give it back to the OS.
                b8 limit_frames = FALSE;
                if (remaining_ms > 0 && limit_frames) {
                    platform_sleep(remaining_ms - 1);
                }

                frame_count++;
            }

            input_update(delta);
            app_state.last_time = current_time;
        }
    }
        app_state.is_running = FALSE;

    return TRUE;

}

b8 application_shutDown() {
    event_shutdown();
    input_shutdown();
    platform_shutdown(&app_state.plat_state);
    
    return TRUE; 
}
