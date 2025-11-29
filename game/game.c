#include "game.h"
#include "../engine/input/input.h"
#include "../engine/renderer/mesh.h"
#include "../engine/renderer/camera.h"
#include "../engine/resource/obj_loader.h"
#include <stdio.h>
#include <stdlib.h>

/* Shader sources */
static const char* vertex_shader_source = 
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aNormal;\n"
    "layout (location = 2) in vec2 aTexCoord;\n"
    "out vec3 FragPos;\n"
    "out vec3 Normal;\n"
    "out vec2 TexCoord;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main() {\n"
    "    FragPos = vec3(model * vec4(aPos, 1.0));\n"
    "    Normal = mat3(transpose(inverse(model))) * aNormal;\n"
    "    TexCoord = aTexCoord;\n"
    "    gl_Position = projection * view * vec4(FragPos, 1.0);\n"
    "}\n";

static const char* fragment_shader_source = 
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 FragPos;\n"
    "in vec3 Normal;\n"
    "in vec2 TexCoord;\n"
    "uniform vec3 lightDir;\n"
    "uniform vec3 lightColor;\n"
    "uniform vec4 objectColor;\n"
    "uniform vec3 viewPos;\n"
    "void main() {\n"
    "    // Ambient\n"
    "    float ambientStrength = 0.3;\n"
    "    vec3 ambient = ambientStrength * lightColor;\n"
    "    // Diffuse\n"
    "    vec3 norm = normalize(Normal);\n"
    "    float diff = max(dot(norm, -lightDir), 0.0);\n"
    "    vec3 diffuse = diff * lightColor;\n"
    "    // Specular\n"
    "    float specularStrength = 0.5;\n"
    "    vec3 viewDir = normalize(viewPos - FragPos);\n"
    "    vec3 reflectDir = reflect(lightDir, norm);\n"
    "    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);\n"
    "    vec3 specular = specularStrength * spec * lightColor;\n"
    "    // Result\n"
    "    vec3 result = (ambient + diffuse + specular) * objectColor.rgb;\n"
    "    FragColor = vec4(result, objectColor.a);\n"
    "}\n";

Game* game_create(void) {
    Game* game = (Game*)malloc(sizeof(Game));
    if (!game) return NULL;
    
    /* Initialize to NULL */
    game->engine = NULL;
    game->player = NULL;
    game->enemies = NULL;
    game->terrain = NULL;
    game->shader = NULL;
    game->player_mesh = NULL;
    game->enemy_mesh = NULL;
    game->game_over = false;
    game->paused = false;
    
    /* Create engine */
    EngineConfig config = engine_default_config();
    config.window_title = "3D Game - WASD to move, Mouse to look, Space to jump";
    game->engine = engine_create(&config);
    if (!game->engine) {
        fprintf(stderr, "Failed to create engine\n");
        free(game);
        return NULL;
    }
    
    /* Create shader */
    game->shader = shader_create(vertex_shader_source, fragment_shader_source);
    if (!game->shader) {
        fprintf(stderr, "Failed to create shader\n");
        engine_destroy(game->engine);
        free(game);
        return NULL;
    }
    
    /* Create terrain */
    game->terrain = terrain_create_procedural(64, 64, 2.0f, 10.0f, 2.0f);
    if (!game->terrain) {
        fprintf(stderr, "Failed to create terrain\n");
        shader_destroy(game->shader);
        engine_destroy(game->engine);
        free(game);
        return NULL;
    }
    
    /* Load or create player mesh */
    game->player_mesh = obj_loader_load("assets/models/player.obj");
    if (!game->player_mesh) {
        /* Create a simple cube as fallback */
        game->player_mesh = mesh_create_cube(1.0f);
    }
    
    /* Create player at center of terrain */
    f32 start_y = terrain_get_height_at(game->terrain, 0, 0) + 1.0f;
    Vec3 player_start = vec3_create(0, start_y, 0);
    game->player = player_create(player_start, game->player_mesh);
    if (!game->player) {
        fprintf(stderr, "Failed to create player\n");
        mesh_destroy(game->player_mesh);
        terrain_destroy(game->terrain);
        shader_destroy(game->shader);
        engine_destroy(game->engine);
        free(game);
        return NULL;
    }
    
    /* Update camera aspect ratio */
    i32 width, height;
    engine_get_window_size(game->engine, &width, &height);
    camera_set_aspect_ratio(game->player->camera, (f32)width / (f32)height);
    
    /* Capture mouse for camera control */
    engine_set_mouse_captured(game->engine, true);
    
    /* Load or create enemy mesh */
    game->enemy_mesh = obj_loader_load("assets/models/enemy.obj");
    if (!game->enemy_mesh) {
        /* Create a simple sphere as fallback */
        game->enemy_mesh = mesh_create_sphere(0.5f, 16, 16);
    }
    
    /* Create enemy manager and spawn enemies */
    game->enemies = enemy_manager_create(16);
    if (!game->enemies) {
        fprintf(stderr, "Failed to create enemy manager\n");
        player_destroy(game->player);
        mesh_destroy(game->player_mesh);
        mesh_destroy(game->enemy_mesh);
        terrain_destroy(game->terrain);
        shader_destroy(game->shader);
        engine_destroy(game->engine);
        free(game);
        return NULL;
    }
    
    enemy_manager_set_mesh(game->enemies, game->enemy_mesh);
    
    /* Spawn some enemies around the terrain */
    f32 spawn_positions[][2] = {
        {20.0f, 20.0f},
        {-20.0f, 20.0f},
        {20.0f, -20.0f},
        {-20.0f, -20.0f},
        {30.0f, 0.0f},
        {-30.0f, 0.0f},
        {0.0f, 30.0f},
        {0.0f, -30.0f}
    };
    
    for (u32 i = 0; i < 8; i++) {
        f32 x = spawn_positions[i][0];
        f32 z = spawn_positions[i][1];
        f32 y = terrain_get_height_at(game->terrain, x, z) + 0.5f;
        enemy_manager_spawn(game->enemies, vec3_create(x, y, z), 50.0f);
    }
    
    printf("Game initialized successfully!\n");
    printf("Controls:\n");
    printf("  WASD - Move\n");
    printf("  Mouse - Look around\n");
    printf("  Space - Jump\n");
    printf("  ESC - Quit\n");
    
    return game;
}

