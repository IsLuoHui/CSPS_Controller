#ifndef BASIS_LOOPBUFFER_H
#define BASIS_LOOPBUFFER_H

#include "stdint.h"

uint8_t Command_Write(uint8_t *data, uint8_t length);

uint8_t Command_GetCommand(uint8_t *command);

#endif //BASIS_LOOPBUFFER_H