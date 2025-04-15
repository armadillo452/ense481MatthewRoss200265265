#ifndef CLI_H
#define CLI_H

#include "stdint.h" // uint_t types

void myprintf(const char *fmt, ...);
void console_UART(uint8_t input_console);
void execute(uint8_t char_count, uint8_t command[]);
void heartbeat(uint32_t uptime);
void status(void);
void help(void);

#endif
