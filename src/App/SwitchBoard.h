#ifndef SWITCH_BOARD_H
#define SWITCH_BOARD_H

#include "Sys.h"

void SwitchBoardDataSend(uint8_t cmd, uint8_t *data, uint16_t len);
void SwitchBoardSendKeyValue(uint8_t id, uint8_t key);
void SwitchBoardInit(void);
void SwitchBoardPoll(void);
#endif

