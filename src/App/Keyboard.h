#ifndef KEYBOARD_H
#define KEYBOARD_H

typedef enum
{
    KEYBOARD_KEY_ACTIVE = 0,
    KEYBOARD_KEY_UP,
    KEYBOARD_KEY_DOWN,
    KEYBOARD_KEY_LEFT,
    KEYBOARD_KEY_RIGHT,
    KEYBOARD_KEY_ENTER,
}KeyboardKey_t;

typedef void (*KeyboardKeyHandle_t)(KeyboardKey_t key);
void KeyboardInit(KeyboardKeyHandle_t handle);
void KeyboardPoll(void);
#endif

