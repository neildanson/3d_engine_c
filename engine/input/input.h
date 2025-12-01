#ifndef INPUT_H
#define INPUT_H

#include "../core/types.h"
#include <stdbool.h>

/* Key codes - matching GLFW key codes */
typedef enum {
    KEY_SPACE = 32,
    KEY_APOSTROPHE = 39,
    KEY_COMMA = 44,
    KEY_MINUS = 45,
    KEY_PERIOD = 46,
    KEY_SLASH = 47,
    KEY_0 = 48,
    KEY_1 = 49,
    KEY_2 = 50,
    KEY_3 = 51,
    KEY_4 = 52,
    KEY_5 = 53,
    KEY_6 = 54,
    KEY_7 = 55,
    KEY_8 = 56,
    KEY_9 = 57,
    KEY_SEMICOLON = 59,
    KEY_EQUAL = 61,
    KEY_A = 65,
    KEY_B = 66,
    KEY_C = 67,
    KEY_D = 68,
    KEY_E = 69,
    KEY_F = 70,
    KEY_G = 71,
    KEY_H = 72,
    KEY_I = 73,
    KEY_J = 74,
    KEY_K = 75,
    KEY_L = 76,
    KEY_M = 77,
    KEY_N = 78,
    KEY_O = 79,
    KEY_P = 80,
    KEY_Q = 81,
    KEY_R = 82,
    KEY_S = 83,
    KEY_T = 84,
    KEY_U = 85,
    KEY_V = 86,
    KEY_W = 87,
    KEY_X = 88,
    KEY_Y = 89,
    KEY_Z = 90,
    KEY_ESCAPE = 256,
    KEY_ENTER = 257,
    KEY_TAB = 258,
    KEY_BACKSPACE = 259,
    KEY_INSERT = 260,
    KEY_DELETE = 261,
    KEY_RIGHT = 262,
    KEY_LEFT = 263,
    KEY_DOWN = 264,
    KEY_UP = 265,
    KEY_PAGE_UP = 266,
    KEY_PAGE_DOWN = 267,
    KEY_HOME = 268,
    KEY_END = 269,
    KEY_CAPS_LOCK = 280,
    KEY_SCROLL_LOCK = 281,
    KEY_NUM_LOCK = 282,
    KEY_PRINT_SCREEN = 283,
    KEY_PAUSE = 284,
    KEY_F1 = 290,
    KEY_F2 = 291,
    KEY_F3 = 292,
    KEY_F4 = 293,
    KEY_F5 = 294,
    KEY_F6 = 295,
    KEY_F7 = 296,
    KEY_F8 = 297,
    KEY_F9 = 298,
    KEY_F10 = 299,
    KEY_F11 = 300,
    KEY_F12 = 301,
    KEY_LEFT_SHIFT = 340,
    KEY_LEFT_CONTROL = 341,
    KEY_LEFT_ALT = 342,
    KEY_LEFT_SUPER = 343,
    KEY_RIGHT_SHIFT = 344,
    KEY_RIGHT_CONTROL = 345,
    KEY_RIGHT_ALT = 346,
    KEY_RIGHT_SUPER = 347,
    KEY_MAX = 512
} KeyCode;

/* Mouse button codes */
typedef enum {
    MOUSE_BUTTON_LEFT = 0,
    MOUSE_BUTTON_RIGHT = 1,
    MOUSE_BUTTON_MIDDLE = 2,
    MOUSE_BUTTON_MAX = 8
} MouseButton;

/* Input state structure */
typedef struct InputState {
    bool keys[KEY_MAX];
    bool keys_pressed[KEY_MAX];
    bool keys_released[KEY_MAX];
    bool mouse_buttons[MOUSE_BUTTON_MAX];
    bool mouse_buttons_pressed[MOUSE_BUTTON_MAX];
    bool mouse_buttons_released[MOUSE_BUTTON_MAX];
    f64 mouse_x;
    f64 mouse_y;
    f64 mouse_delta_x;
    f64 mouse_delta_y;
    f64 scroll_x;
    f64 scroll_y;
} InputState;

/* Input functions */
void input_init(InputState* state);
void input_update(InputState* state);
void input_process_key(InputState* state, i32 key, i32 action);
void input_process_mouse_button(InputState* state, i32 button, i32 action);
void input_process_mouse_move(InputState* state, f64 x, f64 y);
void input_process_scroll(InputState* state, f64 x, f64 y);

bool input_key_down(const InputState* state, KeyCode key);
bool input_key_pressed(const InputState* state, KeyCode key);
bool input_key_released(const InputState* state, KeyCode key);

bool input_mouse_down(const InputState* state, MouseButton button);
bool input_mouse_pressed(const InputState* state, MouseButton button);
bool input_mouse_released(const InputState* state, MouseButton button);

void input_get_mouse_position(const InputState* state, f64* x, f64* y);
void input_get_mouse_delta(const InputState* state, f64* dx, f64* dy);

#endif /* INPUT_H */
