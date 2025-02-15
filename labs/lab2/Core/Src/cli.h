#ifndef CLI_H
#define CLI_H

/**
 * FILENAME: cli.h
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
 * as well as textual responses inside the CLI.  One useful function is LED
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

/**
 * This sends the initial help and command prompt then listens for input
 * interrupt.  It puts the controller to sleep until interrupt
 */
void enel452sh(void);

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
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

/**
 * This functions takes in the command word and length and matches it with the
 * function to execute.
 */
void execute(uint8_t char_count, uint8_t command[]);

/**
 * This prints the command prompt head out to UART.
 */
void prompt(void);

/**
 * This toggles the onboard green LED on or off and then prints that the
 * operation is complete to UART and runs the query function to show what
 * the status is now.
 */
void toggle_led(void);

/**
 * This reads the status of the onboard green LED and prints out what the
 * status is to UART.
 */
void query_led(void);

/**
 * This prints the help usage instructions to UART.
 */
void help(void);

/**
 * Top secret command for authorized personnel only.
 * I am working on another one to teach you Kung Fu.
 */
void neo(void);


#endif
