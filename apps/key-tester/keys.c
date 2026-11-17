#include "keys.h"
/* Transcribed from gint-exword's EX_translation_table,
 * feature/exword-runtime-bridge, src/keysc/keydev.c.
 * [row][col] corresponds to bit col in the twelve scan bytes.
 */
static const char *const names[KEY_ROWS][KEY_COLS] = {
    { "AC/ON", 0, 0, 0, 0, 0, 0, 0 },
    { 0, "F1", "F2", "F3", "F4", "F5", "F6", "F7" },
    { 0, "F8", 0, 0, 0, 0, 0, 0 },
    { 0, "Q", "W", "A", "SHIFT", "Z", "HISTORY", "PAGEUP" },
    { 0, "E", "S", "D", "X", "ZOOM", "LAYOUT", "PAGEDOWN" },
    { 0, "R", "T", "F", "C", "V", "EXIT", "B" },
    { 0, "U", "I", "J", "N", "H", "EXE", "DOWN" },
    { 0, "O", "K", "M", "Y", "G", "UP", "LEFT" },
    { 0, "P", "L", "ALPHA", "DEL", "AUDIO", "JUMP", "RIGHT" },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
};
const char *key_name(unsigned row, unsigned col)
{
    if(row >= KEY_ROWS || col >= KEY_COLS) return "UNMAPPED";
    const char *name = names[row][col];
    return name ? name : "UNMAPPED";
}
void keys_init(key_state_t *state, const uint8_t scan[KEY_ROWS])
{
    for(unsigned r = 0; r < KEY_ROWS; ++r) state->rows[r] = scan[r];
}
unsigned keys_update(key_state_t *state, const uint8_t scan[KEY_ROWS],
    key_change_fn callback, void *context)
{
    unsigned events = 0;
    for(unsigned pressed = 0; pressed < 2; ++pressed) {
        for(unsigned r = 0; r < KEY_ROWS; ++r) {
            uint8_t before = state->rows[r], after = scan[r];
            uint8_t changed = pressed ? (uint8_t)(~before & after)
                                      : (uint8_t)(before & ~after);
            for(unsigned c = 0; c < KEY_COLS; ++c) {
                if(!(changed & (1u << c))) continue;
                ++events;
                if(callback) callback(r, c, pressed, context);
            }
        }
    }
    for(unsigned r = 0; r < KEY_ROWS; ++r) state->rows[r] = scan[r];
    return events;
}
unsigned keys_down_count(const key_state_t *state)
{
    unsigned total = 0;
    for(unsigned r = 0; r < KEY_ROWS; ++r)
        for(unsigned c = 0; c < KEY_COLS; ++c)
            if(state->rows[r] & (1u << c)) ++total;
    return total;
}
