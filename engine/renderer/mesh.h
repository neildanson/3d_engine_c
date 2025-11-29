#ifndef MESH_H
#define MESH_H

#include "../core/types.h"
#include "../math/vec3.h"
#include "../math/vec2.h"

/* Vertex structure */
typedef struct {
    Vec3 position;
    Vec3 normal;
    Vec2 texcoord;
} Vertex;

/* Mesh structure */
typedef struct {
    u32 vao;      /* Vertex Array Object */
    u32 vbo;      /* Vertex Buffer Object */
    u32 ebo;      /* Element Buffer Object */
    u32 vertex_count;
    u32 index_count;
} Mesh;

/* Mesh creation and destruction */
Mesh* mesh_create(const Vertex* vertices, u32 vertex_count, 
                  const u32* indices, u32 index_count);
void mesh_destroy(Mesh* mesh);

/* Mesh rendering */
void mesh_draw(const Mesh* mesh);

/* Primitive mesh creation */
Mesh* mesh_create_cube(f32 size);
Mesh* mesh_create_plane(f32 width, f32 depth, u32 subdivisions);
Mesh* mesh_create_sphere(f32 radius, u32 rings, u32 segments);

#endif /* MESH_H */
