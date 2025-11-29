#include "engine.h"
#include "../input/input.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

/* Internal engine state */
struct Engine {
    GLFWwindow* window;
    InputState input;
    f64 last_frame_time;
    f64 delta_time;
    i32 window_width;
    i32 window_height;
    bool mouse_captured;
    bool first_mouse;
};

/* Global engine pointer for callbacks */
static Engine* g_engine = NULL;

/* GLFW Callbacks */
static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    (void)window;
    glViewport(0, 0, width, height);
    if (g_engine) {
        g_engine->window_width = width;
        g_engine->window_height = height;
    }
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)window;
    (void)scancode;
    (void)mods;
    if (g_engine) {
        input_process_key(&g_engine->input, key, action);
    }
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    (void)window;
    (void)mods;
    if (g_engine) {
        input_process_mouse_button(&g_engine->input, button, action);
    }
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    (void)window;
    if (g_engine) {
        if (g_engine->first_mouse && g_engine->mouse_captured) {
            g_engine->input.mouse_x = xpos;
            g_engine->input.mouse_y = ypos;
            g_engine->first_mouse = false;
        }
        input_process_mouse_move(&g_engine->input, xpos, ypos);
    }
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    (void)window;
    if (g_engine) {
        input_process_scroll(&g_engine->input, xoffset, yoffset);
    }
}

Engine* engine_create(const EngineConfig* config) {
    Engine* engine = (Engine*)malloc(sizeof(Engine));
    if (!engine) return NULL;
    
    /* Initialize GLFW */
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        free(engine);
        return NULL;
    }
    
    /* Configure GLFW */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    /* Create window */
    GLFWmonitor* monitor = config->fullscreen ? glfwGetPrimaryMonitor() : NULL;
    engine->window = glfwCreateWindow(config->window_width, config->window_height, 
                                       config->window_title, monitor, NULL);
    if (!engine->window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        free(engine);
        return NULL;
    }
    
    glfwMakeContextCurrent(engine->window);
    
    /* Load OpenGL functions with GLAD */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        glfwDestroyWindow(engine->window);
        glfwTerminate();
        free(engine);
        return NULL;
    }
    
    /* Set up callbacks */
    g_engine = engine;
    glfwSetFramebufferSizeCallback(engine->window, framebuffer_size_callback);
    glfwSetKeyCallback(engine->window, key_callback);
    glfwSetMouseButtonCallback(engine->window, mouse_button_callback);
    glfwSetCursorPosCallback(engine->window, cursor_position_callback);
    glfwSetScrollCallback(engine->window, scroll_callback);
    
    /* Configure OpenGL */
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    /* VSync */
    glfwSwapInterval(config->vsync ? 1 : 0);
    
    /* Initialize engine state */
    engine->window_width = config->window_width;
    engine->window_height = config->window_height;
    engine->last_frame_time = glfwGetTime();
    engine->delta_time = 0.0;
    engine->mouse_captured = false;
    engine->first_mouse = true;
    
    input_init(&engine->input);
    
    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
    printf("OpenGL Renderer: %s\n", glGetString(GL_RENDERER));
    
    return engine;
}

void engine_destroy(Engine* engine) {
    if (!engine) return;
    
    g_engine = NULL;
    
    if (engine->window) {
        glfwDestroyWindow(engine->window);
    }
    glfwTerminate();
    free(engine);
}

bool engine_should_close(Engine* engine) {
    return glfwWindowShouldClose(engine->window);
}

void engine_poll_events(Engine* engine) {
    input_update(&engine->input);
    glfwPollEvents();
}

void engine_begin_frame(Engine* engine) {
    f64 current_time = glfwGetTime();
    engine->delta_time = current_time - engine->last_frame_time;
    engine->last_frame_time = current_time;
    
    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void engine_end_frame(Engine* engine) {
    glfwSwapBuffers(engine->window);
}

f32 engine_get_delta_time(Engine* engine) {
    return (f32)engine->delta_time;
}

f64 engine_get_time(Engine* engine) {
    (void)engine;
    return glfwGetTime();
}

void engine_get_window_size(Engine* engine, i32* width, i32* height) {
    if (width) *width = engine->window_width;
    if (height) *height = engine->window_height;
}

void engine_set_mouse_captured(Engine* engine, bool captured) {
    engine->mouse_captured = captured;
    engine->first_mouse = true;
    glfwSetInputMode(engine->window, GLFW_CURSOR, 
                     captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

/* Access to input state - needed by game code */
struct InputState* engine_get_input(Engine* engine) {
    return &engine->input;
}
