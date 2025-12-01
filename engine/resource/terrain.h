#ifndef TERRAIN_H
#define TERRAIN_H

#include "../core/types.h"
#include "../renderer/mesh.h"
#include "../math/vec3.h"

/* Terrain structure */
typedef struct {
    Mesh* mesh;
    f32* heights;
    u32 width;
    u32 depth;
    f32 scale_x;
    f32 scale_y;
    f32 scale_z;
    f32 min_height;
    f32 max_height;
} Terrain;

/* Create terrain from heightmap data (grayscale, 8-bit per pixel) */
Terrain* terrain_create_from_heightmap(const u8* heightmap_data, u32 width, u32 height,
                                       f32 scale_x, f32 scale_y, f32 scale_z);

/* Create terrain from procedural noise */
Terrain* terrain_create_procedural(u32 width, u32 depth, f32 scale_x, f32 scale_y, f32 scale_z);

/* Destroy terrain */
void terrain_destroy(Terrain* terrain);

/* Get height at world position */
f32 terrain_get_height_at(const Terrain* terrain, f32 world_x, f32 world_z);

/* Get normal at world position */
Vec3 terrain_get_normal_at(const Terrain* terrain, f32 world_x, f32 world_z);

/* Draw terrain */
void terrain_draw(const Terrain* terrain);

#endif /* TERRAIN_H */
