/*
 * at_command.h
 *
 *  Created on: 12 Nis 2026
 *      Author: PC1
 */

#ifndef AT_COMMAND_H_
#define AT_COMMAND_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

typedef enum {
} atCommandErrorCodes_t;

typedef enum {
} AT_COMMAND_IOCTL_COMMANDS_T;

atCommandErrorCodes_t AtCommand_Open(void* vpParam);
atCommandErrorCodes_t AtCommand_Ioctl(AT_COMMAND_IOCTL_COMMANDS_T eCommand,void* vpParam);
atCommandErrorCodes_t AtCommand_Close(void* vpParam);

#ifdef __cplusplus
}
#endif

#endif /* AT_COMMAND_H_ */
