#include "terrain.h"
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* Simple noise function for procedural terrain */
static f32 noise2d(f32 x, f32 y) {
    i32 xi = (i32)floorf(x);
    i32 yi = (i32)floorf(y);
    
    /* Hash function */
    u32 h = xi * 374761393 + yi * 668265263;
    h = (h ^ (h >> 13)) * 1274126177;
    return (f32)(h & 0x7fffffff) / (f32)0x7fffffff;
}

static f32 smooth_noise2d(f32 x, f32 y) {
    f32 corners = (noise2d(x-1, y-1) + noise2d(x+1, y-1) + 
                   noise2d(x-1, y+1) + noise2d(x+1, y+1)) / 16.0f;
    f32 sides = (noise2d(x-1, y) + noise2d(x+1, y) + 
                 noise2d(x, y-1) + noise2d(x, y+1)) / 8.0f;
    f32 center = noise2d(x, y) / 4.0f;
    return corners + sides + center;
}

static f32 interpolate(f32 a, f32 b, f32 t) {
    f32 f = (1.0f - cosf(t * M_PI)) * 0.5f;
    return a * (1.0f - f) + b * f;
}

static f32 interpolated_noise2d(f32 x, f32 y) {
    i32 ix = (i32)floorf(x);
    i32 iy = (i32)floorf(y);
    f32 fx = x - ix;
    f32 fy = y - iy;
    
    f32 v1 = smooth_noise2d(ix, iy);
    f32 v2 = smooth_noise2d(ix + 1, iy);
    f32 v3 = smooth_noise2d(ix, iy + 1);
    f32 v4 = smooth_noise2d(ix + 1, iy + 1);
    
    f32 i1 = interpolate(v1, v2, fx);
    f32 i2 = interpolate(v3, v4, fx);
    
    return interpolate(i1, i2, fy);
}

static f32 perlin_noise2d(f32 x, f32 y, i32 octaves, f32 persistence) {
    f32 total = 0.0f;
    f32 frequency = 1.0f;
    f32 amplitude = 1.0f;
    f32 max_value = 0.0f;
    
    for (i32 i = 0; i < octaves; i++) {
        total += interpolated_noise2d(x * frequency, y * frequency) * amplitude;
        max_value += amplitude;
        amplitude *= persistence;
        frequency *= 2.0f;
    }
    
    return total / max_value;
}

static void calculate_normals(Terrain* terrain, Vertex* vertices) {
    u32 grid_size = terrain->width;
    
    for (u32 z = 0; z < terrain->depth; z++) {
        for (u32 x = 0; x < terrain->width; x++) {
            f32 hL = (x > 0) ? terrain->heights[(z * grid_size) + (x - 1)] : terrain->heights[(z * grid_size) + x];
            f32 hR = (x < terrain->width - 1) ? terrain->heights[(z * grid_size) + (x + 1)] : terrain->heights[(z * grid_size) + x];
            f32 hD = (z > 0) ? terrain->heights[((z - 1) * grid_size) + x] : terrain->heights[(z * grid_size) + x];
            f32 hU = (z < terrain->depth - 1) ? terrain->heights[((z + 1) * grid_size) + x] : terrain->heights[(z * grid_size) + x];
            
            Vec3 normal = vec3_create(hL - hR, 2.0f, hD - hU);
            vertices[(z * grid_size) + x].normal = vec3_normalize(normal);
        }
    }
}

Terrain* terrain_create_from_heightmap(const u8* heightmap_data, u32 width, u32 height,
                                       f32 scale_x, f32 scale_y, f32 scale_z) {
    Terrain* terrain = (Terrain*)malloc(sizeof(Terrain));
    if (!terrain) return NULL;
    
    terrain->width = width;
    terrain->depth = height;
    terrain->scale_x = scale_x;
    terrain->scale_y = scale_y;
    terrain->scale_z = scale_z;
    terrain->min_height = 0.0f;
    terrain->max_height = scale_y;
    
    /* Allocate heights array */
    terrain->heights = (f32*)malloc(width * height * sizeof(f32));
    if (!terrain->heights) {
        free(terrain);
        return NULL;
    }
    
    /* Convert heightmap to float heights */
    for (u32 i = 0; i < width * height; i++) {
        terrain->heights[i] = (f32)heightmap_data[i] / 255.0f * scale_y;
    }
    
    /* Create vertices */
    u32 vertex_count = width * height;
    Vertex* vertices = (Vertex*)malloc(vertex_count * sizeof(Vertex));
    if (!vertices) {
        free(terrain->heights);
        free(terrain);
        return NULL;
    }
    
    f32 half_width = (width - 1) * scale_x / 2.0f;
    f32 half_depth = (height - 1) * scale_z / 2.0f;
    
    for (u32 z = 0; z < height; z++) {
        for (u32 x = 0; x < width; x++) {
            u32 i = z * width + x;
            vertices[i].position = vec3_create(
                x * scale_x - half_width,
                terrain->heights[i],
                z * scale_z - half_depth
            );
            vertices[i].texcoord = vec2_create(
                (f32)x / (width - 1),
                (f32)z / (height - 1)
            );
        }
    }
    
    /* Calculate normals */
    calculate_normals(terrain, vertices);
    
    /* Create indices */
    u32 index_count = (width - 1) * (height - 1) * 6;
    u32* indices = (u32*)malloc(index_count * sizeof(u32));
    if (!indices) {
        free(vertices);
        free(terrain->heights);
        free(terrain);
        return NULL;
    }
    
    u32 idx = 0;
    for (u32 z = 0; z < height - 1; z++) {
        for (u32 x = 0; x < width - 1; x++) {
            u32 top_left = z * width + x;
            u32 top_right = top_left + 1;
            u32 bottom_left = (z + 1) * width + x;
            u32 bottom_right = bottom_left + 1;
            
            indices[idx++] = top_left;
            indices[idx++] = bottom_left;
            indices[idx++] = top_right;
            
            indices[idx++] = top_right;
            indices[idx++] = bottom_left;
            indices[idx++] = bottom_right;
        }
    }
    
    terrain->mesh = mesh_create(vertices, vertex_count, indices, index_count);
    
    free(vertices);
    free(indices);
    
    if (!terrain->mesh) {
        free(terrain->heights);
        free(terrain);
        return NULL;
    }
    
    return terrain;
}

