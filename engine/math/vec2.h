#ifndef VEC2_H
#define VEC2_H

#include <math.h>

typedef struct {
    float x, y;
} Vec2;

static inline Vec2 vec2_create(float x, float y) {
    return (Vec2){x, y};
}

static inline Vec2 vec2_add(Vec2 a, Vec2 b) {
    return (Vec2){a.x + b.x, a.y + b.y};
}

static inline Vec2 vec2_sub(Vec2 a, Vec2 b) {
    return (Vec2){a.x - b.x, a.y - b.y};
}

static inline Vec2 vec2_scale(Vec2 v, float s) {
    return (Vec2){v.x * s, v.y * s};
}

static inline float vec2_dot(Vec2 a, Vec2 b) {
    return a.x * b.x + a.y * b.y;
}

static inline float vec2_length(Vec2 v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

static inline Vec2 vec2_normalize(Vec2 v) {
    float len = vec2_length(v);
    if (len > 0.0001f) {
        return vec2_scale(v, 1.0f / len);
    }
    return v;
}

#endif /* VEC2_H */
