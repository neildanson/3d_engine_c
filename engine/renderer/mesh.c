#include "mesh.h"
#include <glad/glad.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Mesh* mesh_create(const Vertex* vertices, u32 vertex_count,
                  const u32* indices, u32 index_count) {
    Mesh* mesh = (Mesh*)malloc(sizeof(Mesh));
    if (!mesh) return NULL;
    
    mesh->vertex_count = vertex_count;
    mesh->index_count = index_count;
    
    glGenVertexArrays(1, &mesh->vao);
    glGenBuffers(1, &mesh->vbo);
    
    glBindVertexArray(mesh->vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(Vertex), vertices, GL_STATIC_DRAW);
    
    if (indices && index_count > 0) {
        glGenBuffers(1, &mesh->ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(u32), indices, GL_STATIC_DRAW);
    } else {
        mesh->ebo = 0;
    }
    
    /* Position attribute */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    
    /* Normal attribute */
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    
    /* Texture coordinate attribute */
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
    
    return mesh;
}

void mesh_destroy(Mesh* mesh) {
    if (!mesh) return;
    
    glDeleteVertexArrays(1, &mesh->vao);
    glDeleteBuffers(1, &mesh->vbo);
    if (mesh->ebo) {
        glDeleteBuffers(1, &mesh->ebo);
    }
    
    free(mesh);
}

void mesh_draw(const Mesh* mesh) {
    if (!mesh) return;
    
    glBindVertexArray(mesh->vao);
    
    if (mesh->ebo && mesh->index_count > 0) {
        glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, mesh->vertex_count);
    }
    
    glBindVertexArray(0);
}

Mesh* mesh_create_cube(f32 size) {
    f32 h = size / 2.0f;
    
    Vertex vertices[] = {
        /* Front face */
        {{-h, -h,  h}, { 0,  0,  1}, {0, 0}},
        {{ h, -h,  h}, { 0,  0,  1}, {1, 0}},
        {{ h,  h,  h}, { 0,  0,  1}, {1, 1}},
        {{-h,  h,  h}, { 0,  0,  1}, {0, 1}},
        /* Back face */
        {{ h, -h, -h}, { 0,  0, -1}, {0, 0}},
        {{-h, -h, -h}, { 0,  0, -1}, {1, 0}},
        {{-h,  h, -h}, { 0,  0, -1}, {1, 1}},
        {{ h,  h, -h}, { 0,  0, -1}, {0, 1}},
        /* Top face */
        {{-h,  h,  h}, { 0,  1,  0}, {0, 0}},
        {{ h,  h,  h}, { 0,  1,  0}, {1, 0}},
        {{ h,  h, -h}, { 0,  1,  0}, {1, 1}},
        {{-h,  h, -h}, { 0,  1,  0}, {0, 1}},
        /* Bottom face */
        {{-h, -h, -h}, { 0, -1,  0}, {0, 0}},
        {{ h, -h, -h}, { 0, -1,  0}, {1, 0}},
        {{ h, -h,  h}, { 0, -1,  0}, {1, 1}},
        {{-h, -h,  h}, { 0, -1,  0}, {0, 1}},
        /* Right face */
        {{ h, -h,  h}, { 1,  0,  0}, {0, 0}},
        {{ h, -h, -h}, { 1,  0,  0}, {1, 0}},
        {{ h,  h, -h}, { 1,  0,  0}, {1, 1}},
        {{ h,  h,  h}, { 1,  0,  0}, {0, 1}},
        /* Left face */
        {{-h, -h, -h}, {-1,  0,  0}, {0, 0}},
        {{-h, -h,  h}, {-1,  0,  0}, {1, 0}},
        {{-h,  h,  h}, {-1,  0,  0}, {1, 1}},
        {{-h,  h, -h}, {-1,  0,  0}, {0, 1}},
    };
    
    u32 indices[] = {
        0, 1, 2, 2, 3, 0,       /* Front */
        4, 5, 6, 6, 7, 4,       /* Back */
        8, 9, 10, 10, 11, 8,    /* Top */
        12, 13, 14, 14, 15, 12, /* Bottom */
        16, 17, 18, 18, 19, 16, /* Right */
        20, 21, 22, 22, 23, 20, /* Left */
    };
    
    return mesh_create(vertices, 24, indices, 36);
}

