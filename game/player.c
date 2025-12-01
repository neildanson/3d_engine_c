#include "player.h"
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Player* player_create(Vec3 start_position, Mesh* mesh) {
    Player* player = (Player*)malloc(sizeof(Player));
    if (!player) return NULL;
    
    player->position = start_position;
    player->velocity = vec3_create(0, 0, 0);
    player->yaw = -90.0f;
    player->pitch = 0.0f;
    player->move_speed = 8.0f;
    player->jump_force = 10.0f;
    player->gravity = 25.0f;
    player->health = 100.0f;
    player->max_health = 100.0f;
    player->on_ground = false;
    player->is_jumping = false;
    player->mesh = mesh;
    player->camera_height = 1.7f;
    player->player_radius = 0.5f;
    
    /* Create camera */
    Vec3 camera_pos = vec3_add(start_position, vec3_create(0, player->camera_height, 0));
    player->camera = camera_create(camera_pos, player->yaw, player->pitch);
    if (!player->camera) {
        free(player);
        return NULL;
    }
    
    player->camera->move_speed = player->move_speed;
    
    return player;
}

void player_destroy(Player* player) {
    if (!player) return;
    
    if (player->camera) {
        camera_destroy(player->camera);
    }
    /* Note: mesh is not destroyed here as it's owned by the resource manager */
    free(player);
}

void player_update(Player* player, const InputState* input, const Terrain* terrain, f32 delta_time) {
    if (!player || !input) return;
    
    /* Process mouse input for camera rotation */
    f64 dx, dy;
    input_get_mouse_delta(input, &dx, &dy);
    
    if (dx != 0.0 || dy != 0.0) {
        camera_process_mouse(player->camera, (f32)dx, (f32)(-dy), true);
        player->yaw = player->camera->yaw;
        player->pitch = player->camera->pitch;
    }
    
    /* Calculate movement direction based on camera */
    Vec3 flat_front = camera_get_flat_front(player->camera);
    Vec3 right = player->camera->right;
    
    /* Horizontal movement */
    Vec3 move_dir = vec3_create(0, 0, 0);
    
    if (input_key_down(input, KEY_W)) {
        move_dir = vec3_add(move_dir, flat_front);
    }
    if (input_key_down(input, KEY_S)) {
        move_dir = vec3_sub(move_dir, flat_front);
    }
    if (input_key_down(input, KEY_A)) {
        move_dir = vec3_sub(move_dir, right);
    }
    if (input_key_down(input, KEY_D)) {
        move_dir = vec3_add(move_dir, right);
    }
    
    /* Normalize movement direction if moving */
    if (vec3_length_squared(move_dir) > 0.0001f) {
        move_dir = vec3_normalize(move_dir);
    }
    
    /* Apply horizontal velocity */
    player->velocity.x = move_dir.x * player->move_speed;
    player->velocity.z = move_dir.z * player->move_speed;
    
    /* Jump */
    if (input_key_pressed(input, KEY_SPACE) && player->on_ground) {
        player_jump(player);
    }
    
    /* Apply gravity */
    if (!player->on_ground) {
        player->velocity.y -= player->gravity * delta_time;
    }
    
    /* Update position */
    player->position = vec3_add(player->position, vec3_scale(player->velocity, delta_time));
    
    /* Terrain collision */
    if (terrain) {
        f32 terrain_height = terrain_get_height_at(terrain, player->position.x, player->position.z);
        
        if (player->position.y < terrain_height) {
            player->position.y = terrain_height;
            player->velocity.y = 0.0f;
            player->on_ground = true;
            player->is_jumping = false;
        } else if (player->position.y > terrain_height + 0.1f) {
            player->on_ground = false;
        }
    } else {
        /* No terrain, use ground at y=0 */
        if (player->position.y < 0.0f) {
            player->position.y = 0.0f;
            player->velocity.y = 0.0f;
            player->on_ground = true;
            player->is_jumping = false;
        }
    }
    
    /* Update camera position */
    player->camera->position = vec3_add(player->position, vec3_create(0, player->camera_height, 0));
}

Mat4 player_get_model_matrix(const Player* player) {
    Mat4 translation = mat4_translate(player->position);
    Mat4 rotation = mat4_rotate_y(player->yaw * M_PI / 180.0f);
    return mat4_multiply(translation, rotation);
}

void player_jump(Player* player) {
    if (player->on_ground) {
        player->velocity.y = player->jump_force;
        player->on_ground = false;
        player->is_jumping = true;
    }
}

void player_take_damage(Player* player, f32 damage) {
    player->health -= damage;
    if (player->health < 0.0f) {
        player->health = 0.0f;
    }
}

bool player_is_alive(const Player* player) {
    return player->health > 0.0f;
}

Camera* player_get_camera(const Player* player) {
    return player->camera;
}
