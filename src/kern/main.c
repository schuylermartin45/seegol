/*
** File:    main.c
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Main execution point of the OS
*/

/** Headers    **/
#include "gcc16.h"
#include "kio.h"
#include "pit.h"
// TODO move pic stuff out of the main
#include "asm_lib.h"
#include "../usr/seesh.h"

// main welcome messages
#define MSG_KERN_LOAD   "Kernel Loaded...\n"
#define MSG_WELCOME     "Welcome to SeeGOL!\n"
#define MSG_KERN_EXIT   "< Kernel Exited >\n"

// Definitions for 8259 Programmable Interrupt Controller
#define PIC_NEEDICW4    0x01    // ICW4 needed
#define PIC_ICW1BASE    0x10    // base for ICW1
#define PIC_LTIM        0x08    // level-triggered mode
#define PIC_86MODE      0x01    // MCS 86 mode
#define PIC_AUTOEOI     0x02    // do auto eoi's
#define PIC_SLAVEBUF    0x08    // put slave in buffered mode
#define PIC_MASTERBUF   0x0C    // put master in buffered mode
#define PIC_SPFMODE     0x10    // special fully nested mode
#define PIC_READISR     0x0B    // Read the ISR
#define PIC_READIRR     0x0A    // Read the IRR
#define PIC_EOI         0x20    // Non-specific EOI command
#define PIC_SEOI        0x60    // specific EOI command
#define PIC_SEOI_LVL7   (PIC_SEOI | 0x7)    // specific EOI for level 7

/*
Interrupt configuration information specific to a particular computer.
These constants are used to initialize tables in modules/pic/space.c.
NOTE: The master pic must always be pic zero.
*/

#define NPIC    2   // 2 PICs
/*
Port addresses for the command port and interrupt mask register port
for both the master and slave PICs.
*/
#define PIC_MASTER_CMD_PORT     0x20    // master command
#define PIC_MASTER_IMR_PORT     0x21    // master intr mask register
#define PIC_SLAVE_CMD_PORT      0xA0    // slave command
#define PIC_SLAVE_IMR_PORT      0xA1    // slave intr mask register
#define PIC_MASTER_SLAVE_LINE   0x04    // bit mask: slave id
#define PIC_SLAVE_ID            0x02    // integer: slave id
#define PIC_BUFFERED            0       // PICs not in buffered mode

/*
** TODO expl
*/
static void pic_init(void)
{
    __asm__ __volatile__("cli");
    /*
    */
    // TODO
    // copy over
    /*
    */
    uint16_t* ivt_ptr = (uint16_t*)(0x1C);
    uint16_t* new_ivt_ptr = (uint16_t*)(31 * 4);
    // loop over, copying IRQ ISRs
    for (uint16_t i=1; i<=16; ++i)
    {
        *new_ivt_ptr++ = *ivt_ptr++;
        *new_ivt_ptr++ = *ivt_ptr++;
    }
    ivt_ptr = (uint16_t*)0x0;
    // TODO rm dump table
    for(uint16_t i=1; i<=48; ++i)
    {
        kio_printf("%d|%x,", &i, ivt_ptr);
        ++ivt_ptr;
        kio_printf("%x ", ivt_ptr, NULL);
        ++ivt_ptr;
    }
    // ICW1
    _outb(PIC_MASTER_CMD_PORT, PIC_ICW1BASE | PIC_NEEDICW4);
    _outb(PIC_SLAVE_CMD_PORT, PIC_ICW1BASE | PIC_NEEDICW4);

    // TODO
    // ICW2: master offset of 20 in the IDT, slave offset of 28
    _outb(PIC_MASTER_IMR_PORT, 32);
    _outb(PIC_SLAVE_IMR_PORT, 40);

    // ICW3: slave attached to line 2 of master, bit mask is 00000100
    // slave id is 2
    _outb(PIC_MASTER_IMR_PORT, PIC_MASTER_SLAVE_LINE);
    _outb(PIC_SLAVE_IMR_PORT, PIC_SLAVE_ID);

    // ICW4
    _outb(PIC_MASTER_IMR_PORT, PIC_86MODE);
    _outb(PIC_SLAVE_IMR_PORT, PIC_86MODE);

    // OCW1: allow interrupts on all lines
    uint8_t pic0_enable = _inb(PIC_MASTER_IMR_PORT);
    uint8_t pic1_enable = _inb(PIC_SLAVE_IMR_PORT);
    kio_printf("PIC PORTs: %B, %B\n", &pic0_enable, &pic1_enable);
    _outb(PIC_MASTER_IMR_PORT, 0x00);
    _outb(PIC_SLAVE_IMR_PORT, 0x00);
    __asm__ __volatile__("sti");
}

/*
** Main execution point of the OS
*/
void main(void)
{
    // Hardware initialization
    pic_init();
    pit_init();
    kio_printf("System Time: %d\n", &pit_clock_value, NULL);

    // kernel print messaging
    kio_print(MSG_KERN_LOAD);
    kio_print(MSG_WELCOME);

    // primary OS control loop
    while(true)
    {
        // call the seesh shell to run commands
        seesh_main();
        kio_printf("System clock value: %d\n", &pit_clock_value, NULL);
    }
    kio_print(MSG_KERN_EXIT);
}
