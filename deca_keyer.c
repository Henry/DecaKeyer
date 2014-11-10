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
#include "usb_print.h"

// -----------------------------------------------------------------------------
// --- Main
// -----------------------------------------------------------------------------

int main(void)
{
    // Current chord ID and modifiers
    uint16_t chord_id = 0;
    uint8_t modifiers = 0, sticky_modifier = 0, prev_sticky_modifier = 0;
    uint8_t n_modifier_clicks = 0, modifier_timer = 0;
    uint8_t sticky_modifiers = 0;

    // Mouse mode, pointer movement and previous chord ID
    uint8_t mouse_mode = 0;
    int8_t ud = 0, lr = 0;
    uint16_t mouse_chord_id = 0, prev_mouse_chord_id = 0;
    uint8_t mouse_modifiers = 0, prev_mouse_modifiers = 0;


    initialize();

    read_ports(ports_prev_state);

    print("Begin deca_keyer\n");
    while (1)
    {
        read_ports(ports_state);

        if (pressed(&button))
        {
            mouse_mode = !mouse_mode;

            if (mouse_mode)
            {
                led_on(WHITE_LED);

                // Reset sticky-modifiers
                sticky_modifiers = 0;
                sticky_modifier = 0;
                prev_sticky_modifier = 0;
                n_modifier_clicks = 0;
                modifier_timer = 0;
                led_off(BLUE_LED);
                led_off(RED_LED);
                led_off(GREEN_LED);
            }
            else
            {
                usb_mouse_buttons(MOUSE_BUTTON_LEFT);
                mouse_buttons = 0;
                usb_mouse_move(0, 0, 0);
                led_off(WHITE_LED);

                // Reset sticky-modifiers
                sticky_modifiers = 0;
                sticky_modifier = 0;
                prev_sticky_modifier = 0;
                n_modifier_clicks = 0;
                modifier_timer = 0;
                led_off(BLUE_LED);
                led_off(RED_LED);
                led_off(GREEN_LED);
            }
        }

        // Get the trackball motion and move pointer
        motion(&ud, &lr);
        if (ud || lr)
        {
            prev_mouse_chord_id = 0;

            if (mouse_mode)
            {
                // Mouse pointer movement
                usb_mouse_move(10*lr, 10*ud, 0);
            }
            else
            {
                // Mouse cursor movement
                if (lr > 0)
                {
                    usb_keyboard_press(KEY_RIGHT, modifiers|sticky_modifiers);
                }
                else if (lr < 0)
                {
                    usb_keyboard_press(KEY_LEFT, modifiers|sticky_modifiers);
                }

                if (ud > 0)
                {
                    usb_keyboard_press(KEY_DOWN, modifiers|sticky_modifiers);
                }
                else if (ud < 0)
                {
                    usb_keyboard_press(KEY_UP, modifiers|sticky_modifiers);
                }
            }
        }

        if (mouse_mode)
        {
            // Get the modifiers from the pinky switches
            mouse_modifiers = get_modifier(mouse_modifier_keys);

            // Get the chord and send the corresponding mouse buttons
            mouse_chord_id = get_mouse_chord();

            if
            (
                mouse_chord_id != prev_mouse_chord_id
             || mouse_modifiers != prev_mouse_modifiers
            )
            {
                print("Mouse chord ");
                phex(mouse_chord_id);
                print("\n");
                send_mouse_codes
                (
                    mouse_chord_id,
                    mouse_modifiers,
                    prev_mouse_modifiers
                );
                prev_mouse_chord_id = mouse_chord_id;
                prev_mouse_modifiers = mouse_modifiers;
            }
        }
        else
        {
            // Get the modifiers from the thumb switches
            modifiers = get_modifier(modifier_keys);
            sticky_modifier = get_sticky_modifier(modifier_keys);

            // Get the chord and send the corresponding modified key codes
            if ((chord_id = get_chord()))
            {
                print("Chord ");
                phex16(chord_id);
                print("\n");
                send_key_codes(chord_id, modifiers|sticky_modifiers);

                // Reset sticky-modifiers
                sticky_modifiers = 0;
                sticky_modifier = 0;
                prev_sticky_modifier = 0;
                n_modifier_clicks = 0;
                modifier_timer = 0;
                led_off(BLUE_LED);
                led_off(RED_LED);
                led_off(GREEN_LED);
            }
            else if (prev_sticky_modifier && !sticky_modifier)
            {
                // Check for sticky modifiers
                n_modifier_clicks++;

                // If sticky modifier pressed twice record it
                if (n_modifier_clicks == 2)
                {
                    sticky_modifiers |= prev_sticky_modifier;
                    print("Sticky Modifier ");
                    phex(prev_sticky_modifier);
                    print("\n");

                    if (sticky_modifiers & KEY_SHIFT)
                    {
                        led_on(BLUE_LED);
                    }
                    if (sticky_modifiers & KEY_CTRL)
                    {
                        led_on(RED_LED);
                    }
                    if (sticky_modifiers & KEY_ALT)
                    {
                        led_on(GREEN_LED);
                    }

                    n_modifier_clicks = 0;
                    modifier_timer = 0;
                }
                else if (sticky_modifiers & prev_sticky_modifier)
                {
                    // Reset sticky-modifiers
                    sticky_modifiers = 0;
                    sticky_modifier = 0;
                    prev_sticky_modifier = 0;
                    n_modifier_clicks = 0;
                    modifier_timer = 0;
                    led_off(BLUE_LED);
                    led_off(RED_LED);
                    led_off(GREEN_LED);
                }
            }
            else if (n_modifier_clicks == 1)
            {
                modifier_timer++;

                if (modifier_timer > DOUBLE_CLICK_COUNT)
                {
                    n_modifier_clicks = 0;
                    modifier_timer = 0;
                }
            }

            prev_sticky_modifier = sticky_modifier;
        }

        // Store the port state as the previous state
        for (uint8_t i=0; i<N_PORTS; i++)
        {
            ports_prev_state[i] = ports_state[i];
        }

        // Wait a short time to limit the effect of mechanical "bounce"
        _delay_ms(LOOP_DELAY);
    }
}


// -----------------------------------------------------------------------------
