#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "keys.h"
typedef struct { unsigned row, col, pressed; } event_t;
static event_t received[100];
static unsigned n;
static void got(unsigned row, unsigned col, unsigned pressed, void *context)
{
    assert(context == received);
    assert(n < 100);
    received[n++] = (event_t){row, col, pressed};
}
static void expected(unsigned i, unsigned row, unsigned col, unsigned pressed)
{
    assert(i < n);
    assert(received[i].row == row);
    assert(received[i].col == col);
    assert(received[i].pressed == pressed);
}
int main(void)
{
    uint8_t scan[KEY_ROWS] = {0};
    key_state_t state;
    keys_init(&state, scan);
    assert(keys_down_count(&state) == 0);
    assert(!strcmp(key_name(3, 1), "Q"));
    assert(!strcmp(key_name(5, 6), "EXIT"));
    assert(!strcmp(key_name(6, 6), "EXE"));
    assert(!strcmp(key_name(1, 7), "F7"));
    assert(!strcmp(key_name(2, 1), "F8"));
    assert(!strcmp(key_name(9, 2), "UNMAPPED"));
    assert(!strcmp(key_name(12, 0), "UNMAPPED"));
    assert(!strcmp(key_name(0, 8), "UNMAPPED"));
    scan[3] = (1u << 1) | (1u << 4); /* Q and SHIFT */
    n = 0;
    assert(keys_update(&state, scan, got, received) == 2);
    expected(0, 3, 1, 1);
    expected(1, 3, 4, 1);
    assert(keys_down_count(&state) == 2);
    assert(keys_update(&state, scan, got, received) == 0);
    assert(n == 2); /* No invented hold events. */
    scan[3] = 1u << 4; /* Q release */
    scan[6] = 1u << 6; /* EXE press */
    assert(keys_update(&state, scan, got, received) == 2);
    expected(2, 3, 1, 0);
    expected(3, 6, 6, 1);
    scan[3] = 0; scan[6] = 0; scan[11] = 1u << 7;
    assert(keys_update(&state, scan, got, received) == 3);
    expected(4, 3, 4, 0);
    expected(5, 6, 6, 0);
    expected(6, 11, 7, 1);
    scan[11] = 0;
    assert(keys_update(&state, scan, got, received) == 1);
    expected(7, 11, 7, 0);
    assert(keys_down_count(&state) == 0);
    scan[8] = 1u << 2;
    keys_init(&state, scan);
    n = 0;
    assert(keys_update(&state, scan, got, received) == 0);
    scan[8] = 0;
    assert(keys_update(&state, scan, got, received) == 1);
    expected(0, 8, 2, 0);
    puts("key scan tests: PASS");
    return 0;
}
