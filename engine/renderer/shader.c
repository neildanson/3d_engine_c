#include "shader.h"
#include <glad/glad.h>
#include <stdlib.h>
#include <stdio.h>

static u32 compile_shader(const char* source, GLenum type) {
    u32 shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    i32 success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        fprintf(stderr, "Shader compilation error: %s\n", info_log);
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

Shader* shader_create(const char* vertex_source, const char* fragment_source) {
    Shader* shader = (Shader*)malloc(sizeof(Shader));
    if (!shader) return NULL;
    
    u32 vertex_shader = compile_shader(vertex_source, GL_VERTEX_SHADER);
    u32 fragment_shader = compile_shader(fragment_source, GL_FRAGMENT_SHADER);
    
    if (!vertex_shader || !fragment_shader) {
        if (vertex_shader) glDeleteShader(vertex_shader);
        if (fragment_shader) glDeleteShader(fragment_shader);
        free(shader);
        return NULL;
    }
    
    shader->program = glCreateProgram();
    glAttachShader(shader->program, vertex_shader);
    glAttachShader(shader->program, fragment_shader);
    glLinkProgram(shader->program);
    
    i32 success;
    glGetProgramiv(shader->program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetProgramInfoLog(shader->program, 512, NULL, info_log);
        fprintf(stderr, "Shader link error: %s\n", info_log);
        glDeleteProgram(shader->program);
        free(shader);
        shader = NULL;
    }
    
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    
    return shader;
}

void shader_destroy(Shader* shader) {
    if (!shader) return;
    glDeleteProgram(shader->program);
    free(shader);
}

void shader_use(const Shader* shader) {
    if (shader) {
        glUseProgram(shader->program);
    }
}

i32 shader_get_uniform_location(const Shader* shader, const char* name) {
    return glGetUniformLocation(shader->program, name);
}

void shader_set_int(const Shader* shader, const char* name, i32 value) {
    glUniform1i(shader_get_uniform_location(shader, name), value);
}

void shader_set_float(const Shader* shader, const char* name, f32 value) {
    glUniform1f(shader_get_uniform_location(shader, name), value);
}

void shader_set_vec3(const Shader* shader, const char* name, Vec3 value) {
    glUniform3f(shader_get_uniform_location(shader, name), value.x, value.y, value.z);
}

void shader_set_mat4(const Shader* shader, const char* name, const Mat4* value) {
    glUniformMatrix4fv(shader_get_uniform_location(shader, name), 1, GL_FALSE, value->m);
}

void shader_set_color(const Shader* shader, const char* name, Color value) {
    glUniform4f(shader_get_uniform_location(shader, name), value.r, value.g, value.b, value.a);
}
