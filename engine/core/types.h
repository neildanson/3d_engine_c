#ifndef ENGINE_TYPES_H
#define ENGINE_TYPES_H

#include <stdint.h>
#include <stdbool.h>

/* Common type definitions */
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef float    f32;
typedef double   f64;

/* Color structure */
typedef struct {
    f32 r, g, b, a;
} Color;

static inline Color color_create(f32 r, f32 g, f32 b, f32 a) {
    return (Color){r, g, b, a};
}

#define COLOR_WHITE  (Color){1.0f, 1.0f, 1.0f, 1.0f}
#define COLOR_BLACK  (Color){0.0f, 0.0f, 0.0f, 1.0f}
#define COLOR_RED    (Color){1.0f, 0.0f, 0.0f, 1.0f}
#define COLOR_GREEN  (Color){0.0f, 1.0f, 0.0f, 1.0f}
#define COLOR_BLUE   (Color){0.0f, 0.0f, 1.0f, 1.0f}

/* Handle types for resources */
typedef u32 MeshHandle;
typedef u32 TextureHandle;
typedef u32 ShaderHandle;

#define INVALID_HANDLE ((u32)-1)

#endif /* ENGINE_TYPES_H */
