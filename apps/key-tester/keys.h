#ifndef PROBE_KEYS_H
#define PROBE_KEYS_H
#include <stdint.h>
enum { KEY_ROWS = 12, KEY_COLS = 8 };
typedef struct { uint8_t rows[KEY_ROWS]; } key_state_t;
typedef void (*key_change_fn)(unsigned row, unsigned col,
    unsigned pressed, void *context);
void keys_init(key_state_t *state, const uint8_t scan[KEY_ROWS]);
/* Report all transitions, including unmapped bits. Release before press. */
unsigned keys_update(key_state_t *state, const uint8_t scan[KEY_ROWS],
    key_change_fn callback, void *context);
unsigned keys_down_count(const key_state_t *state);
const char *key_name(unsigned row, unsigned col);
#endif
