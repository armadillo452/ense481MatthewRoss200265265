/**
 * @file epaper_ctl.h
 * @brief Functions for controlling and updating the e-paper display.
 *
 * This header declares functions to initialize the e-paper display,
 * clear it initially, and show the display status.
 *
 * @note
 * Hardware connections:
 * - **RES:** GPIO (PB5) Reset
 * - **BUSY:** GPIO (PB4)
 * - **D/C:** GPIO (PB3) Data/Command
 * - **CS:**  GPIO (PB1) Chip Select
 * - **SCK:** SPI2_SCK (PB13)
 * - **MOSI:** SPI2_MOSI (PB15)
 * - **VDD:** VDD – 3.3V or 5V
 *
 * @author Matthew Ross 200265265
 *
 * @version 1.0
 * @date    17apr2025
 */

#ifndef EPAPER_CTL_H
#define EPAPER_CTL_H

/**
 * @brief Initializes the e-paper display.
 *
 * This function performs the initial setup of the e-paper display. It calls the
 * main e-paper initialization routine, clears the display, configures partial
 * update mode, and then displays the current status on the e-paper.
 */
void epaper_start(void);

/**
 * @brief Performs an initial clear of the e-paper display.
 *
 * This function prepares a new image buffer, selects it for drawing,
 * clears the image to white, and sends it to the display. It is typically called
 * during the startup sequence.
 */
void epaper_clear_initial(void);

/**
 * @brief Displays status framework on the e-paper.
 *
 * This function writes a series of strings on the e-paper display to form
 * framework and formatting.
 */
void epaper_status(void);

#endif
