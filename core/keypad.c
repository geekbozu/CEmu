#include "core/keypad.h"
#include <stdio.h>
#include <stdlib.h>

// Global KEYPAD state
keypad_state_t keypad;

uint8_t keypad_read(const uint16_t pio)
{
  int index = (int)pio & 0x7F;
  int bit_offset = (index&3)<<3;

  switch( upperNibble8(index) ) {
    case 0x1:
    case 0x2:        // keypad data range
         return read8(keypad.data[(mask8(index)-0x10) & 15],(index & 1)<<3);
    default:
      switch( mask8(index) ) {
      case 0x00:
      case 0x01:
      case 0x02:
      case 0x03:     // keypad contoller range
           return read8(keypad.cntrl,bit_offset);
      case 0x04:
      case 0x05:     // keypad size range
           return read8(keypad.size,bit_offset);
      case 0x08:
      case 0x09:
      case 0x0A:
      case 0x0B:     // keypad interrupt acknowledge range
           return read8(keypad.interrupt_ack,bit_offset);
      case 0x0C:
      case 0x0D:
      case 0x0E:
      case 0x0F:     // keypad interrupt mask range
           return read8(keypad.interrupt_mask,bit_offset);
      case 0x40:
      case 0x41:
      case 0x42:
      case 0x43:     // GPIO interrupt acknowlegde range
           return read8(keypad.gpio_interrupt_ack,bit_offset);
      case 0x44:
      case 0x45:
      case 0x46:
      case 0x47:     // GPIO interrupt mask range
           return read8(keypad.gpio_interrupt_mask,bit_offset);
        default:
           return 0; // return 0 if unimplemented or not in range
       }
   }
}

void keypad_write(const uint16_t pio, const uint8_t byte)
{
 int index = (int)pio % 0x80;
 int bit_offset = (index&3)<<3;

 switch( upperNibble8(index) ) {
   case 0x1:
   case 0x2:    // we can't write to the data section (read-only)
           return;
   default:
     switch( mask8(index) ) {
        case 0x00:
        case 0x01:
        case 0x02:
        case 0x03:
             write8(keypad.cntrl,bit_offset,byte);
             return;
        case 0x04:
        case 0x05:
             write8(keypad.size,bit_offset,byte);
             return;
        case 0x08:
        case 0x09:
        case 0x0A:
        case 0x0B:
             write8(keypad.interrupt_ack,bit_offset,byte);
             return;
        case 0x0C:
        case 0x0D:
       case 0x0E:
       case 0x0F:
            write8(keypad.interrupt_mask,bit_offset,byte);
            return;
       case 0x40:
       case 0x41:
       case 0x42:
       case 0x43:
            write8(keypad.gpio_interrupt_ack,bit_offset,byte);
            return;
       case 0x44:
       case 0x45:
       case 0x46:
       case 0x47:
            write8(keypad.gpio_interrupt_mask,bit_offset,byte);
            return;
       default: return;  // Escape write sequence if unimplemented
     }
  }
}

eZ80portrange_t init_keypad() {
	int i;
	for(i=0; i<16; i++) {
	    keypad.data[i] = 0;
	}
	eZ80portrange_t device = { keypad_read, keypad_write };
	return device;
}