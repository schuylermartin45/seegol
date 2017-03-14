##
## File:    pit_as.s
##
## Author:  Schuyler Martin <sam8050@rit.edu>
##
## Description: Programmable Interval Timer driver (Intel 8254)
##

# force 16-bit assembly
.code16gcc

###############################################################################
##
## BEGIN Data Section
###############################################################################
.data
pit_clock_value:    .word   0x00

###############################################################################
##
## BEGIN Text Section
###############################################################################
.text

###############################################################################
##
## BEGIN Declare & export labels as C functions
###############################################################################
.globl pit_isr
.type  pit_isr, @function

.globl pit_clock_value

## ============================================================================
## x86 wrapper for PIT ISR code
##
## ============================================================================
pit_isr:
    pushf
    cli
    push %dx
    push %ax

    call pit_isr_c
    movw $42, pit_clock_value
    #incl pit_clock_value

    # TODO do this in the C code
    # send EOI signal to PIC
    movb $0x20, %al  # Send EOI to PIC (Details later)
    movw $0x20, %dx  # write to Master PIC port
    #movw $0xA0, %dx  # write to Slave PIC port
    outb %al, %dx

    pop %ax
    pop %dx
    sti
    popf
    iret
