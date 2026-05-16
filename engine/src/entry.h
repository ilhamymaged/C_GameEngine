#pragma once

#include "core/application.h"
#include "core/logger.h"
#include "game_types.h"

extern b8 create_game(game* out_game); 

int main(void) {

    game 🎮;
    if (!create_game(&🎮)) {
        AG_FATAL("Couldn't Create Game");
        return -1;
    }

    if (!🎮.init || !🎮.render || !🎮.update || !🎮.on_resize || !🎮.shutdown) {
        AG_FATAL("The game's function pointers must be assigned");
        return -2;
    }


    if (!application_create(&🎮)) {
        AG_INFO("Application Failed To Create!\n");
        return 1;
    }

    if (!application_run()) {
        AG_INFO("Application Failed To Run!\n");
        return 2;
    }

    if (!application_shutDown()) {
        AG_INFO("Application Failed To ShutDown!\n");
        return 3;
    }
   
    return 0;
}