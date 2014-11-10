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

#include <util/delay.h>
#include "deca_keyer.h"

// -----------------------------------------------------------------------------
// --- Led interface
// -----------------------------------------------------------------------------

const led leds[N_LEDS] =
{
    {2, 0}, // PF0
    {2, 1}, // PF1
    {2, 4}, // PF4
    {2, 5}  // PF5
};


// -----------------------------------------------------------------------------
// --- Keys interface
// -----------------------------------------------------------------------------

// Chord keys on the fingers
const key keys[N_CHORD_KEYS] =
{
    {0, 0}, // PB0
    {0, 1}, // PB1
    {0, 2}, // PB2
    {0, 3}, // PB3
    {0, 7}, // PB7
    {1, 0}, // PD0
    {1, 1}, // PD1
    {1, 2}, // PD2
    {1, 4}  // PD4
};

// Modifier keys on the thumb
const key modifier_keys[N_MODIFIER_KEYS] =
{
    {1, 6}, // PD6
    {1, 7}  // PD7
};

// Mouse modifier keys on the pinky
// (only the first three fingers use for left, middle and right)
const key mouse_modifier_keys[N_MODIFIER_KEYS] =
{
    {1, 1}, // PD1
    {1, 2}  // PD2
};

const key other_keys[N_OTHER_KEYS] =
{
    {1, 3}  // PD3
};


// -----------------------------------------------------------------------------
// --- Trackball interface
// -----------------------------------------------------------------------------

const dir dirs[N_DIRS] =
{
    {0, 5}, // PB5
    {0, 6}, // PB6
    {2, 6}, // PF6
    {2, 7}  // PF7
};

const key button = {0, 4}; // PB4


// -----------------------------------------------------------------------------
// --- Led interface
// -----------------------------------------------------------------------------

void config_led(const led *led_ptr)
{
    switch (led_ptr->port)
    {
        case 0:
            (DDRB |= (1<<led_ptr->bit));
            break;
        case 1:
            (DDRD |= (1<<led_ptr->bit));
            break;
        case 2:
            (DDRF |= (1<<led_ptr->bit));
            break;
    }
}

void led_on(const uint8_t ledi)
{
    const led *led_ptr = &leds[ledi];

    switch (led_ptr->port)
    {
        case 0:
            (PORTB |= (1<<led_ptr->bit));
            break;
        case 1:
            (PORTD |= (1<<led_ptr->bit));
            break;
        case 2:
            (PORTF |= (1<<led_ptr->bit));
            break;
    }
}

void led_off(const uint8_t ledi)
{
    const led *led_ptr = &leds[ledi];

    switch (led_ptr->port)
    {
        case 0:
            (PORTB &= ~(1<<led_ptr->bit));
            break;
        case 1:
            (PORTD &= ~(1<<led_ptr->bit));
            break;
        case 2:
            (PORTF &= ~(1<<led_ptr->bit));
            break;
    }
}


// -----------------------------------------------------------------------------
// --- Trackball interface
// -----------------------------------------------------------------------------

void config_tball(const port_bit *pb_ptr)
{
    switch (pb_ptr->port)
    {
        case 0:
            (DDRB &= ~(1<<pb_ptr->bit));
            (PORTB &= ~(1<<pb_ptr->bit));
            break;
        case 1:
            (DDRD &= ~(1<<pb_ptr->bit));
            (PORTD &= ~(1<<pb_ptr->bit));
            break;
        case 2:
            (DDRF &= ~(1<<pb_ptr->bit));
            (PORTF &= ~(1<<pb_ptr->bit));
            break;
    }
}


// -----------------------------------------------------------------------------
// --- Keys interface
// -----------------------------------------------------------------------------

void config_key(const port_bit *pb_ptr)
{
    switch (pb_ptr->port)
    {
        case 0:
            (DDRB &= ~(1<<pb_ptr->bit));
            (PORTB |= ~(1<<pb_ptr->bit));
            break;
        case 1:
            (DDRD &= ~(1<<pb_ptr->bit));
            (PORTD |= ~(1<<pb_ptr->bit));
            break;
        case 2:
            (DDRF &= ~(1<<pb_ptr->bit));
            (PORTF |= ~(1<<pb_ptr->bit));
            break;
    }
}


// -----------------------------------------------------------------------------
// --- Initialize ports and USB interface
// -----------------------------------------------------------------------------

