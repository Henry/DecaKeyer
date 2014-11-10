/// Copyright 2012 Henry G. Weller
// -----------------------------------------------------------------------------
//  This file is part of
/// ---     DecaKeyer: Hand-held Chorded Keyboard
// -----------------------------------------------------------------------------
//
//  DecaKeyer is free software: you can redistribute it and/or modify it
//  under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  DecaKeyer is distributed in the hope that it will be useful, but WITHOUT
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
//  for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with DecaKeyer.  If not, see <http://www.gnu.org/licenses/>.
//
// -----------------------------------------------------------------------------
/// Title: Definitions
// -----------------------------------------------------------------------------

#ifndef deca_keyer_h
#define deca_keyer_h

#ifndef CONVERT_H
#include <avr/pgmspace.h>
#endif

#include "usb_interface.h"

// -----------------------------------------------------------------------------
// --- Ports
// -----------------------------------------------------------------------------

// Port B -> 0
// Port D -> 1
// Port F -> 2
#define N_PORTS 3

typedef struct
{
    uint8_t port;
    uint8_t bit;
} port_bit;


// -----------------------------------------------------------------------------
// --- Port state
// -----------------------------------------------------------------------------

// Current state of the ports
//
extern uint8_t ports_state[N_PORTS];

// Previous state of the ports
//
extern uint8_t ports_prev_state[N_PORTS];

#define BIT_STATE(pb) (ports_state[(pb).port] & (1<<(pb).bit))
#define BIT_PREV_STATE(pb) (ports_prev_state[(pb).port] & (1<<(pb).bit))

#define KEY_PRESSED(key) (!BIT_STATE(key))
#define KEY_PREV_PRESSED(key) (!BIT_PREV_STATE(key))

void read_ports(uint8_t *ports_state);


// -----------------------------------------------------------------------------
// --- Led interface
// -----------------------------------------------------------------------------

#define N_LEDS 4
#define WHITE_LED               0
#define RED_LED                 1
#define GREEN_LED               2
#define BLUE_LED                3

typedef port_bit led;
extern const led leds[N_LEDS];

void config_led(const led *led_ptr);
void led_on(const uint8_t ledi);
void led_off(const uint8_t ledi);
uint8_t is_led_on(const uint8_t l);

// -----------------------------------------------------------------------------
// --- Keys interface
// -----------------------------------------------------------------------------

#define N_CHORD_KEY_PAIRS 4
#define N_CHORD_SHIFT_KEYS 1
#define N_CHORD_KEYS (2*N_CHORD_KEY_PAIRS + N_CHORD_SHIFT_KEYS)
#define N_MODIFIER_KEYS 2
#define N_OTHER_KEYS 1
#define N_SHIFT_MAPS 1

typedef port_bit key;
extern const key keys[N_CHORD_KEYS];
extern const key modifier_keys[N_MODIFIER_KEYS];
extern const key mouse_modifier_keys[N_MODIFIER_KEYS];
extern const key other_keys[N_OTHER_KEYS];

void config_key(const port_bit *pb_ptr);


// -----------------------------------------------------------------------------
// --- Mouse buttons interface
// -----------------------------------------------------------------------------

#define N_MOUSE_CHORD_KEY_PAIRS 3
#define N_MOUSE_CHORD_KEYS (2*N_MOUSE_CHORD_KEY_PAIRS)


// -----------------------------------------------------------------------------
// --- Trackball interface
// -----------------------------------------------------------------------------

#define N_DIRS 4
#define UP                      0
#define DOWN                    1
#define LEFT                    2
#define RIGHT                   3

typedef port_bit dir;
extern const dir dirs[N_DIRS];
extern const key button;

void config_tball(const port_bit *pb_ptr);
uint8_t pressed(const port_bit *button_ptr);
void motion(int8_t *ud, int8_t *lr);

// -----------------------------------------------------------------------------
// --- Chord HID mapping
// -----------------------------------------------------------------------------

#define N_CHORDS (1<<N_CHORD_KEYS)

// HID keyboard usage code with modifier
typedef struct
{
    uint8_t mod;
    uint8_t code;
} mkuc;

typedef struct
{
    uint8_t len;
    const mkuc *codes;
} chord_codes;

#ifndef CONVERT_H
extern const chord_codes chord_map[N_CHORDS] PROGMEM;
#endif

// Example definition of chord_map
//
// const mkuc hmm1[] PROGMEM = {{0, KEY_A}};
// const mkuc hmm2[] PROGMEM = {{0, KEY_A}, {0, KEY_B}, {KEY_SHIFT, KEY_B}};
//
// const chord_codes chord_map[N_CHORDS] PROGMEM =
// {
//     {1, hmm1},
//     {3, hmm2}
// };

uint16_t chord_id(void);
uint16_t get_chord(void);
uint8_t get_modifier(const key modifier_keys[]);
uint8_t get_sticky_modifier(const key modifier_keys[]);
void send_key_codes(const uint16_t chord_id, const uint8_t modifiers);


// -----------------------------------------------------------------------------
// --- Character Repeat controls
// -----------------------------------------------------------------------------

#define LOOP_DELAY 4      // Main loop delay (ms)

#define REPEAT_DELAY 1000  // Delay before starting repeat (ms)
#define REPEAT_RATE 25    // Time between repeated characters (ms)

#define N_REPEAT (REPEAT_DELAY/LOOP_DELAY)


// Which buttons are currently pressed
extern uint8_t mouse_buttons;

#define N_MOUSE_CHORDS (1<<N_MOUSE_CHORD_KEYS)

uint16_t get_mouse_chord(void);

void send_mouse_codes
(
    uint16_t chord_id,
    const uint8_t modifiers,
    const uint8_t prev_modifiers
);

// -----------------------------------------------------------------------------
// --- Double-click delay
// -----------------------------------------------------------------------------

#define DOUBLE_CLICK_DELAY 400             // Maximum delay between clicks (ms)
#define DOUBLE_CLICK_COUNT (DOUBLE_CLICK_DELAY/LOOP_DELAY)


// -----------------------------------------------------------------------------
// --- Initialization
// -----------------------------------------------------------------------------

void initialize(void);


// -----------------------------------------------------------------------------
#endif
// -----------------------------------------------------------------------------
