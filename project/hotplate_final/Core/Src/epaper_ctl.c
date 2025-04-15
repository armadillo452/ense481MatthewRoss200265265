#include "epaper.h"
#include "epaper_ctl.h"
#include "stdio.h"
#include "main.h"
#include "cli.h"
#include "string.h"
#include "stm32f1xx_hal.h"

uint8_t image_bw[EPD_W_BUFF_SIZE * EPD_H];

void epaper_status(void) {
	// init and clear screen
	epd_init();
	epd_paint_newimage(image_bw, EPD_W, EPD_H, EPD_ROTATE_180, EPD_COLOR_WHITE);
	epd_paint_selectimage(image_bw);
	epd_paint_clear(EPD_COLOR_WHITE);
	epd_displayBW(image_bw);

	epd_init_partial(); // init partial update mode

	// status
	epd_paint_showString(0, 0,
			(uint8_t*) "       - Frynet by Cyberdining Systems -",
			EPD_FONT_SIZE8x6, EPD_COLOR_BLACK);
	epd_paint_showString(0, 10,
			(uint8_t*) "-------------------------------------------------",
			EPD_FONT_SIZE8x6, EPD_COLOR_BLACK);
	epd_paint_showString(0, 20,
			(uint8_t*) "VDD :        Chip :        Uptime :              ",
			EPD_FONT_SIZE8x6, EPD_COLOR_BLACK);
	epd_paint_showString(0, 30,
			(uint8_t*) "-------------------------------------------------",
			EPD_FONT_SIZE8x6, EPD_COLOR_BLACK);
	epd_paint_showString(0, 40,
			(uint8_t*) "Target:        Current:              ",
			EPD_FONT_SIZE16x8, EPD_COLOR_BLACK);
	epd_paint_showString(0, 60,
			(uint8_t*) "-------------------------------------------------",
			EPD_FONT_SIZE8x6, EPD_COLOR_BLACK);
	epd_displayBW_partial(image_bw);
}
