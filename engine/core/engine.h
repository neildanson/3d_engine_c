#ifndef ENGINE_H
#define ENGINE_H

#include "types.h"
#include "../math/vec3.h"
#include "../math/mat4.h"
#include <stdbool.h>

/* Forward declarations */
typedef struct Engine Engine;
typedef struct EngineConfig EngineConfig;

/* Engine configuration */
struct EngineConfig {
    const char* window_title;
    i32 window_width;
    i32 window_height;
    bool fullscreen;
    bool vsync;
};

/* Engine initialization and shutdown */
Engine* engine_create(const EngineConfig* config);
void engine_destroy(Engine* engine);

/* Engine main loop */
bool engine_should_close(Engine* engine);
void engine_poll_events(Engine* engine);
void engine_begin_frame(Engine* engine);
void engine_end_frame(Engine* engine);
f32 engine_get_delta_time(Engine* engine);
f64 engine_get_time(Engine* engine);

/* Window management */
void engine_get_window_size(Engine* engine, i32* width, i32* height);
void engine_set_mouse_captured(Engine* engine, bool captured);

/* Default configuration */
static inline EngineConfig engine_default_config(void) {
    return (EngineConfig){
        .window_title = "3D Engine",
        .window_width = 1280,
        .window_height = 720,
        .fullscreen = false,
        .vsync = true
    };
}

/* Access input state - declared in input/input.h */
struct InputState* engine_get_input(Engine* engine);

#endif /* ENGINE_H */
