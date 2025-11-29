#include "enemy.h"
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

EnemyManager* enemy_manager_create(u32 initial_capacity) {
    EnemyManager* manager = (EnemyManager*)malloc(sizeof(EnemyManager));
    if (!manager) return NULL;
    
    manager->enemies = (Enemy*)malloc(initial_capacity * sizeof(Enemy));
    if (!manager->enemies) {
        free(manager);
        return NULL;
    }
    
    manager->count = 0;
    manager->capacity = initial_capacity;
    manager->shared_mesh = NULL;
    
    return manager;
}

void enemy_manager_destroy(EnemyManager* manager) {
    if (!manager) return;
    
    free(manager->enemies);
    free(manager);
}

void enemy_manager_set_mesh(EnemyManager* manager, Mesh* mesh) {
    if (!manager) return;
    
    manager->shared_mesh = mesh;
    
    /* Update all existing enemies */
    for (u32 i = 0; i < manager->count; i++) {
        manager->enemies[i].mesh = mesh;
    }
}

Enemy* enemy_manager_spawn(EnemyManager* manager, Vec3 position, f32 health) {
    if (!manager) return NULL;
    
    /* Grow array if needed */
    if (manager->count >= manager->capacity) {
        u32 new_capacity = manager->capacity * 2;
        Enemy* new_enemies = (Enemy*)realloc(manager->enemies, new_capacity * sizeof(Enemy));
        if (!new_enemies) return NULL;
        
        manager->enemies = new_enemies;
        manager->capacity = new_capacity;
    }
    
    Enemy* enemy = &manager->enemies[manager->count++];
    
    enemy->position = position;
    enemy->velocity = vec3_create(0, 0, 0);
    enemy->yaw = 0.0f;
    enemy->move_speed = 4.0f;
    enemy->health = health;
    enemy->max_health = health;
    enemy->attack_damage = 10.0f;
    enemy->attack_range = 2.0f;
    enemy->detection_range = 15.0f;
    enemy->attack_cooldown = 1.0f;
    enemy->last_attack_time = 0.0f;
    enemy->state = ENEMY_STATE_IDLE;
    enemy->mesh = manager->shared_mesh;
    enemy->enemy_radius = 0.5f;
    
    return enemy;
}

static void enemy_update_single(Enemy* enemy, Vec3 player_position, 
                                const Terrain* terrain, f32 delta_time) {
    if (enemy->state == ENEMY_STATE_DEAD) return;
    
    /* Calculate distance to player */
    Vec3 to_player = vec3_sub(player_position, enemy->position);
    to_player.y = 0.0f; /* Ignore vertical distance for AI */
    f32 distance = vec3_length(to_player);
    
    /* Update state based on distance */
    if (distance < enemy->attack_range) {
        enemy->state = ENEMY_STATE_ATTACK;
    } else if (distance < enemy->detection_range) {
        enemy->state = ENEMY_STATE_CHASE;
    } else {
        enemy->state = ENEMY_STATE_IDLE;
    }
    
    /* Process state */
    switch (enemy->state) {
        case ENEMY_STATE_CHASE:
        case ENEMY_STATE_ATTACK: {
            /* Face player */
            if (distance > 0.001f) {
                Vec3 dir = vec3_normalize(to_player);
                enemy->yaw = atan2f(dir.x, dir.z) * 180.0f / M_PI;
                
                /* Move towards player if chasing */
                if (enemy->state == ENEMY_STATE_CHASE) {
                    enemy->velocity.x = dir.x * enemy->move_speed;
                    enemy->velocity.z = dir.z * enemy->move_speed;
                } else {
                    /* Stop when attacking */
                    enemy->velocity.x = 0.0f;
                    enemy->velocity.z = 0.0f;
                }
            }
            break;
        }
        case ENEMY_STATE_IDLE:
        case ENEMY_STATE_PATROL:
        default:
            enemy->velocity.x = 0.0f;
            enemy->velocity.z = 0.0f;
            break;
    }
    
    /* Apply gravity */
    enemy->velocity.y -= 25.0f * delta_time;
    
    /* Update position */
    enemy->position = vec3_add(enemy->position, vec3_scale(enemy->velocity, delta_time));
    
    /* Terrain collision */
    if (terrain) {
        f32 terrain_height = terrain_get_height_at(terrain, enemy->position.x, enemy->position.z);
        if (enemy->position.y < terrain_height) {
            enemy->position.y = terrain_height;
            enemy->velocity.y = 0.0f;
        }
    } else {
        if (enemy->position.y < 0.0f) {
            enemy->position.y = 0.0f;
            enemy->velocity.y = 0.0f;
        }
    }
}

void enemy_manager_update(EnemyManager* manager, Vec3 player_position, 
                          const Terrain* terrain, f32 delta_time, f32 current_time) {
    (void)current_time;
    
    if (!manager) return;
    
    for (u32 i = 0; i < manager->count; i++) {
        enemy_update_single(&manager->enemies[i], player_position, terrain, delta_time);
    }
}

f32 enemy_manager_check_attacks(EnemyManager* manager, Vec3 player_position, f32 current_time) {
    if (!manager) return 0.0f;
    
    f32 total_damage = 0.0f;
    
    for (u32 i = 0; i < manager->count; i++) {
        Enemy* enemy = &manager->enemies[i];
        
        if (enemy->state != ENEMY_STATE_ATTACK) continue;
        if (enemy->state == ENEMY_STATE_DEAD) continue;
        
        /* Check attack cooldown */
        if (current_time - enemy->last_attack_time < enemy->attack_cooldown) continue;
        
        /* Check distance */
        Vec3 to_player = vec3_sub(player_position, enemy->position);
        f32 distance = vec3_length(to_player);
        
        if (distance < enemy->attack_range) {
            total_damage += enemy->attack_damage;
            enemy->last_attack_time = current_time;
        }
    }
    
    return total_damage;
}

u32 enemy_manager_alive_count(const EnemyManager* manager) {
    if (!manager) return 0;
    
    u32 count = 0;
    for (u32 i = 0; i < manager->count; i++) {
        if (enemy_is_alive(&manager->enemies[i])) {
            count++;
        }
    }
    return count;
}

void enemy_take_damage(Enemy* enemy, f32 damage) {
    enemy->health -= damage;
    if (enemy->health <= 0.0f) {
        enemy->health = 0.0f;
        enemy->state = ENEMY_STATE_DEAD;
    }
}

bool enemy_is_alive(const Enemy* enemy) {
    return enemy->health > 0.0f && enemy->state != ENEMY_STATE_DEAD;
}

Mat4 enemy_get_model_matrix(const Enemy* enemy) {
    Mat4 translation = mat4_translate(enemy->position);
    Mat4 rotation = mat4_rotate_y(enemy->yaw * M_PI / 180.0f);
    return mat4_multiply(translation, rotation);
}
