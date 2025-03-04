/**
 * FILENAME: cli.c
 *
 * DESCRIPTION:
 * The objective here is to set up a Command-Line Interface (CLI) through
 * which you can communicate with the STM32F103RB.  Such a tool is
 * extremely useful for unit testing code, and for automating scripted unit
 * tests.
 *
 * Onboard USART is enabled and establishes a simple interrupt and blocking
 * serial communication with a terminal program (e.g. Putty, or TeraTerm)
 * running on the host machine. I prefer Screen terminal, personally.
 *
 * There are commands and responses such that you can type the commands at
 * your CLI prompt, and expect to see various behaviours on the target board
 * as well as textualtim responses inside the CLI.  One useful function is LED
 * control.
 *
 * At each step of the development, I am paying attention to good
 * software design principles.
 *
 * One design choice I made with multitasking in mind is only transmitting
 * one character at a time to UART instead of full strings.
 *
 * I also added a top secret command for authorized personnel only.
 *
 * AUTHOR: Matthew Ross 200265265
 */

#include "main.h"
#include "cli.h"
#include "rcc.h"
#include "trig.h"
#include "string.h"
#include "stdlib.h"
#include "timer.h"
extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;
#define char_max 10 // Max command length
uint8_t input; // Temporary input character record
uint64_t uptime = 0; // Counter for system uptime
uint8_t char_count = 0; // Counter for place and length of string
uint8_t command[char_max]; // The full command string with max length

/**
 * This sends the initial status windows, help and command prompt then listens
 * for input interrupt.  It puts the controller to sleep until interrupt.
 */
void enel452sh() {
	const uint8_t UART_timeout = 100;
	const uint8_t status1[] = "+----------------------------------------+\n\r";
	const uint8_t status2[] = "| On-board LED Status :                  |\n\r";
	const uint8_t status3[] = "| Uptime  (seconds)   :                  |\n\r";
	const uint8_t status4[] = "+----------------------------------------+\n\r\n";
	const uint8_t scroll[] = "\x1b[5;r\x1b[5;0H"; // Scroll keep top 4 lines
	for (int i = 0; i < sizeof(status1); i++) { // status1 msg
		HAL_UART_Transmit(&huart2, status1 + i, 1, UART_timeout);
	}
	for (int i = 0; i < sizeof(status2); i++) { // status1 msg
		HAL_UART_Transmit(&huart2, status2 + i, 1, UART_timeout);
	}
	for (int i = 0; i < sizeof(status3); i++) { // status1 msg
		HAL_UART_Transmit(&huart2, status3 + i, 1, UART_timeout);
	}
	for (int i = 0; i < sizeof(status4); i++) { // status1 msg
		HAL_UART_Transmit(&huart2, status4 + i, 1, UART_timeout);
	}
	for (int i = 0; i < sizeof(scroll); i++) { // status1 msg
		HAL_UART_Transmit(&huart2, scroll + i, 1, UART_timeout);
	}
	help(); // Initial usage help instructions
	prompt(); // Initial prompt
	int32_t seed = __HAL_TIM_GET_COUNTER(&htim2); // Make random seed from timer
	srand(seed); // Seed random number generator
	HAL_UART_Receive_IT(&huart2, &input, 1); // Listen for the interrupt
	while (1) { // Primary loop that takes advantage of sleep with interrupts
		HAL_SuspendTick(); // Suspend systicks for going to sleep
		HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI); // Sleep
	}
}

