#include "application.h"

#include "logger.h"

#include "platform/platform.h"
#include "core/AGmemory.h"

#include <game_types.h>

typedef struct application_state {
    game* game_instance;
    b8 is_running;
    b8 is_suspended;
    platform_state plat_state;
    i16 width;
    i16 height;
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

    // TODO: Remove This
    AG_INFO("Hello! From Agina");

    app_state.is_running = TRUE;
    app_state.is_suspended = FALSE;

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
    AG_INFO(get_memory_usage_str());

    while(app_state.is_running) {
        if(!platform_pump_messages(&app_state.plat_state)) {
            app_state.is_running = FALSE;
        }

        if (!app_state.is_suspended) {
            if (!app_state.game_instance->update(app_state.game_instance, (f32)0)) {
                AG_FATAL("Game Update Failed, Shutting Down!");
                app_state.is_running = FALSE;
                break;
            }

            if (!app_state.game_instance->render(app_state.game_instance, (f32)0)) {
                AG_FATAL("Game Render Failed, Shutting Down!");
                app_state.is_running = FALSE;
                break;
            }
        }
    }

    app_state.is_running = FALSE;

    // TODO: change that
    return TRUE;
}

b8 application_shutDown() {
    platform_shutdown(&app_state.plat_state);
    
    // TODO: change that
    return TRUE; 
}
