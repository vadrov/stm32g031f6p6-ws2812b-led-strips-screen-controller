/*
 *      Author: VadRov
 */

#ifndef AVI_H_
#define AVI_H_

#include "ff.h"
#include "ws2812b.h"
#include "main.h"

uint8_t LED_Load_BMP (LED_Handler *led, uint16_t x, uint16_t y, uint16_t w, uint16_t h, FIL *file, uint32_t *load_bytes);
int PlayAVI(char *dir, char *fname, LED_Handler *led, uint16_t x, uint16_t y, uint16_t win_wdt, uint16_t win_hgt);

#endif /* AVI_H_ */
