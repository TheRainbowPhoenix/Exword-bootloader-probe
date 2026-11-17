/* Read-only standalone UPDADN2 keyboard probe; no OS/filesystem calls. */
#include <stdint.h>
#include "keys.h"
#include "lcd.h"
#define KEYSC ((volatile const uint16_t *)0xa44b0000)
#define LOG_SIZE 16u
typedef struct {
    uint32_t serial;
    uint8_t row, col, pressed;
} log_entry_t;
static key_state_t keyboard;
static log_entry_t history[LOG_SIZE];
static uint32_t total_events;
static unsigned needs_redraw;

static void scan_matrix(uint8_t rows[KEY_ROWS])
{
    for(unsigned i = 0; i < KEY_ROWS / 2; ++i) {
        uint16_t word = KEYSC[i];
        rows[2 * i] = (uint8_t)word;
        rows[2 * i + 1] = (uint8_t)(word >> 8);
    }
}
static void record_change(unsigned row, unsigned col,
    unsigned pressed, void *context)
{
    (void)context;
    ++total_events;
    log_entry_t *e = &history[(total_events - 1u) & (LOG_SIZE - 1u)];
    e->serial = total_events;
    e->row = (uint8_t)row;
    e->col = (uint8_t)col;
    e->pressed = (uint8_t)pressed;
    needs_redraw = 1;
}
static void poll_keys(void)
{
    uint8_t scan[KEY_ROWS];
    scan_matrix(scan);
    keys_update(&keyboard, scan, record_change, 0);
}
static char *append(char *out, const char *s)
{
    while(*s) *out++ = *s++;
    return out;
}
static char *hex(char *out, uint32_t value, unsigned digits)
{
    static const char alphabet[] = "0123456789ABCDEF";
    for(unsigned i = 0; i < digits; ++i)
        *out++ = alphabet[(value >> ((digits - 1u - i) * 4u)) & 15u];
    return out;
}
static void show_matrix(void)
{
    lcd_text(333, 67, "RAW  ROW  BITS", LCD_GREY);
    for(unsigned row = 0; row < KEY_ROWS; ++row) {
        char line[32];
        char *p = append(line, "R");
        p = hex(p, row, 2);
        p = append(p, "   ");
        for(int col = 7; col >= 0; --col)
            *p++ = (keyboard.rows[row] & (1u << col)) ? '1' : '0';
        *p = '\0';
        lcd_text(333, 85 + 15 * (int)row, line, LCD_BLACK);
    }
}
static void show_events(void)
{
    lcd_text(13, 68, "SEQ      EVENT  KEY        R:C", LCD_GREY);
    uint32_t count = total_events < LOG_SIZE ? total_events : LOG_SIZE;
    for(uint32_t i = 0; i < count; ++i) {
        uint32_t serial = total_events - count + i;
        const log_entry_t *e = &history[serial & (LOG_SIZE - 1u)];
        char line[64];
        char *p = hex(line, e->serial, 6);
        p = append(p, e->pressed ? "   DOWN  " : "   UP    ");
        p = append(p, key_name(e->row, e->col));
        while(p - line < 25) *p++ = ' ';
        p = hex(p, e->row, 2);
        *p++ = ':';
        p = hex(p, e->col, 1);
        *p = '\0';
        lcd_text(13, 86 + 12 * (int)i, line, LCD_BLACK);
    }
}
static void render(void)
{
    char status[64];
    char *p;
    lcd_clear(LCD_WHITE);
    lcd_rect(0, 0, LCD_WIDTH - 1, 31, LCD_BLUE);
    lcd_text(11, 11, "EX-word UPDADN2 - Key tester", LCD_WHITE);
    p = append(status, "Edges 0x");
    p = hex(p, total_events, 8);
    p = append(p, "   Held 0x");
    p = hex(p, keys_down_count(&keyboard), 2);
    *p = '\0';
    lcd_text(13, 44, status, LCD_BLACK);
    lcd_rect(320, 62, 321, 278, LCD_LIGHT);
    show_events();
    show_matrix();
    lcd_rect(0, 296, LCD_WIDTH - 1, LCD_HEIGHT - 1, LCD_BLUE);
    lcd_text(11, 304, "Press/release any key. No flash or file writes.", LCD_WHITE);
}
int main(void)
{
    uint8_t baseline[KEY_ROWS];
    scan_matrix(baseline);
    keys_init(&keyboard, baseline);
    total_events = 0;
    needs_redraw = 1;
    for(;;) {
        poll_keys();
        if(needs_redraw) {
            needs_redraw = 0;
            render();
            lcd_present(poll_keys);
        }
    }
}