/**
 * This is a 1 second interrupt timer that will update the LED status and the
 * time counter.
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	const uint8_t UART_timeout = 100;
	const uint8_t on[] = "ON "; // Status for LED on
	const uint8_t off[] = "OFF"; // Status for LED off
	const uint8_t hide_cursor[] = "\033[?25l";
	const uint8_t restore_cursor[] = "\033[?25h";
	const uint8_t cursor_save[] = "\x1b[s"; // Save the current cursor position
	const uint8_t cursor_load[] = "\x1b[u"; // Load the saved cursor position
	const uint8_t led_pos[] = "\x1b[2;25H"; // Position cursor for LED status
	const uint8_t uptime_pos[] = "\x1b[3;25H"; // Position cursor for uptime
	uptime++; // Increment the time counter
	uint8_t uptime_str[15] = ""; // Initialize uptime string
	itoa(uptime, (char*) uptime_str, 10); // Convert int to string
	HAL_UART_Transmit(&huart2, cursor_save, sizeof(cursor_save), UART_timeout);
	HAL_UART_Transmit(&huart2, hide_cursor, sizeof(hide_cursor), UART_timeout);
	HAL_UART_Transmit(&huart2, led_pos, sizeof(led_pos), UART_timeout);
	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == 1)
		HAL_UART_Transmit(&huart2, on, sizeof(on), UART_timeout);
	else
		HAL_UART_Transmit(&huart2, off, sizeof(off), UART_timeout);
	HAL_UART_Transmit(&huart2, uptime_pos, sizeof(uptime_pos), UART_timeout);
	HAL_UART_Transmit(&huart2, uptime_str, sizeof(uptime_str), UART_timeout);
	HAL_UART_Transmit(&huart2, cursor_load, sizeof(cursor_load), UART_timeout);
	HAL_UART_Transmit(&huart2, restore_cursor, sizeof(restore_cursor),
			UART_timeout);
}

/**
 * This is the main Command Line Interface (CLI) interrupt. It watches for
 * character input.  Characters and the count are stored.  Enter will execute
 * the command and zero the character count to start again. Backspace can be
 * used to correct typing mistakes for commands and stop when empty. The max
 * character input for commands is capped and it stops at the cap. All the
 * characters are echoed back as you type. It has the conventional
 * functionality of most CLIs, I didn't do anything really weird. Anyone
 * can probably use this intuitively.
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) { // Check for input
	const uint8_t UART_timeout = 100;
	const uint8_t BEL_endl[] = "\07\n\r"; // Bell, endline, return
	const uint8_t BS = '\177'; // Backspace key
	const uint8_t backspace[] = "\b \b"; // Backspace command to terminal
	const uint8_t CR = '\r'; // Carriage return or enter key
	if (input == BS) { // Check for backspace
		if (char_count > 0) {
			HAL_UART_Transmit(&huart2, backspace, sizeof(backspace),
					UART_timeout);
			char_count--; // Deleted character
		}
	} else if (input == CR) { // Check for enter
		HAL_UART_Transmit(&huart2, BEL_endl, sizeof(BEL_endl), UART_timeout);
		execute(char_count, command); // Execute the command
		char_count = 0; // Start the command string over
		prompt();
	} else if (char_count == char_max) { // Max characters stop input
	} else { // Echo input character
		HAL_UART_Transmit(&huart2, &input, 1, UART_timeout);
		command[char_count] = input; // Write the char to the command string
		char_count++; // Added character
	}
	HAL_UART_Receive_IT(&huart2, &input, 1); // Reset the interrupt
}

/**
 * This functions takes in the command word and length and matches it with the
 * function to execute.
 */
void execute(uint8_t char_count, uint8_t command[]) { // Execute command
	const uint8_t neo_cmd[] = "neo"; // Top secret command
	const uint8_t toggle_cmd[] = "toggle"; // toggle command
	const uint8_t query_cmd[] = "query"; // query command
	const uint8_t help_cmd[] = "help"; // help command
	const uint8_t timetest_cmd[] = "timetest"; // time test suite command
	const uint8_t rcc_cmd[] = "rcc"; // rcc command
	const uint8_t trigtest_cmd[] = "trigtest"; // trig test suite command
	if (strncmp((const char*) command, (const char*) neo_cmd, char_count) == 0
			&& char_count == sizeof(neo_cmd) - 1)
		neo();
	else if (strncmp((const char*) command, (const char*) toggle_cmd,
			char_count) == 0 && char_count == sizeof(toggle_cmd) - 1)
		toggle_led();
	else if (strncmp((const char*) command, (const char*) query_cmd, char_count)
			== 0 && char_count == sizeof(query_cmd) - 1)
		query_led();
	else if (strncmp((const char*) command, (const char*) help_cmd, char_count)
			== 0 && char_count == sizeof(help_cmd) - 1)
		help();
	else if (strncmp((const char*) command, (const char*) timetest_cmd,
			char_count) == 0 && char_count == sizeof(timetest_cmd) - 1)
		run_test();
	else if (strncmp((const char*) command, (const char*) rcc_cmd, char_count)
			== 0 && char_count == sizeof(rcc_cmd) - 1)
		rcc_status();
	else if (strncmp((const char*) command, (const char*) trigtest_cmd,
			char_count) == 0 && char_count == sizeof(trigtest_cmd) - 1)
		run_trig_test();
	else if (char_count > 0)
		help(); // If not an empty prompt then print help
}

/**
 * This prints the command prompt head out to UART.
 */
