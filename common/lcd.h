#ifndef PROBE_LCD_H
#define PROBE_LCD_H
#include <stdint.h>
enum { LCD_WIDTH = 528, LCD_HEIGHT = 320 };
enum { LCD_WHITE = 0xffff, LCD_BLACK = 0x0000,
       LCD_BLUE = 0x125f, LCD_GREY = 0x5aeb, LCD_LIGHT = 0xe71c };
void lcd_clear(uint16_t color);
void lcd_rect(int x0, int y0, int x1, int y1, uint16_t color);
void lcd_text(int x, int y, const char *s, uint16_t color);
void lcd_present(void (*poll)(void));
#endif
