/*
** File:    pit.c
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Programmable Interval Timer driver (Intel 8254)
*/

/** Headers    **/
#include "gcc16.h"
#include "pit.h"

// TODO rm
#include "kio.h"

#include "asm_lib.h"

/** Macros     **/

// Programmable Interval Timer - Intel 8254
#define PIT_DEFAULT_TICKS_PER_SEC 18       // default ticks per second
#define PIT_DEFAULT_MS_PER_TICK (1000 / PIT_DEFAULT_TICKS_PER_SEC)
#define PIT_FREQUENCY      1193182         // clock cycles/sec
#define PIT_BASE_PORT      0x40            // I/O port for the timer
#define PIT_0_PORT         (PIT_BASE_PORT)
#define PIT_1_PORT         (PIT_BASE_PORT + 1)
#define PIT_2_PORT         (PIT_BASE_PORT + 2)
#define PIT_CTRL_PORT      (PIT_BASE_PORT + 3)
#define PIT_USE_BCD        0x01            // BCD counter
// Timer modes
#define PIT_MODE_0         0x00            // int on terminal count
#define PIT_MODE_1         0x02            // one-shot
#define PIT_MODE_2         0x04            // divide-by-N
#define PIT_MODE_3         0x06            // square-wave
#define PIT_MODE_4         0x08            // software strobe
#define PIT_MODE_5         0x0a            // hwd strobe
// Timer 0 settings
#define PIT_0_SELECT       0x00            // select timer 0
#define PIT_0_LOAD         0x30            // load LSB, then MSB
#define PIT_0_NDIV         PIT_MODE_2      // divide-by-N counter
#define PIT_0_SQUARE       PIT_MODE_3      // square-wave mode
#define PIT_0_ENDSIGNAL    0x00            // assert OUT at end of count

// Programmable Interrupt Controller - Intel 8259
#define PIC_MASTER_CMD_PORT 0x20           // master command port
#define PIC_SLAVE_CMD_PORT  0xA0           // slave command port
#define PIC_EOI             0x20           // Non-specific EOI command

/** Globals    **/
uint16_t pit_clock_value = 0;

/*
** Initializes the clock device
*/
void pit_init(void)
{
    __asm__ __volatile__("cli");
    // TODO rm
    kio_print("Test0\n");
    // calculate the divisor
    uint16_t divisor = PIT_FREQUENCY / CLOCK_SPEED;
    // load timer 0 and turn set to square wave
    _outb(PIT_CTRL_PORT, PIT_0_LOAD | PIT_0_SQUARE);
    // load divisor into timer 0
    _outb(PIT_0_PORT, divisor & 0xFF);
    _outb(PIT_0_PORT, (divisor >> 8) & 0xFF);
    // TODO rm
    kio_print("Test1\n");
    __asm__ __volatile__("sti");
}

/*
** PIT interrupt service routine, implementation in C
*/
void pit_isr_c(void)
{
    // increment the clock counter in software
    ++pit_clock_value;
    _outb(PIC_MASTER_CMD_PORT, PIC_EOI);
}
