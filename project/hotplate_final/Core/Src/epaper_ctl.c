#include "epaper.h"
#include "epaper_ctl.h"
#include "stdio.h"
#include "main.h"
#include "cli.h"
#include "string.h"
#include "stm32f1xx_hal.h"

uint8_t image_bw[EPD_W_BUFF_SIZE * EPD_H];
uint8_t clearPaper = 1; // Initialize flag for clearing epaper

void epaper_start() {
  epd_init(); // main init
  epaper_clear_initial();
  epd_init_partial(); // init partial update mode
  epaper_status();
}

void epaper_clear_initial() {
  epd_paint_newimage(image_bw, EPD_W, EPD_H, EPD_ROTATE_180, EPD_COLOR_WHITE);
  epd_paint_selectimage(image_bw);
  epd_paint_clear(EPD_COLOR_WHITE);
  epd_displayBW(image_bw);
}

void epaper_status(void) {
  epd_paint_showString(4, 4, (uint8_t*) "       - Frynet by Cyberdining Systems -",
  EPD_FONT_SIZE8x6, EPD_COLOR_BLACK);
  epd_paint_showString(4, 14, (uint8_t*) "          B-800 Burninator",
  EPD_FONT_SIZE16x8, EPD_COLOR_BLACK);
  epd_paint_showString(4, 32, (uint8_t*) "------------------------------------------------",
  EPD_FONT_SIZE8x6, EPD_COLOR_BLACK);
//	epd_paint_showString(4, 42,
//			(uint8_t*) "VDD :        Chip :        Uptime :              ",
//			EPD_FONT_SIZE8x6, EPD_COLOR_BLACK);
  epd_paint_showString(4, 52, (uint8_t*) "------------------------------------------------",
  EPD_FONT_SIZE8x6, EPD_COLOR_BLACK);
//	epd_paint_showString(4, 62,
//			(uint8_t*) "Target:        Current:              ",
//			EPD_FONT_SIZE16x8, EPD_COLOR_BLACK);
  epd_displayBW_partial(image_bw);
}
