#include "input.h"
#include <string.h>

void input_init(InputState* state) {
    memset(state, 0, sizeof(InputState));
}

void input_update(InputState* state) {
    /* Clear per-frame input states */
    memset(state->keys_pressed, 0, sizeof(state->keys_pressed));
    memset(state->keys_released, 0, sizeof(state->keys_released));
    memset(state->mouse_buttons_pressed, 0, sizeof(state->mouse_buttons_pressed));
    memset(state->mouse_buttons_released, 0, sizeof(state->mouse_buttons_released));
    state->mouse_delta_x = 0.0;
    state->mouse_delta_y = 0.0;
    state->scroll_x = 0.0;
    state->scroll_y = 0.0;
}

void input_process_key(InputState* state, i32 key, i32 action) {
    if (key < 0 || key >= KEY_MAX) return;
    
    if (action == 1) { /* GLFW_PRESS */
        if (!state->keys[key]) {
            state->keys_pressed[key] = true;
        }
        state->keys[key] = true;
    } else if (action == 0) { /* GLFW_RELEASE */
        state->keys[key] = false;
        state->keys_released[key] = true;
    }
}

void input_process_mouse_button(InputState* state, i32 button, i32 action) {
    if (button < 0 || button >= MOUSE_BUTTON_MAX) return;
    
    if (action == 1) { /* GLFW_PRESS */
        if (!state->mouse_buttons[button]) {
            state->mouse_buttons_pressed[button] = true;
        }
        state->mouse_buttons[button] = true;
    } else if (action == 0) { /* GLFW_RELEASE */
        state->mouse_buttons[button] = false;
        state->mouse_buttons_released[button] = true;
    }
}

void input_process_mouse_move(InputState* state, f64 x, f64 y) {
    state->mouse_delta_x = x - state->mouse_x;
    state->mouse_delta_y = y - state->mouse_y;
    state->mouse_x = x;
    state->mouse_y = y;
}

void input_process_scroll(InputState* state, f64 x, f64 y) {
    state->scroll_x = x;
    state->scroll_y = y;
}

bool input_key_down(const InputState* state, KeyCode key) {
    if (key < 0 || key >= KEY_MAX) return false;
    return state->keys[key];
}

bool input_key_pressed(const InputState* state, KeyCode key) {
    if (key < 0 || key >= KEY_MAX) return false;
    return state->keys_pressed[key];
}

bool input_key_released(const InputState* state, KeyCode key) {
    if (key < 0 || key >= KEY_MAX) return false;
    return state->keys_released[key];
}

bool input_mouse_down(const InputState* state, MouseButton button) {
    if (button < 0 || button >= MOUSE_BUTTON_MAX) return false;
    return state->mouse_buttons[button];
}

bool input_mouse_pressed(const InputState* state, MouseButton button) {
    if (button < 0 || button >= MOUSE_BUTTON_MAX) return false;
    return state->mouse_buttons_pressed[button];
}

bool input_mouse_released(const InputState* state, MouseButton button) {
    if (button < 0 || button >= MOUSE_BUTTON_MAX) return false;
    return state->mouse_buttons_released[button];
}

void input_get_mouse_position(const InputState* state, f64* x, f64* y) {
    if (x) *x = state->mouse_x;
    if (y) *y = state->mouse_y;
}

void input_get_mouse_delta(const InputState* state, f64* dx, f64* dy) {
    if (dx) *dx = state->mouse_delta_x;
    if (dy) *dy = state->mouse_delta_y;
}
