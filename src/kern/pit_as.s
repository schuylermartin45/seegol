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
#pit_clock_value:    .word   0x00

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
    cli
    pusha
    push %ax

    #call pit_isr_c
    movw $42, pit_clock_value
    #incl pit_clock_value

    # TODO do this in the C code
    movb $0x20, %al  # Send EOI to PIC (Details later)
    outb %al, $0x20     #write to Master PIC port
    #outb %al, $0xA0     #write to Master PIC port

    pop %ax
    popa
    sti
    iret
