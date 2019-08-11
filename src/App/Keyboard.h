#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "Sys.h"

typedef enum
{
    KEYBOARD_KEY_DOUBLE_CTRL = 1,
    KEYBOARD_KEY_UP,
    KEYBOARD_KEY_DOWN,
    KEYBOARD_KEY_LEFT,
    KEYBOARD_KEY_RIGHT,
    KEYBOARD_KEY_ENTER,
}KeyboardKey_t;

typedef enum
{
    KEYBOARD_ID_KVM1 = 0,
    KEYBOARD_ID_KVM2,
}KeyboardID_t;

typedef struct
{
    bool ctrlPressed;
    uint8_t buffcount;
    uint8_t buff[64];
    KeyboardID_t id;
    SysTime_t lastTime;
    SysTime_t lastCtrlTime;
}Keyboard_t;

typedef void (*KeyboardKeyHandle_t)(KeyboardID_t id, KeyboardKey_t key);
void KeyboardInit(KeyboardKeyHandle_t handle);
void KeyboardPoll(void);
#endif

