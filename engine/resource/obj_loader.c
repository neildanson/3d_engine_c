#include "obj_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VERTICES 100000
#define MAX_FACES 100000

typedef struct {
    Vec3* positions;
    Vec3* normals;
    Vec2* texcoords;
    u32 position_count;
    u32 normal_count;
    u32 texcoord_count;
} ObjData;

static char* read_file(const char* filepath, u32* out_size) {
    FILE* file = fopen(filepath, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", filepath);
        return NULL;
    }
    
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* buffer = (char*)malloc(size + 1);
    if (!buffer) {
        fclose(file);
        return NULL;
    }
    
    fread(buffer, 1, size, file);
    buffer[size] = '\0';
    fclose(file);
    
    if (out_size) *out_size = (u32)size;
    return buffer;
}

static void parse_face_vertex(const char* str, i32* pos_idx, i32* tex_idx, i32* norm_idx) {
    *pos_idx = 0;
    *tex_idx = 0;
    *norm_idx = 0;
    
    /* Parse position index */
    *pos_idx = atoi(str);
    
    /* Find first slash */
    const char* slash1 = strchr(str, '/');
    if (!slash1) return;
    
    /* Parse texture coordinate index */
    if (slash1[1] != '/') {
        *tex_idx = atoi(slash1 + 1);
    }
    
    /* Find second slash */
    const char* slash2 = strchr(slash1 + 1, '/');
    if (!slash2) return;
    
    /* Parse normal index */
    *norm_idx = atoi(slash2 + 1);
}