void prompt(void) { // Command prompt
	const uint8_t UART_timeout = 100;
	const uint8_t prompt_txt[] = "ense481: ";
	for (int i = 0; i < sizeof(prompt_txt); i++) { // prompt msg
		HAL_UART_Transmit(&huart2, prompt_txt + i, 1, UART_timeout);
	}
}

/**
 * This toggles the onboard green LED on or off and then prints that the
 * operation is complete to UART and runs the query function to show what
 * the status is now.
 */
void toggle_led(void) { // Turn off or on the LED
	const uint8_t UART_timeout = 100;
	const uint8_t on[] = "ON "; // Status for LED on
	const uint8_t off[] = "OFF"; // Status for LED off
	const uint8_t cursor_save[] = "\x1b[s"; // Save the current cursor position
	const uint8_t cursor_load[] = "\x1b[u"; // Load the saved cursor position
	const uint8_t led_pos[] = "\x1b[2;25H"; // Position cursor for LED status
	const uint8_t toggle_txt[] = "  You have toggled the LED.\n\r";
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5); // Toggle onboard green LED
	for (int i = 0; i < sizeof(toggle_txt); i++) // toggle msg
		HAL_UART_Transmit(&huart2, toggle_txt + i, 1, UART_timeout);
	query_led();
	HAL_UART_Transmit(&huart2, cursor_save, sizeof(cursor_save), UART_timeout);
	HAL_UART_Transmit(&huart2, led_pos, sizeof(led_pos), UART_timeout);
	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == 1)
		HAL_UART_Transmit(&huart2, on, sizeof(on), UART_timeout);
	else
		HAL_UART_Transmit(&huart2, off, sizeof(off), UART_timeout);
	HAL_UART_Transmit(&huart2, cursor_load, sizeof(cursor_load), UART_timeout);
}

/**
 * This reads the status of the onboard green LED and prints out what the
 * status is to UART.
 */
void query_led(void) { // Query the state of the LED
	const uint8_t UART_timeout = 100;
	const uint8_t on_txt[] = "  The LED is now ON!\n\r";
	const uint8_t off_txt[] = "  The LED is now OFF!\n\r";
	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == 1)
		for (int i = 0; i < sizeof(on_txt); i++) // on msg
			HAL_UART_Transmit(&huart2, on_txt + i, 1, UART_timeout);
	else
		for (int i = 0; i < sizeof(off_txt); i++) { // off msg
			HAL_UART_Transmit(&huart2, off_txt + i, 1, UART_timeout);
		}
}

/**
 * This prints the help usage instructions to UART.
 */
void help(void) { // Usage help instructions
	const uint8_t UART_timeout = 100;
	const uint8_t help1[] = "  The case-sensitive usage commands are:\n\r";
	const uint8_t help2[] = "    toggle   - toggles the onboard LED\n\r";
	const uint8_t help3[] =
			"    query    - queries the status of the onboard LED\n\r";
	const uint8_t help4[] = "    timetest - prints common operation times\n\r";
	const uint8_t help5[] = "    trigtest - prints trig operation times\n\r";
	const uint8_t help6[] = "    rcc      - prints RCC config\n\r";
	const uint8_t help7[] = "    help     - prints usage commands\n\r";
	for (int i = 0; i < sizeof(help1); i++) { // help1 msg
		HAL_UART_Transmit(&huart2, help1 + i, 1, UART_timeout);
	}
	for (int i = 0; i < sizeof(help2); i++) { // help2 msg
		HAL_UART_Transmit(&huart2, help2 + i, 1, UART_timeout);
	}
	for (int i = 0; i < sizeof(help3); i++) { // help3 msg
		HAL_UART_Transmit(&huart2, help3 + i, 1, UART_timeout);
	}
	for (int i = 0; i < sizeof(help4); i++) { // help4 msg
		HAL_UART_Transmit(&huart2, help4 + i, 1, UART_timeout);
	}
	for (int i = 0; i < sizeof(help5); i++) { // help5 msg
		HAL_UART_Transmit(&huart2, help5 + i, 1, UART_timeout);
	}
	for (int i = 0; i < sizeof(help6); i++) { // help6 msg
		HAL_UART_Transmit(&huart2, help6 + i, 1, UART_timeout);
	}
	for (int i = 0; i < sizeof(help7); i++) { // help7 msg
		HAL_UART_Transmit(&huart2, help7 + i, 1, UART_timeout);
	}
}

/**
 * Top secret command for authorized personnel only.
 * I am working on another one to teach you Kung Fu.
 */
