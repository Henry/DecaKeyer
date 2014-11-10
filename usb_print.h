#ifndef usb_print_h__
#define usb_print_h__

#include <avr/pgmspace.h>
#include "usb_interface.h"

#ifdef USB_DEBUG_HID

// this macro allows you to write print("some text") and
// the string is automatically placed into flash memory :)
#define print(s) print_P(PSTR(s))
#define pchar(c) usb_debug_putchar(c)

void print_P(const char *s);
void phex(unsigned char c);
void phex16(unsigned int i);
#else

#define print(s)
#define phex(c)
#define phex16(i)

#endif

#endif