Mesh* mesh_create_plane(f32 width, f32 depth, u32 subdivisions) {
    u32 grid_size = subdivisions + 1;
    u32 vertex_count = grid_size * grid_size;
    u32 index_count = subdivisions * subdivisions * 6;
    
    Vertex* vertices = (Vertex*)malloc(vertex_count * sizeof(Vertex));
    u32* indices = (u32*)malloc(index_count * sizeof(u32));
    
    if (!vertices || !indices) {
        free(vertices);
        free(indices);
        return NULL;
    }
    
    f32 half_width = width / 2.0f;
    f32 half_depth = depth / 2.0f;
    f32 step_x = width / subdivisions;
    f32 step_z = depth / subdivisions;
    
    /* Generate vertices */
    for (u32 z = 0; z < grid_size; z++) {
        for (u32 x = 0; x < grid_size; x++) {
            u32 i = z * grid_size + x;
            vertices[i].position = vec3_create(
                -half_width + x * step_x,
                0.0f,
                -half_depth + z * step_z
            );
            vertices[i].normal = vec3_create(0.0f, 1.0f, 0.0f);
            vertices[i].texcoord = vec2_create(
                (f32)x / subdivisions,
                (f32)z / subdivisions
            );
        }
    }
    
    /* Generate indices */
    u32 idx = 0;
    for (u32 z = 0; z < subdivisions; z++) {
        for (u32 x = 0; x < subdivisions; x++) {
            u32 top_left = z * grid_size + x;
            u32 top_right = top_left + 1;
            u32 bottom_left = (z + 1) * grid_size + x;
            u32 bottom_right = bottom_left + 1;
            
            indices[idx++] = top_left;
            indices[idx++] = bottom_left;
            indices[idx++] = top_right;
            
            indices[idx++] = top_right;
            indices[idx++] = bottom_left;
            indices[idx++] = bottom_right;
        }
    }
    
    Mesh* mesh = mesh_create(vertices, vertex_count, indices, index_count);
    
    free(vertices);
    free(indices);
    
    return mesh;
}

Mesh* mesh_create_sphere(f32 radius, u32 rings, u32 segments) {
    u32 vertex_count = (rings + 1) * (segments + 1);
    u32 index_count = rings * segments * 6;
    
    Vertex* vertices = (Vertex*)malloc(vertex_count * sizeof(Vertex));
    u32* indices = (u32*)malloc(index_count * sizeof(u32));
    
    if (!vertices || !indices) {
        free(vertices);
        free(indices);
        return NULL;
    }
    
    /* Generate vertices */
    u32 v = 0;
    for (u32 ring = 0; ring <= rings; ring++) {
        f32 phi = (f32)ring / rings * M_PI;
        for (u32 seg = 0; seg <= segments; seg++) {
            f32 theta = (f32)seg / segments * 2.0f * M_PI;
            
            f32 x = sinf(phi) * cosf(theta);
            f32 y = cosf(phi);
            f32 z = sinf(phi) * sinf(theta);
            
            vertices[v].position = vec3_create(x * radius, y * radius, z * radius);
            vertices[v].normal = vec3_create(x, y, z);
            vertices[v].texcoord = vec2_create((f32)seg / segments, (f32)ring / rings);
            v++;
        }
    }
    
    /* Generate indices */
    u32 idx = 0;
    for (u32 ring = 0; ring < rings; ring++) {
        for (u32 seg = 0; seg < segments; seg++) {
            u32 current = ring * (segments + 1) + seg;
            u32 next = current + segments + 1;
            
            indices[idx++] = current;
            indices[idx++] = next;
            indices[idx++] = current + 1;
            
            indices[idx++] = current + 1;
            indices[idx++] = next;
            indices[idx++] = next + 1;
        }
    }
    
    Mesh* mesh = mesh_create(vertices, vertex_count, indices, index_count);
    
    free(vertices);
    free(indices);
    
    return mesh;
}
