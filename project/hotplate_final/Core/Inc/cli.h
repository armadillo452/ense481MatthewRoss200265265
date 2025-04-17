/**
 * @file cli.h
 * @brief Contains declarations for CLI-related functions.
 *
 * @author Matthew Ross 200265265
 *
 * @note
 * Hardware connections:
 * - **SSR:** GPIO (PB0)
 * - **GND:** Ground
 *
 * @version 1.0
 * @date    17apr2025
 */

#ifndef CLI_H
#define CLI_H

#include "stdint.h" // uint_t types

/**
 * @brief Custom formatted output function.
 *
 * This function works similarly to the standard printf(), sending the
 * formatted output to a designated output channel (for example, a debug console).
 * It supports a variable number of arguments according to the format string provided.
 *
 * @param fmt The format string that specifies how subsequent arguments are converted
 *            for output.
 * @param ... A variable list of arguments to be formatted and output.
 *
 * @note Use this function for debugging and logging purposes. Ensure that the format
 *       string and arguments are correctly matched to avoid runtime errors.
 */
void myprintf(const char *fmt, ...);

/**
 * @brief Processes a single console input via UART.
 *
 * This function takes one byte of input received from the console
 * and processes it accordingly. The processing includes buffering the input,
 * echoing it back, and interpreting it as part of a command sequence.
 *
 * @param input_console The byte of input received from the console.
 */
void console_UART(uint8_t input_console);

/**
 * @brief Executes a command received from the console.
 *
 * This function parses and executes a command represented by an array of bytes.
 * The number of valid characters in the command is specified by char_count.
 *
 * @param char_count The number of characters in the command.
 * @param command The array containing the command data.
 */
void execute(uint8_t char_count, uint8_t command[]);

/**
 * @brief Performs periodic system heartbeat operations.
 *
 * This function is intended to be called periodically to perform routine
 * tasks such as refreshing watchdog timer, updating system status,
 * and triggering other time-critical processes.
 */
void heartbeat(void);

/**
 * @brief Displays the framework for current system status.
 *
 * This function displays various system status information,
 * which can be useful for debugging or monitoring purposes.
 */
void status(void);

/**
 * @brief Displays help and usage information.
 *
 * This function prints a list of available commands or general usage information
 * to assist the user in understanding how to interact with the system.
 */
void help(void);

#endif