void neo() { // Top secret command
	const uint8_t UART_timeout = 100;
	const uint8_t BEL[] = "\07"; // Bell key
	const uint8_t cls[] = "\033[2J\033[H"; // Cls, home
	const uint8_t hide_cursor[] = "\033[?25l";
	const uint8_t restore_cursor[] = "\033[?25h";
	const uint8_t matrix1[] = "Wake up, Neo...";
	const uint8_t matrix2[] = "The Matrix has you...";
	const uint8_t matrix3[] = "Follow the white rabbit.";
	const uint8_t matrix4[] = "Knock, knock, Neo.";
	const int slow_delay = 400; // 400ms for slow typing
	const int fast_delay = 100; // 100ms for fast typing
	const int long_delay = 3000; // 3s wait
	// Look at me, I'm the captain now.
	HAL_TIM_Base_Stop_IT(&htim4); // This disables the status update every second
	HAL_ResumeTick(); // Turn on systicks for typing delays
	HAL_UART_Transmit(&huart2, hide_cursor, sizeof(hide_cursor), UART_timeout);
	HAL_UART_Transmit(&huart2, cls, sizeof(cls), UART_timeout);
	for (int i = 0; i < sizeof(matrix1); i++) { // matrix1 msg
		HAL_Delay(fast_delay);
		HAL_UART_Transmit(&huart2, matrix1 + i, 1, UART_timeout);
	}
	HAL_UART_Transmit(&huart2, BEL, 1, UART_timeout);
	HAL_Delay(long_delay);
	HAL_UART_Transmit(&huart2, cls, sizeof(cls), UART_timeout);
	for (int i = 0; i < sizeof(matrix2); i++) { // matrix2 msg
		HAL_Delay(slow_delay);
		HAL_UART_Transmit(&huart2, matrix2 + i, 1, UART_timeout);
	}
	HAL_UART_Transmit(&huart2, BEL, 1, UART_timeout);
	HAL_Delay(long_delay);
	HAL_UART_Transmit(&huart2, cls, sizeof(cls), UART_timeout);
	for (int i = 0; i < sizeof(matrix3); i++) { // matrix3 msg
		HAL_Delay(fast_delay);
		HAL_UART_Transmit(&huart2, matrix3 + i, 1, UART_timeout);
	}
	HAL_UART_Transmit(&huart2, BEL, 1, UART_timeout);
	HAL_Delay(long_delay);
	HAL_UART_Transmit(&huart2, cls, sizeof(cls), UART_timeout);
	for (int i = 0; i < sizeof(matrix4); i++) { // matrix4 msg
		HAL_UART_Transmit(&huart2, matrix4 + i, 1, UART_timeout);
	}
	HAL_UART_Transmit(&huart2, BEL, 1, UART_timeout);
	HAL_Delay(long_delay);

	HAL_UART_Transmit(&huart2, cls, sizeof(cls), UART_timeout);
	HAL_UART_Transmit(&huart2, restore_cursor, sizeof(restore_cursor),
			UART_timeout);
	HAL_TIM_Base_Start_IT(&htim4); // Enable status updates again
	// Print the intro again
	const uint8_t status1[] = "+----------------------------------------+\n\r";
	const uint8_t status2[] = "| On-board LED Status :                  |\n\r";
	const uint8_t status3[] = "| Uptime  (seconds)   :                  |\n\r";
	const uint8_t status4[] = "+----------------------------------------+\n\r\n";
	const uint8_t scroll[] = "\x1b[5;r\x1b[5;0H"; // Scroll keep top 4 lines
	for (int i = 0; i < sizeof(status1); i++) { // status1 msg
		HAL_UART_Transmit(&huart2, status1 + i, 1, UART_timeout);
	}
	for (int i = 0; i < sizeof(status2); i++) { // status1 msg
		HAL_UART_Transmit(&huart2, status2 + i, 1, UART_timeout);
	}
	for (int i = 0; i < sizeof(status3); i++) { // status1 msg
		HAL_UART_Transmit(&huart2, status3 + i, 1, UART_timeout);
	}
	for (int i = 0; i < sizeof(status4); i++) { // status1 msg
		HAL_UART_Transmit(&huart2, status4 + i, 1, UART_timeout);
	}
	for (int i = 0; i < sizeof(scroll); i++) { // status1 msg
		HAL_UART_Transmit(&huart2, scroll + i, 1, UART_timeout);
	}
	help(); // Initial usage help instructions
}
