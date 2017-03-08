##
## File:    pit_as.s
##
## Author:  Schuyler Martin <sam8050@rit.edu>
##
## Description: Programmable Interval Timer driver (Intel 8254)
##

###############################################################################
##
## BEGIN Data Section
###############################################################################
.data

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

.globl pit_isr_c

## ============================================================================
## x86 wrapper for PIT ISR code
##
## ============================================================================
pit_isr:
    pusha
    cld
    call pit_isr_c
    popa
    iret