void initialize(void)
{
    // Set for 16 MHz clock
    CLKPR = 0x80;
    CLKPR = (0);

    // Configure leds
    for (uint8_t i=0; i<N_LEDS; i++)
    {
        config_led(&leds[i]);
    }

    // Configure trackball directions
    for (uint8_t i=0; i<N_DIRS; i++)
    {
        config_tball(&dirs[i]);
    }

    // Configure trackball button
    config_tball(&button);

    // Configure chorded keys
    for (uint8_t i=0; i<N_CHORD_KEYS; i++)
    {
        config_key(&keys[i]);
    }

    // Configure modifier keys
    for (uint8_t i=0; i<N_MODIFIER_KEYS; i++)
    {
        config_key(&modifier_keys[i]);
    }

    usb_init();
    while (!usb_configured());

    // Wait an extra second for the PC's operating system to load drivers
    // and do whatever it does to actually be ready for input
    _delay_ms(1000);
}


// -----------------------------------------------------------------------------
// --- Port state
// -----------------------------------------------------------------------------

// Current state of the ports
//
uint8_t ports_state[N_PORTS];

// Previous state of the ports
//
uint8_t ports_prev_state[N_PORTS];

// Read the state of the ports and store in the argument
//
void read_ports(uint8_t *ports_state)
{
    ports_state[0] = PINB;
    ports_state[1] = PIND;
    ports_state[2] = PINF;
}

// Trackball button
//   Return 1 if button pressed and released, otherwise 0
//
uint8_t pressed(const port_bit *button_ptr)
{
    return (KEY_PREV_PRESSED(*button_ptr) && !KEY_PRESSED(*button_ptr));
}

// Trackball motion
//   The trackball has moved if the state of the corresponding port bits
//   have changed.
//   The motion is provided as two int8_t for the up-down and left-right motion
//   which may be:
//     positive: motion in the positive direction
//     negative: motion in the negative direction
//     zero:     No movement in this direction
//
void motion(int8_t *ud, int8_t *lr)
{
    *ud =
        (BIT_STATE(dirs[DOWN]) != BIT_PREV_STATE(dirs[DOWN]))
      - (BIT_STATE(dirs[UP]) != BIT_PREV_STATE(dirs[UP]));

    *lr =
        (BIT_STATE(dirs[RIGHT]) != BIT_PREV_STATE(dirs[RIGHT]))
      - (BIT_STATE(dirs[LEFT]) != BIT_PREV_STATE(dirs[LEFT]));
}

// Is the led on
//   Return 1 if the led denoted by the index provided in on, otherwise return 0
uint8_t is_led_on(const uint8_t l)
{
    return BIT_STATE(leds[l]);
}


uint16_t chord_id(void)
{
    uint16_t cid = KEY_PREV_PRESSED(keys[0]);

    for (uint8_t i=1; i<N_CHORD_KEYS; i++)
    {
        if (KEY_PREV_PRESSED(keys[i]))
        {
            cid += 1<<i;
        }
    }

    return cid;
}


