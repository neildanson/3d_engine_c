#ifndef ENEMY_H
#define ENEMY_H

#include "../engine/core/types.h"
#include "../engine/math/vec3.h"
#include "../engine/math/mat4.h"
#include "../engine/renderer/mesh.h"
#include "../engine/resource/terrain.h"

/* Enemy state */
typedef enum {
    ENEMY_STATE_IDLE,
    ENEMY_STATE_PATROL,
    ENEMY_STATE_CHASE,
    ENEMY_STATE_ATTACK,
    ENEMY_STATE_DEAD
} EnemyState;

/* Enemy structure */
typedef struct {
    Vec3 position;
    Vec3 velocity;
    f32 yaw;
    f32 move_speed;
    f32 health;
    f32 max_health;
    f32 attack_damage;
    f32 attack_range;
    f32 detection_range;
    f32 attack_cooldown;
    f32 last_attack_time;
    EnemyState state;
    Mesh* mesh;
    f32 enemy_radius;
} Enemy;

/* Enemy manager to handle multiple enemies */
typedef struct {
    Enemy* enemies;
    u32 count;
    u32 capacity;
    Mesh* shared_mesh;
} EnemyManager;

/* Create and destroy enemy manager */
EnemyManager* enemy_manager_create(u32 initial_capacity);
void enemy_manager_destroy(EnemyManager* manager);

/* Add enemy at position */
Enemy* enemy_manager_spawn(EnemyManager* manager, Vec3 position, f32 health);

/* Update all enemies */
void enemy_manager_update(EnemyManager* manager, Vec3 player_position, 
                          const Terrain* terrain, f32 delta_time, f32 current_time);

/* Check for attacks on player - returns damage dealt */
f32 enemy_manager_check_attacks(EnemyManager* manager, Vec3 player_position, f32 current_time);

/* Get number of alive enemies */
u32 enemy_manager_alive_count(const EnemyManager* manager);

/* Individual enemy functions */
void enemy_take_damage(Enemy* enemy, f32 damage);
bool enemy_is_alive(const Enemy* enemy);
Mat4 enemy_get_model_matrix(const Enemy* enemy);

/* Set shared mesh for all enemies */
void enemy_manager_set_mesh(EnemyManager* manager, Mesh* mesh);

#endif /* ENEMY_H */
