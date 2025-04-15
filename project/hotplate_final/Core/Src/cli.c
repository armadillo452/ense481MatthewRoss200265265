#include "cli.h"
#include "adc.h"	    // heartbeat temp and volt on adc
#include "stdarg.h"     // va_list, va_start, va_end, vsnprintf
#include "stdio.h"      // vsnprintf (safe string formatting)
#include "string.h"     // strlen
#include "cmsis_os2.h"  // myprintf queue put
#include "MAX6675.h"    // thermocouple
#include "epaper_ctl.h" // epaper
#include "epaper.h"     // epaper
#include "git_version.h"// git commit hash

extern osMessageQueueId_t UARToutHandle;
extern osMessageQueueId_t EpaperStatusHandle;
extern osMessageQueueId_t EpaperStatus2Handle;
extern osMessageQueueId_t EpaperStatus3Handle;
extern osMessageQueueId_t EpaperStatus4Handle;
extern TIM_HandleTypeDef htim3;
extern uint8_t image_bw[EPD_W_BUFF_SIZE * EPD_H];
extern uint8_t target; // Hotplate target temperature
extern uint8_t button1_held;
extern uint8_t button4_held;
extern uint32_t button1_press_time;
extern uint32_t button4_press_time;

#define charmax 10 // Max length of command
uint8_t char_count = 0; // Counter for how many chars in input
uint8_t command[charmax]; // The full command string with max length

void myprintf(const char *fmt, ...) {
	char buffer[128];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);
	osMessageQueuePut(UARToutHandle, buffer, 0, 0);
}

// UART2 console keyboard
void console_UART(uint8_t input_console) {
	if (input_console == '\177') { // Check for backspace
		if (char_count > 0) {
			myprintf("\b \b"); // Backspace
			char_count--; // Deleted character
		}
	} else if (input_console == '\r') { // Check for enter
		myprintf("\r\n");
		execute(char_count, command); // Execute the command
		char_count = 0; // Start the command string over
		myprintf(">> ");
	} else if (char_count == charmax) { // Max characters stop input
	} else { // Echo input character
		myprintf("%c", input_console);
		command[char_count] = input_console; // Write the char to the command string
		char_count++; // Added character
	}
}

// Execute commands
void execute(uint8_t char_count, uint8_t command[]) { // Execute command
	if (strncmp((const char*) command, "help", char_count) == 0
			&& char_count == sizeof("help") - 1)
		help();
	else if (strncmp((const char*) command, "cls", char_count) == 0
			&& char_count == sizeof("cls") - 1)
		status();
	else if (strncmp((const char*) command, "clse", char_count) == 0
			&& char_count == sizeof("clse") - 1)
		epaper_clear_refresh();
	else if (strncmp((const char*) command, "gitver", char_count) == 0
				&& char_count == sizeof("gitver") - 1)
		myprintf("Git Version: %s\n\r", GIT_VERSION);
	else if (char_count > 0)
		myprintf("  Invalid command\n\r");
}

// Print heartbeat and update counters
void heartbeat(uint32_t uptime) { // Print the heartbeat data
	uint32_t uptime_divided = uptime / 2;
	char displayBuffer[64]; // Buffer for formatted output to epaper
	sprintf(displayBuffer, "VDD: %.3fV  Chip: %.2fC   Uptime: %lus", getVolt(),
			getTemp(), uptime_divided);
	osMessageQueuePut(EpaperStatusHandle, displayBuffer, 0, 0);
	sprintf(displayBuffer, "Target: %d       Current: %.2fC       ", target,
			getMAX6675());
	osMessageQueuePut(EpaperStatus2Handle, displayBuffer, 0, 0);
	myprintf("\x1b[s\033[?25l\x1b[4;29H"); // Save position, hide, jump
	myprintf("%lus", uptime_divided); // Print the uptime
	myprintf("\x1b[3;29H"); // Jump to plate temp
	myprintf("%.2f°C   ", getMAX6675()); // Print plate temp
	myprintf("\x1b[3;10H"); // Jump to VDD
	myprintf("%.3fV", getVolt()); // Print the VDD in V
	myprintf("\x1b[4;10H"); // Jump to Temp
	myprintf("%.2f°C", getTemp()); // Print the Temp in C
	myprintf("\x1b[5;10H"); // Jump to Heat
	if (target > getMAX6675()) myprintf("ON ", getTemp()); // Print the heater status
	else myprintf("OFF", getTemp()); // Print the heater status
	myprintf("\x1b[u\033[?25h"); // Load position, show

	// If button is being held for at least two second
	if (button1_held && (HAL_GetTick() - button1_press_time) >= 2000) {
		if (target <= 245)
		target += 10; // Increase by 10 if still held
		else target = 255;
		button1_press_time += 2000;
	}
	if (button4_held && (HAL_GetTick() - button4_press_time) >= 2000) {
		if (target >= 10)
		target -= 10; // Decrease by 10 if still held
		else target = 0;
		button4_press_time += 2000;
	}

	// Hotplate control
	if (target > getMAX6675()) {
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3,
				(htim3.Init.Period * 1) /  30); // 3.25% duty
		HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
	} else {
		HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);
	}
}

void status() {
	myprintf("\033[2J\033[H"); // Clear screen and home cursor
	myprintf("\033[?25h"); // Make sure cursor not hidden
	myprintf("  Hotplate Console - Type 'help' for commands\n\r");
	myprintf("+--------------------------------------------+\n\r");
	myprintf("| VDD  :          Plate   :                  |\n\r");
	myprintf("| Chip :          Uptime  :                  |\n\r");
	myprintf("| Heat :                                     |\n\r");
	myprintf("+--------------------------------------------+\n\r");
	myprintf("\x1b[7;r\x1b[7;0H"); // Scroll keep top 5 lines")
	myprintf(">> ");
}

// Print help
void help(void) { // Usage help instructions
	myprintf("  The case-sensitive usage commands are:\n\r");
	myprintf("    cls       - clear screen\n\r");
	myprintf("    clse      - clear epaper\n\r");
	myprintf("    gitver    - print git commit hash\n\r");
}
