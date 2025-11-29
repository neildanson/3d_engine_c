#ifndef GAME_H
#define GAME_H

#include "../engine/core/engine.h"
#include "../engine/core/types.h"
#include "../engine/renderer/shader.h"
#include "../engine/resource/terrain.h"
#include "player.h"
#include "enemy.h"

/* Game state */
typedef struct {
    Engine* engine;
    Player* player;
    EnemyManager* enemies;
    Terrain* terrain;
    Shader* shader;
    Mesh* player_mesh;
    Mesh* enemy_mesh;
    bool game_over;
    bool paused;
} Game;

/* Game lifecycle */
Game* game_create(void);
void game_destroy(Game* game);
void game_run(Game* game);

/* Game update and render */
void game_update(Game* game, f32 delta_time);
void game_render(Game* game);

/* Game state */
bool game_is_running(Game* game);

#endif /* GAME_H */
