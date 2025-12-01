#include "camera.h"
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static const f32 DEFAULT_YAW = -90.0f;
static const f32 DEFAULT_PITCH = 0.0f;
static const f32 DEFAULT_FOV = 45.0f;
static const f32 DEFAULT_SPEED = 5.0f;
static const f32 DEFAULT_SENSITIVITY = 0.1f;

Camera* camera_create(Vec3 position, f32 yaw, f32 pitch) {
    Camera* camera = (Camera*)malloc(sizeof(Camera));
    if (!camera) return NULL;
    
    camera->position = position;
    camera->world_up = vec3_create(0.0f, 1.0f, 0.0f);
    camera->yaw = yaw;
    camera->pitch = pitch;
    camera->fov = DEFAULT_FOV;
    camera->aspect_ratio = 16.0f / 9.0f;
    camera->near_plane = 0.1f;
    camera->far_plane = 1000.0f;
    camera->move_speed = DEFAULT_SPEED;
    camera->mouse_sensitivity = DEFAULT_SENSITIVITY;
    
    camera_update_vectors(camera);
    
    return camera;
}

void camera_destroy(Camera* camera) {
    free(camera);
}

void camera_update_vectors(Camera* camera) {
    Vec3 front;
    f32 yaw_rad = camera->yaw * M_PI / 180.0f;
    f32 pitch_rad = camera->pitch * M_PI / 180.0f;
    
    front.x = cosf(yaw_rad) * cosf(pitch_rad);
    front.y = sinf(pitch_rad);
    front.z = sinf(yaw_rad) * cosf(pitch_rad);
    
    camera->front = vec3_normalize(front);
    camera->right = vec3_normalize(vec3_cross(camera->front, camera->world_up));
    camera->up = vec3_normalize(vec3_cross(camera->right, camera->front));
}

Mat4 camera_get_view_matrix(const Camera* camera) {
    Vec3 center = vec3_add(camera->position, camera->front);
    return mat4_look_at(camera->position, center, camera->up);
}

Mat4 camera_get_projection_matrix(const Camera* camera) {
    f32 fov_rad = camera->fov * M_PI / 180.0f;
    return mat4_perspective(fov_rad, camera->aspect_ratio, camera->near_plane, camera->far_plane);
}

void camera_process_mouse(Camera* camera, f32 xoffset, f32 yoffset, bool constrain_pitch) {
    xoffset *= camera->mouse_sensitivity;
    yoffset *= camera->mouse_sensitivity;
    
    camera->yaw += xoffset;
    camera->pitch += yoffset;
    
    if (constrain_pitch) {
        if (camera->pitch > 89.0f)
            camera->pitch = 89.0f;
        if (camera->pitch < -89.0f)
            camera->pitch = -89.0f;
    }
    
    camera_update_vectors(camera);
}

void camera_set_aspect_ratio(Camera* camera, f32 aspect) {
    camera->aspect_ratio = aspect;
}

void camera_process_keyboard(Camera* camera, CameraMovement direction, f32 delta_time) {
    f32 velocity = camera->move_speed * delta_time;
    
    switch (direction) {
        case CAMERA_FORWARD:
            camera->position = vec3_add(camera->position, vec3_scale(camera->front, velocity));
            break;
        case CAMERA_BACKWARD:
            camera->position = vec3_sub(camera->position, vec3_scale(camera->front, velocity));
            break;
        case CAMERA_LEFT:
            camera->position = vec3_sub(camera->position, vec3_scale(camera->right, velocity));
            break;
        case CAMERA_RIGHT:
            camera->position = vec3_add(camera->position, vec3_scale(camera->right, velocity));
            break;
        case CAMERA_UP:
            camera->position = vec3_add(camera->position, vec3_scale(camera->up, velocity));
            break;
        case CAMERA_DOWN:
            camera->position = vec3_sub(camera->position, vec3_scale(camera->up, velocity));
            break;
    }
}

Vec3 camera_get_flat_front(const Camera* camera) {
    Vec3 flat = vec3_create(camera->front.x, 0.0f, camera->front.z);
    return vec3_normalize(flat);
}
