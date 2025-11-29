#ifndef PLAYER_H
#define PLAYER_H

#include "../engine/core/types.h"
#include "../engine/math/vec3.h"
#include "../engine/renderer/mesh.h"
#include "../engine/renderer/camera.h"
#include "../engine/input/input.h"
#include "../engine/resource/terrain.h"

/* Player state */
typedef struct {
    Vec3 position;
    Vec3 velocity;
    f32 yaw;
    f32 pitch;
    f32 move_speed;
    f32 jump_force;
    f32 gravity;
    f32 health;
    f32 max_health;
    bool on_ground;
    bool is_jumping;
    Mesh* mesh;
    Camera* camera;
    f32 camera_height;
    f32 player_radius;
} Player;

/* Create and destroy player */
Player* player_create(Vec3 start_position, Mesh* mesh);
void player_destroy(Player* player);

/* Update player */
void player_update(Player* player, const InputState* input, const Terrain* terrain, f32 delta_time);

/* Get player transform matrix */
Mat4 player_get_model_matrix(const Player* player);

/* Player actions */
void player_jump(Player* player);
void player_take_damage(Player* player, f32 damage);
bool player_is_alive(const Player* player);

/* Get camera for rendering */
Camera* player_get_camera(const Player* player);

#endif /* PLAYER_H */
