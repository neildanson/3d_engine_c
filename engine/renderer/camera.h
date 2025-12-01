#ifndef CAMERA_H
#define CAMERA_H

#include "../core/types.h"
#include "../math/vec3.h"
#include "../math/mat4.h"

/* Camera structure */
typedef struct {
    Vec3 position;
    Vec3 front;
    Vec3 up;
    Vec3 right;
    Vec3 world_up;
    f32 yaw;
    f32 pitch;
    f32 fov;
    f32 aspect_ratio;
    f32 near_plane;
    f32 far_plane;
    f32 move_speed;
    f32 mouse_sensitivity;
} Camera;

/* Camera creation and initialization */
Camera* camera_create(Vec3 position, f32 yaw, f32 pitch);
void camera_destroy(Camera* camera);

/* Camera matrix getters */
Mat4 camera_get_view_matrix(const Camera* camera);
Mat4 camera_get_projection_matrix(const Camera* camera);

/* Camera updates */
void camera_update_vectors(Camera* camera);
void camera_process_mouse(Camera* camera, f32 xoffset, f32 yoffset, bool constrain_pitch);
void camera_set_aspect_ratio(Camera* camera, f32 aspect);

/* Camera movement */
typedef enum {
    CAMERA_FORWARD,
    CAMERA_BACKWARD,
    CAMERA_LEFT,
    CAMERA_RIGHT,
    CAMERA_UP,
    CAMERA_DOWN
} CameraMovement;

void camera_process_keyboard(Camera* camera, CameraMovement direction, f32 delta_time);

/* Get forward direction without Y component (for player movement) */
Vec3 camera_get_flat_front(const Camera* camera);

#endif /* CAMERA_H */
