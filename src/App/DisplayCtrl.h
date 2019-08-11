#ifndef DISPLAY_CTRL_H
#define DISPLAY_CTRL_H

#include "Sys.h"

void DisplayCtrlDataSend(uint8_t cmd, uint8_t *data, uint16_t len);
void DisplayCtrlInit(void);
void DisplayCtrlPoll(void);


#endif