uint16_t get_chord(void)
{
    // Keep track of new chords
    static uint8_t new_chord = 0;

    // Keep track of repeated chords
    static uint16_t repeat = 0;

    // Is any switch currently pressed
    uint8_t pressed = 0;
    for (uint8_t i=0; i<N_CHORD_KEYS; i++)
    {
        if (KEY_PRESSED(keys[i]))
        {
            pressed = 1;
            break;
        }
    }

    // If any switch is pressed check if it is a repeated chord
    if (pressed)
    {
        // Limited repeat increment
        repeat += (repeat > N_REPEAT ? 0 : 1);

        // Check if same chord is pressed, if not reset repeat
        for (uint8_t i=0; i<N_CHORD_KEYS; i++)
        {
            if (KEY_PREV_PRESSED(keys[i]) != KEY_PRESSED(keys[i]))
            {
                repeat = 0;
                break;
            }
        }

        // If the chord has been repeated enough times resend
        if (repeat > N_REPEAT)
        {
            _delay_ms(REPEAT_RATE);
            return chord_id();
        }
    }

    // Check if any key has been pressed which initiates a new chord
    if (!new_chord)
    {
        for (uint8_t i=0; i<N_CHORD_KEYS; i++)
        {
            if (!KEY_PREV_PRESSED(keys[i]) && KEY_PRESSED(keys[i]))
            {
                new_chord = 1;
                break;
            }
        }
    }

    if (new_chord)
    {
        uint8_t chord_complete = 0;

        // Check if any key has been released which completes a chord
        for (uint8_t i=0; i<N_CHORD_KEYS; i++)
        {
            if (KEY_PREV_PRESSED(keys[i]) && !KEY_PRESSED(keys[i]))
            {
                chord_complete = 1;
                break;
            }
        }

        // If the chord is complete return the chord ID
        if (chord_complete)
        {
            // Set the new chord indicator to false
            new_chord = 0;

            return chord_id();
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}


uint8_t get_modifier(const key modifier_keys[])
{
    if (KEY_PRESSED(modifier_keys[0]) && KEY_PRESSED(modifier_keys[1]))
    {
        return KEY_ALT;
    }
    else if (KEY_PRESSED(modifier_keys[0]))
    {
        return KEY_SHIFT;
    }
    else if (KEY_PRESSED(modifier_keys[1]))
    {
        return KEY_CTRL;
    }
    else
    {
        return 0;
    }
}


uint8_t get_sticky_modifier(const key modifier_keys[])
{
    // Keep track of new modifier
    static uint8_t new_modifier = 0;

    // Check if any modifier key has been pressed which initiates a new modifier
    if (!new_modifier)
    {
        if
        (
            (
                !KEY_PREV_PRESSED(modifier_keys[0])
             && KEY_PRESSED(modifier_keys[0])
            )
         || (
                !KEY_PREV_PRESSED(modifier_keys[1])
              && KEY_PRESSED(modifier_keys[1])
            )
        )
        {
            new_modifier = 1;
        }
    }

    // Check if any modifier key has been released which completes a modifier
    if
    (
        new_modifier
     && (
            (
                KEY_PREV_PRESSED(modifier_keys[0])
             && !KEY_PRESSED(modifier_keys[0])
            )
         || (
                KEY_PREV_PRESSED(modifier_keys[1])
             && !KEY_PRESSED(modifier_keys[1])
            )
        )
    )
    {
        new_modifier = 0;

        if
        (
            KEY_PREV_PRESSED(modifier_keys[0])
         && KEY_PREV_PRESSED(modifier_keys[1])
        )
        {
            return KEY_ALT;
        }
        else if (KEY_PREV_PRESSED(modifier_keys[0]))
        {
            return KEY_SHIFT;
        }
        else if (KEY_PREV_PRESSED(modifier_keys[1]))
        {
            return KEY_CTRL;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}


#include "chord_map.h"

void send_key_codes(const uint16_t chord_id, const uint8_t modifiers)
{
    uint8_t mod = modifiers;

    chord_codes chord;
    mkuc codes[32];

    // Get the chord codes from flash
    memcpy_P(&chord, &chord_map[chord_id-1], sizeof(chord_codes));
    memcpy_P(codes, chord.codes, chord.len*sizeof(mkuc));

    // Send the key codes to the USB port
    for (uint8_t i=0; i<chord.len; i++)
    {
        // if (codes[i].mod == KEY_STICKY)
        // {
        //     // Send sticky modifier
        // }
        // else
        {
            usb_keyboard_press
            (
                codes[i].code,
                mod|codes[i].mod
            );
        }
    }
}


#include "mouse_chord_map.h"

uint16_t get_mouse_chord(void)
{
    uint16_t cid = KEY_PRESSED(keys[0]);

    for (uint8_t i=1; i<N_MOUSE_CHORD_KEYS; i++)
    {
        if (KEY_PRESSED(keys[i]))
        {
            cid += 1<<i;
        }
    }

    return cid;
}


void send_mouse_codes
(
    uint16_t chord_id,
    const uint8_t modifiers,
    const uint8_t prev_modifiers
)
{
    // Release modifiers
    if (prev_modifiers && !modifiers)
    {
        keyboard_modifier_keys = 0;
        keyboard_keys[0] = 0;
        usb_keyboard_send();
    }

    if (chord_id)
    {
        chord_codes chord;
        mkuc codes[32];

        // Get the chord codes from flash
        memcpy_P(&chord, &mouse_chord_map[chord_id-1], sizeof(chord_codes));
        memcpy_P(codes, chord.codes, chord.len*sizeof(mkuc));

        // Send all the modifiers
        if (modifiers || codes[0].mod)
        {
            keyboard_modifier_keys = modifiers|codes[0].mod;
            keyboard_keys[0] = 0;
            usb_keyboard_send();
        }

        // Double-click
        if (codes[0].code & MOUSE_DOUBLE)
        {
            mouse_buttons = codes[0].code;
            usb_mouse_move(0, 0, 0);
            mouse_buttons = 0;
            usb_mouse_move(0, 0, 0);
            mouse_buttons = codes[0].code;
            usb_mouse_move(0, 0, 0);
        }
        // Single-click
        else
        {
            mouse_buttons = codes[0].code;
            usb_mouse_move(0, 0, 0);
        }

        // Release the local modifiers
        if (modifiers || codes[0].mod)
        {
            keyboard_modifier_keys = modifiers;
            keyboard_keys[0] = 0;
            usb_keyboard_send();
        }
    }
    else if (!(mouse_buttons & MOUSE_TOGGLE))
    {
        mouse_buttons = 0;
        usb_mouse_move(0, 0, 0);
    }
}


// -----------------------------------------------------------------------------