Terrain* terrain_create_procedural(u32 width, u32 depth, f32 scale_x, f32 scale_y, f32 scale_z) {
    /* Generate procedural heightmap */
    u8* heightmap = (u8*)malloc(width * depth);
    if (!heightmap) return NULL;
    
    for (u32 z = 0; z < depth; z++) {
        for (u32 x = 0; x < width; x++) {
            f32 nx = (f32)x / width * 4.0f;
            f32 nz = (f32)z / depth * 4.0f;
            f32 h = perlin_noise2d(nx, nz, 4, 0.5f);
            heightmap[z * width + x] = (u8)(h * 255.0f);
        }
    }
    
    Terrain* terrain = terrain_create_from_heightmap(heightmap, width, depth, scale_x, scale_y, scale_z);
    free(heightmap);
    
    return terrain;
}

void terrain_destroy(Terrain* terrain) {
    if (!terrain) return;
    
    if (terrain->mesh) {
        mesh_destroy(terrain->mesh);
    }
    free(terrain->heights);
    free(terrain);
}

f32 terrain_get_height_at(const Terrain* terrain, f32 world_x, f32 world_z) {
    if (!terrain) return 0.0f;
    
    f32 half_width = (terrain->width - 1) * terrain->scale_x / 2.0f;
    f32 half_depth = (terrain->depth - 1) * terrain->scale_z / 2.0f;
    
    /* Convert world coordinates to terrain grid coordinates */
    f32 grid_x = (world_x + half_width) / terrain->scale_x;
    f32 grid_z = (world_z + half_depth) / terrain->scale_z;
    
    /* Clamp to terrain bounds */
    if (grid_x < 0) grid_x = 0;
    if (grid_z < 0) grid_z = 0;
    if (grid_x >= terrain->width - 1) grid_x = terrain->width - 1.001f;
    if (grid_z >= terrain->depth - 1) grid_z = terrain->depth - 1.001f;
    
    /* Get integer and fractional parts */
    u32 x0 = (u32)grid_x;
    u32 z0 = (u32)grid_z;
    f32 fx = grid_x - x0;
    f32 fz = grid_z - z0;
    
    /* Get heights at four corners */
    f32 h00 = terrain->heights[z0 * terrain->width + x0];
    f32 h10 = terrain->heights[z0 * terrain->width + (x0 + 1)];
    f32 h01 = terrain->heights[(z0 + 1) * terrain->width + x0];
    f32 h11 = terrain->heights[(z0 + 1) * terrain->width + (x0 + 1)];
    
    /* Bilinear interpolation */
    f32 h0 = h00 * (1.0f - fx) + h10 * fx;
    f32 h1 = h01 * (1.0f - fx) + h11 * fx;
    
    return h0 * (1.0f - fz) + h1 * fz;
}

Vec3 terrain_get_normal_at(const Terrain* terrain, f32 world_x, f32 world_z) {
    f32 delta = terrain->scale_x;
    f32 hL = terrain_get_height_at(terrain, world_x - delta, world_z);
    f32 hR = terrain_get_height_at(terrain, world_x + delta, world_z);
    f32 hD = terrain_get_height_at(terrain, world_x, world_z - delta);
    f32 hU = terrain_get_height_at(terrain, world_x, world_z + delta);
    
    Vec3 normal = vec3_create(hL - hR, 2.0f * delta, hD - hU);
    return vec3_normalize(normal);
}

void terrain_draw(const Terrain* terrain) {
    if (terrain && terrain->mesh) {
        mesh_draw(terrain->mesh);
    }
}