Mesh* obj_loader_parse(const char* data, u32 data_size) {
    (void)data_size;
    
    /* Allocate temporary storage */
    Vec3* positions = (Vec3*)malloc(MAX_VERTICES * sizeof(Vec3));
    Vec3* normals = (Vec3*)malloc(MAX_VERTICES * sizeof(Vec3));
    Vec2* texcoords = (Vec2*)malloc(MAX_VERTICES * sizeof(Vec2));
    Vertex* vertices = (Vertex*)malloc(MAX_FACES * 3 * sizeof(Vertex));
    u32* indices = (u32*)malloc(MAX_FACES * 3 * sizeof(u32));
    
    if (!positions || !normals || !texcoords || !vertices || !indices) {
        free(positions);
        free(normals);
        free(texcoords);
        free(vertices);
        free(indices);
        return NULL;
    }
    
    u32 pos_count = 0;
    u32 norm_count = 0;
    u32 tex_count = 0;
    u32 vertex_count = 0;
    u32 index_count = 0;
    
    /* Parse line by line */
    char line[256];
    const char* ptr = data;
    
    while (*ptr) {
        /* Read line */
        const char* line_end = strchr(ptr, '\n');
        if (!line_end) line_end = ptr + strlen(ptr);
        
        size_t line_len = line_end - ptr;
        if (line_len >= sizeof(line)) line_len = sizeof(line) - 1;
        
        strncpy(line, ptr, line_len);
        line[line_len] = '\0';
        
        /* Remove carriage return if present */
        char* cr = strchr(line, '\r');
        if (cr) *cr = '\0';
        
        /* Parse line */
        if (line[0] == 'v' && line[1] == ' ') {
            /* Vertex position */
            Vec3 v;
            sscanf(line + 2, "%f %f %f", &v.x, &v.y, &v.z);
            if (pos_count < MAX_VERTICES) {
                positions[pos_count++] = v;
            }
        } else if (line[0] == 'v' && line[1] == 'n') {
            /* Vertex normal */
            Vec3 n;
            sscanf(line + 3, "%f %f %f", &n.x, &n.y, &n.z);
            if (norm_count < MAX_VERTICES) {
                normals[norm_count++] = n;
            }
        } else if (line[0] == 'v' && line[1] == 't') {
            /* Texture coordinate */
            Vec2 t;
            sscanf(line + 3, "%f %f", &t.x, &t.y);
            if (tex_count < MAX_VERTICES) {
                texcoords[tex_count++] = t;
            }
        } else if (line[0] == 'f' && line[1] == ' ') {
            /* Face */
            char v1[64], v2[64], v3[64], v4[64];
            int matches = sscanf(line + 2, "%s %s %s %s", v1, v2, v3, v4);
            
            if (matches >= 3) {
                i32 pos_idx[4], tex_idx[4], norm_idx[4];
                
                parse_face_vertex(v1, &pos_idx[0], &tex_idx[0], &norm_idx[0]);
                parse_face_vertex(v2, &pos_idx[1], &tex_idx[1], &norm_idx[1]);
                parse_face_vertex(v3, &pos_idx[2], &tex_idx[2], &norm_idx[2]);
                
                /* Add first triangle */
                for (int i = 0; i < 3 && vertex_count < MAX_FACES * 3; i++) {
                    Vertex* vert = &vertices[vertex_count];
                    
                    /* Handle negative indices (relative to end) */
                    i32 pi = pos_idx[i] > 0 ? pos_idx[i] - 1 : (i32)pos_count + pos_idx[i];
                    i32 ti = tex_idx[i] > 0 ? tex_idx[i] - 1 : (i32)tex_count + tex_idx[i];
                    i32 ni = norm_idx[i] > 0 ? norm_idx[i] - 1 : (i32)norm_count + norm_idx[i];
                    
                    if (pi >= 0 && pi < (i32)pos_count) {
                        vert->position = positions[pi];
                    } else {
                        vert->position = vec3_create(0, 0, 0);
                    }
                    
                    if (ti >= 0 && ti < (i32)tex_count) {
                        vert->texcoord = texcoords[ti];
                    } else {
                        vert->texcoord = vec2_create(0, 0);
                    }
                    
                    if (ni >= 0 && ni < (i32)norm_count) {
                        vert->normal = normals[ni];
                    } else {
                        vert->normal = vec3_create(0, 1, 0);
                    }
                    
                    indices[index_count++] = vertex_count++;
                }
                
                /* Handle quad (4 vertices) - add second triangle */
                if (matches >= 4) {
                    parse_face_vertex(v4, &pos_idx[3], &tex_idx[3], &norm_idx[3]);
                    
                    int quad_indices[] = {0, 2, 3};
                    for (int i = 0; i < 3 && vertex_count < MAX_FACES * 3; i++) {
                        int vi = quad_indices[i];
                        Vertex* vert = &vertices[vertex_count];
                        
                        i32 pi = pos_idx[vi] > 0 ? pos_idx[vi] - 1 : (i32)pos_count + pos_idx[vi];
                        i32 ti = tex_idx[vi] > 0 ? tex_idx[vi] - 1 : (i32)tex_count + tex_idx[vi];
                        i32 ni = norm_idx[vi] > 0 ? norm_idx[vi] - 1 : (i32)norm_count + norm_idx[vi];
                        
                        if (pi >= 0 && pi < (i32)pos_count) {
                            vert->position = positions[pi];
                        } else {
                            vert->position = vec3_create(0, 0, 0);
                        }
                        
                        if (ti >= 0 && ti < (i32)tex_count) {
                            vert->texcoord = texcoords[ti];
                        } else {
                            vert->texcoord = vec2_create(0, 0);
                        }
                        
                        if (ni >= 0 && ni < (i32)norm_count) {
                            vert->normal = normals[ni];
                        } else {
                            vert->normal = vec3_create(0, 1, 0);
                        }
                        
                        indices[index_count++] = vertex_count++;
                    }
                }
            }
        }
        
        /* Move to next line */
        ptr = *line_end ? line_end + 1 : line_end;
    }
    
    /* Create mesh */
    Mesh* mesh = NULL;
    if (vertex_count > 0) {
        mesh = mesh_create(vertices, vertex_count, indices, index_count);
    }
    
    free(positions);
    free(normals);
    free(texcoords);
    free(vertices);
    free(indices);
    
    return mesh;
}

Mesh* obj_loader_load(const char* filepath) {
    u32 size;
    char* data = read_file(filepath, &size);
    if (!data) return NULL;
    
    Mesh* mesh = obj_loader_parse(data, size);
    free(data);
    
    return mesh;
}
