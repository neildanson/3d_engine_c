#include "game/game.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    
    printf("Starting 3D Game Engine...\n");
    
    Game* game = game_create();
    if (!game) {
        fprintf(stderr, "Failed to create game\n");
        return 1;
    }
    
    game_run(game);
    
    game_destroy(game);
    
    printf("Game ended.\n");
    return 0;
}
