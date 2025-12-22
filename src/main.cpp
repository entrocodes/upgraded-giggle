#define SDL_MAIN_HANDLED  // <--- Add this at the absolute top
#include <SDL.h>
#include "game/GameEngine.hpp"

int main(int argc, char* argv[]) {
    // Before calling any SDL functions, let SDL know we are handling the entry point
    SDL_SetMainReady();
    GameEngine engine;
    engine.run();

    return 0;
}