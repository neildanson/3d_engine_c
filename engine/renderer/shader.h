#ifndef SHADER_H
#define SHADER_H

#include "../core/types.h"
#include "../math/vec3.h"
#include "../math/mat4.h"

/* Shader structure */
typedef struct {
    u32 program;
} Shader;

/* Shader creation and destruction */
Shader* shader_create(const char* vertex_source, const char* fragment_source);
void shader_destroy(Shader* shader);

/* Shader usage */
void shader_use(const Shader* shader);

/* Uniform setters */
void shader_set_int(const Shader* shader, const char* name, i32 value);
void shader_set_float(const Shader* shader, const char* name, f32 value);
void shader_set_vec3(const Shader* shader, const char* name, Vec3 value);
void shader_set_mat4(const Shader* shader, const char* name, const Mat4* value);
void shader_set_color(const Shader* shader, const char* name, Color value);

/* Get uniform location */
i32 shader_get_uniform_location(const Shader* shader, const char* name);

#endif /* SHADER_H */
