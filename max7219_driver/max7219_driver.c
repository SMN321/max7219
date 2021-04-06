
#include "max7219_driver.h"
#include <avr/io.h>

static void send_byte(volatile uint8_t data) {
    //USIDR = data;
    // this seemed to work in the simulations
    // the whole block needed 20 clock cycles there
    __asm__ volatile (
    /* asm instructions */
    "out %[usidr], %[data]" "\n\t"
    "out %[usicr], %[high]" "\n\t"
    "out %[usicr], %[low]"  "\n\t"
    "out %[usicr], %[high]" "\n\t"
    "out %[usicr], %[low]"  "\n\t"
    "out %[usicr], %[high]" "\n\t"
    "out %[usicr], %[low]"  "\n\t"
    "out %[usicr], %[high]" "\n\t"
    "out %[usicr], %[low]"  "\n\t"
    "out %[usicr], %[high]" "\n\t"
    "out %[usicr], %[low]"  "\n\t"
    "out %[usicr], %[high]" "\n\t"
    "out %[usicr], %[low]"  "\n\t"
    "out %[usicr], %[high]" "\n\t"
    "out %[usicr], %[low]"  "\n\t"
    "out %[usicr], %[high]" "\n\t"
    "out %[usicr], %[low]"  "\n\t"
    "out %[usisr], 0"
    : /* no outputs */
    : /* only inputs */
    [data]  "r" (data),
    [usidr] "I" (_SFR_IO_ADDR(USIDR)),
    [usicr] "I" (_SFR_IO_ADDR(USICR)),
    [usisr] "I" (_SFR_IO_ADDR(USISR)),
    [high]  "r" ((1<<USIWM0)|(1<<USITC)),
    [low]   "r" ((1<<USIWM0)|(1<<USITC)|(1<<USICLK))
    // USICR = (1u << USIWM0) | (1u << USITC);
    // USICR = (1u << USIWM0) | (1u << USITC) | (1u << USICLK);
    : /* no clobbers */
    );
    //USISR = 0;
}

inline void max7219_init() {
    DDRB |= (1u << MAX7219_SCK) | (1u << MAX7219_DO) | (1u << MAX7219_CS);
    PORTB |= (1u << MAX7219_CS);    // CS to 1
    PORTB &= ~(1u << MAX7219_SCK);  // SCK to 0
    USISR = 0;
    max7219_send_command(MAX7219_DECODE_MODE, 0x00); // no Code B decoding
    max7219_send_command(MAX7219_SCAN_LIMIT, MAX7219_SCAN_LIMIT_7);
    max7219_send_command(MAX7219_DISPLAY_TEST, MAX7219_DISPLAY_TEST_OFF);
    max7219_send_command(MAX7219_SHUTDOWN, MAX7219_SHUTDOWN_OFF);
    max7219_send_command(MAX7219_INTENSITY, MAX7219_INTENSITY_MID);
}

inline void max7219_send_command(uint8_t address, uint8_t data) {
    PORTB &= ~(1u << MAX7219_CS);   // CS to 0
    send_byte(address);
    send_byte(data);
    PORTB |= (1u << MAX7219_CS);    // CS to 1
} 