void game_destroy(Game* game) {
    if (!game) return;
    
    if (game->enemies) enemy_manager_destroy(game->enemies);
    if (game->player) player_destroy(game->player);
    if (game->player_mesh) mesh_destroy(game->player_mesh);
    if (game->enemy_mesh) mesh_destroy(game->enemy_mesh);
    if (game->terrain) terrain_destroy(game->terrain);
    if (game->shader) shader_destroy(game->shader);
    if (game->engine) engine_destroy(game->engine);
    
    free(game);
}

void game_update(Game* game, f32 delta_time) {
    if (!game || game->game_over) return;
    
    InputState* input = engine_get_input(game->engine);
    
    /* Check for escape key */
    if (input_key_pressed(input, KEY_ESCAPE)) {
        /* Toggle mouse capture or quit */
        static bool mouse_captured = true;
        mouse_captured = !mouse_captured;
        engine_set_mouse_captured(game->engine, mouse_captured);
    }
    
    /* Update player */
    player_update(game->player, input, game->terrain, delta_time);
    
    /* Update camera aspect ratio if window was resized */
    i32 width, height;
    engine_get_window_size(game->engine, &width, &height);
    camera_set_aspect_ratio(game->player->camera, (f32)width / (f32)height);
    
    /* Update enemies */
    f64 current_time = engine_get_time(game->engine);
    enemy_manager_update(game->enemies, game->player->position, 
                         game->terrain, delta_time, (f32)current_time);
    
    /* Check for enemy attacks */
    f32 damage = enemy_manager_check_attacks(game->enemies, game->player->position, (f32)current_time);
    if (damage > 0.0f) {
        player_take_damage(game->player, damage);
        printf("Player took %.0f damage! Health: %.0f/%.0f\n", 
               damage, game->player->health, game->player->max_health);
    }
    
    /* Check game over */
    if (!player_is_alive(game->player)) {
        game->game_over = true;
        printf("Game Over! You died.\n");
    }
}

void game_render(Game* game) {
    if (!game) return;
    
    Camera* camera = player_get_camera(game->player);
    Mat4 view = camera_get_view_matrix(camera);
    Mat4 projection = camera_get_projection_matrix(camera);
    
    /* Use shader */
    shader_use(game->shader);
    shader_set_mat4(game->shader, "view", &view);
    shader_set_mat4(game->shader, "projection", &projection);
    
    /* Set lighting */
    Vec3 light_dir = vec3_normalize(vec3_create(-0.5f, -1.0f, -0.5f));
    shader_set_vec3(game->shader, "lightDir", light_dir);
    shader_set_vec3(game->shader, "lightColor", vec3_create(1.0f, 1.0f, 0.9f));
    shader_set_vec3(game->shader, "viewPos", camera->position);
    
    /* Draw terrain */
    Mat4 terrain_model = mat4_identity();
    shader_set_mat4(game->shader, "model", &terrain_model);
    shader_set_color(game->shader, "objectColor", color_create(0.3f, 0.6f, 0.2f, 1.0f));
    terrain_draw(game->terrain);
    
    /* Draw enemies */
    shader_set_color(game->shader, "objectColor", color_create(0.8f, 0.2f, 0.2f, 1.0f));
    for (u32 i = 0; i < game->enemies->count; i++) {
        Enemy* enemy = &game->enemies->enemies[i];
        if (!enemy_is_alive(enemy)) continue;
        
        Mat4 enemy_model = enemy_get_model_matrix(enemy);
        shader_set_mat4(game->shader, "model", &enemy_model);
        if (enemy->mesh) {
            mesh_draw(enemy->mesh);
        }
    }
    
    /* Note: Player model is not drawn in first-person view */
}

void game_run(Game* game) {
    if (!game) return;
    
    while (!engine_should_close(game->engine)) {
        engine_poll_events(game->engine);
        engine_begin_frame(game->engine);
        
        f32 delta_time = engine_get_delta_time(game->engine);
        
        game_update(game, delta_time);
        game_render(game);
        
        engine_end_frame(game->engine);
    }
}

bool game_is_running(Game* game) {
    return game && !game->game_over && !engine_should_close(game->engine);
}
