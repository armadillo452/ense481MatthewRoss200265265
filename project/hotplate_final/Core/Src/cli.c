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
#include "neuron.h"     // heatDuty

extern osMessageQueueId_t UARToutHandle;
extern osMessageQueueId_t EpaperStatusHandle;
extern osMessageQueueId_t EpaperStatus2Handle;
extern osMessageQueueId_t EpaperStatus3Handle;
extern TIM_HandleTypeDef htim3;
extern uint8_t image_bw[EPD_W_BUFF_SIZE * EPD_H];
extern uint8_t target; // Hotplate target temperature
extern IWDG_HandleTypeDef hiwdg;
extern osSemaphoreId_t consoleHandle;
extern uint8_t clearPaper;
extern uint8_t uptime;
extern uint8_t minutes;
extern uint8_t hours;
extern uint16_t days;

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
  if (osSemaphoreAcquire(consoleHandle, osWaitForever) == osOK) {
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
    osSemaphoreRelease(consoleHandle);
  }
}

// Execute commands
void execute(uint8_t char_count, uint8_t command[]) { // Execute command
  if (strncmp((const char*) command, "help", char_count) == 0 && char_count == sizeof("help") - 1)
    help();
  else if (strncmp((const char*) command, "cls", char_count) == 0
      && char_count == sizeof("cls") - 1)
    status();
  else if (strncmp((const char*) command, "clse", char_count) == 0
      && char_count == sizeof("clse") - 1)
    clearPaper = 1;
  else if (strncmp((const char*) command, "gitver", char_count) == 0
      && char_count == sizeof("gitver") - 1)
    myprintf("Git Version: %s\n\r", GIT_VERSION);
  else if (strncmp((const char*) command, "temp", 4) == 0) {
    command[char_count] = '\0'; // Null terminate the command for sscanf
    sscanf((char*) command, "temp %hhu", &target); // Strip out and set the target value
  } else if (char_count > 0)
    myprintf("  Invalid command\n\r");
}

// Print heartbeat and update counters
void heartbeat() { // Print the heartbeat data
  uint8_t uptime_divided = uptime / 2;
  double calcDuty = heatDuty();
  uint8_t cycles = calcDuty * 30 / 100; // 3.25% per cycle
  float duty = cycles * 3.25;
  char displayBuffer[64]; // Buffer for formatted output to epaper
  char secondsBuffer[5];
  char minutesBuffer[5];
  char hoursBuffer[5];
  char daysBuffer[7];
  HAL_IWDG_Refresh(&hiwdg); // Kick the watchdog
  sprintf(displayBuffer, "VDD: %.3fV Chip: %.2fC  Uptime: ", getVolt(), getTemp());
  sprintf(daysBuffer, "%ud", days);
  strcat(displayBuffer, daysBuffer);
  sprintf(hoursBuffer, "%02uh", hours);
  strcat(displayBuffer, hoursBuffer);
  sprintf(minutesBuffer, "%02um", minutes);
  strcat(displayBuffer, minutesBuffer);
  sprintf(secondsBuffer, "%02us", uptime_divided);
  strcat(displayBuffer, secondsBuffer);
  osMessageQueuePut(EpaperStatusHandle, displayBuffer, 0, 0);
  sprintf(displayBuffer, "Set : %dC  Now: %.2fC       ", target, getMAX6675());
  osMessageQueuePut(EpaperStatus2Handle, displayBuffer, 0, 0);
  sprintf(displayBuffer, "Duty: %3.2f%%  ", duty);
  osMessageQueuePut(EpaperStatus3Handle, displayBuffer, 0, 0);

  if (osSemaphoreAcquire(consoleHandle, osWaitForever) == osOK) {
    myprintf("\x1b[s\033[?25l\x1b[3;45H"); // Save position, hide, jump
    myprintf("%ud", days); // Print the uptime
    myprintf("%02uh", hours); // Print the uptime
    myprintf("%02um", minutes); // Print the uptime
    myprintf("%02us", uptime_divided); // Print the uptime
    myprintf("\x1b[4;10H"); // Jump to Set
    myprintf("%d°C", target); // Print the Set in C
    myprintf("\x1b[4;26H"); // Jump to Now
    myprintf("%.2f°C   ", getMAX6675()); // Print plate temp
    myprintf("\x1b[3;10H"); // Jump to VDD
    myprintf("%.3fV", getVolt()); // Print the VDD in V
    myprintf("\x1b[3;26H"); // Jump to Chip
    myprintf("%.2f°C", getTemp()); // Print the Chip in C
    myprintf("\x1b[5;10H"); // Jump to Heat
    myprintf("%3.2f%%  ", duty); // Print the Duty in %
    myprintf("\x1b[u\033[?25h"); // Load position, show
    osSemaphoreRelease(consoleHandle);
  }

  // Hotplate control
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, (htim3.Init.Period * cycles) / 30);
}

void status() {
  myprintf("\033[2J\033[H"); // Clear screen and home cursor
  myprintf("\033[?25h"); // Make sure cursor not hidden
  myprintf("  Hotplate Console - Type 'help' for commands\n\r");
  myprintf("+-----------------------------------------------------------+\n\r");
  myprintf("| VDD  :          Chip :           Uptime :                 |\n\r");
  myprintf("| Set  :          Now  :                                    |\n\r");
  myprintf("| Duty :                                                    |\n\r");
  myprintf("+-----------------------------------------------------------+\n\r");
  myprintf("\x1b[7;r\x1b[7;0H"); // Scroll keep top 6 lines")
}

// Print help
void help(void) { // Usage help instructions
  myprintf("  The case-sensitive usage commands are:\n\r");
  myprintf("    cls       - clear screen\n\r");
  myprintf("    clse      - clear epaper\n\r");
  myprintf("    gitver    - print git commit hash\n\r");
  myprintf("    temp ###  - set the target temperature\n\r");
}
