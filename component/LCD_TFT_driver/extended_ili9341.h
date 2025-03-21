/**
 * @file extended_ili9341.h
 * @brief LCD TFT driver (ILI9341)
 */

#ifndef __EXTENDED_ILI9341_H__
#define __EXTENDED_ILI9341_H__
// INCLUDE DRIVERS
#include <stdio.h>
#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "soc/rtc.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/mcpwm.h"
#include "driver/uart.h"
#include "esp_timer.h"
#include "sdkconfig.h"

#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"

#include "lvgl.h"            // LVGL library
#include "esp_lcd_ili9341.h" //LCD TFT control driver
#if CONFIG_USING_LCD_TOUCH_CONTROLLER_XPT2046
#include "esp_lcd_touch_xpt2046.h" //LCD TFT touch driver
#endif

#define LCD_HOST_SPI VSPI_HOST
#define LCD_PIXEL_CLOCK_HZ (20 * 1000 * 1000)
#define LCD_BK_LIGHT_ON_LEVEL 1
#define LCD_BK_LIGHT_OFF_LEVEL !LCD_BK_LIGHT_ON_LEVEL
// The pixel number in horizontal and vertical
#define LCD_H_RES 240
#define LCD_V_RES 320
// Bit number used to represent command and parameter
#define LCD_CMD_BITS 8
#define LCD_PARAM_BITS 8
#define LVGL_TICK_PERIOD_MS 2
#define LVGL_TASK_MAX_DELAY_MS 500
#define LVGL_TASK_MIN_DELAY_MS 1
#define LVGL_TASK_STACK_SIZE (4 * 1024)
#define LVGL_TASK_PRIORITY 2

/**
 * @brief Initialize the ILI9341 LCD display.
 */
void ili9341_init();

#endif // __EXTENDED_ILI9341_H__
