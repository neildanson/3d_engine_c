#ifndef MAT4_H
#define MAT4_H

#include "vec3.h"
#include <string.h>

typedef struct {
    float m[16]; /* Column-major order for OpenGL */
} Mat4;

static inline Mat4 mat4_identity(void) {
    Mat4 result = {0};
    result.m[0] = 1.0f;
    result.m[5] = 1.0f;
    result.m[10] = 1.0f;
    result.m[15] = 1.0f;
    return result;
}

static inline Mat4 mat4_multiply(Mat4 a, Mat4 b) {
    Mat4 result = {0};
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[i * 4 + j] = 
                a.m[j] * b.m[i * 4] +
                a.m[j + 4] * b.m[i * 4 + 1] +
                a.m[j + 8] * b.m[i * 4 + 2] +
                a.m[j + 12] * b.m[i * 4 + 3];
        }
    }
    return result;
}

static inline Mat4 mat4_translate(Vec3 v) {
    Mat4 result = mat4_identity();
    result.m[12] = v.x;
    result.m[13] = v.y;
    result.m[14] = v.z;
    return result;
}

static inline Mat4 mat4_scale(Vec3 v) {
    Mat4 result = mat4_identity();
    result.m[0] = v.x;
    result.m[5] = v.y;
    result.m[10] = v.z;
    return result;
}

static inline Mat4 mat4_rotate_x(float angle) {
    Mat4 result = mat4_identity();
    float c = cosf(angle);
    float s = sinf(angle);
    result.m[5] = c;
    result.m[6] = s;
    result.m[9] = -s;
    result.m[10] = c;
    return result;
}

static inline Mat4 mat4_rotate_y(float angle) {
    Mat4 result = mat4_identity();
    float c = cosf(angle);
    float s = sinf(angle);
    result.m[0] = c;
    result.m[2] = -s;
    result.m[8] = s;
    result.m[10] = c;
    return result;
}

static inline Mat4 mat4_rotate_z(float angle) {
    Mat4 result = mat4_identity();
    float c = cosf(angle);
    float s = sinf(angle);
    result.m[0] = c;
    result.m[1] = s;
    result.m[4] = -s;
    result.m[5] = c;
    return result;
}

static inline Mat4 mat4_perspective(float fov, float aspect, float near, float far) {
    Mat4 result = {0};
    float tan_half_fov = tanf(fov / 2.0f);
    result.m[0] = 1.0f / (aspect * tan_half_fov);
    result.m[5] = 1.0f / tan_half_fov;
    result.m[10] = -(far + near) / (far - near);
    result.m[11] = -1.0f;
    result.m[14] = -(2.0f * far * near) / (far - near);
    return result;
}

static inline Mat4 mat4_look_at(Vec3 eye, Vec3 center, Vec3 up) {
    Vec3 f = vec3_normalize(vec3_sub(center, eye));
    Vec3 s = vec3_normalize(vec3_cross(f, up));
    Vec3 u = vec3_cross(s, f);
    
    Mat4 result = mat4_identity();
    result.m[0] = s.x;
    result.m[4] = s.y;
    result.m[8] = s.z;
    result.m[1] = u.x;
    result.m[5] = u.y;
    result.m[9] = u.z;
    result.m[2] = -f.x;
    result.m[6] = -f.y;
    result.m[10] = -f.z;
    result.m[12] = -vec3_dot(s, eye);
    result.m[13] = -vec3_dot(u, eye);
    result.m[14] = vec3_dot(f, eye);
    return result;
}

static inline Vec3 mat4_transform_point(Mat4 m, Vec3 v) {
    float w = m.m[3] * v.x + m.m[7] * v.y + m.m[11] * v.z + m.m[15];
    return (Vec3){
        (m.m[0] * v.x + m.m[4] * v.y + m.m[8] * v.z + m.m[12]) / w,
        (m.m[1] * v.x + m.m[5] * v.y + m.m[9] * v.z + m.m[13]) / w,
        (m.m[2] * v.x + m.m[6] * v.y + m.m[10] * v.z + m.m[14]) / w
    };
}

static inline Vec3 mat4_transform_direction(Mat4 m, Vec3 v) {
    return (Vec3){
        m.m[0] * v.x + m.m[4] * v.y + m.m[8] * v.z,
        m.m[1] * v.x + m.m[5] * v.y + m.m[9] * v.z,
        m.m[2] * v.x + m.m[6] * v.y + m.m[10] * v.z
    };
}

#endif /* MAT4_H */
