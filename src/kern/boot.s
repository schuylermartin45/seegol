##
## File:    boot.s
##
## Author:  Schuyler Martin <sam8050@rit.edu>
##
## Description: Boot loader
##

# force 16-bit assembly
.code16

###############################################################################
##
## BEGIN Text Section
###############################################################################
.text

# reference to the main program code
.globl main
.type main, @function

.globl __boot;

__boot:

    movb    $'B',  %al          # print chars via interrupt to test bootloader
    movb    $0x0E, %al
    int     $0x10
    movb    $'L',  %al
    movb    $0x0E, %al
    int     $0x10

    # TODO load the rest of the OS from the floppy disk

    jmpl    $0x0000, $main      # jump to main program section

    # TODO init graphics mode

    . = __boot + 510             # append boot signature
    .byte 0x55
    .byte 0xAA
