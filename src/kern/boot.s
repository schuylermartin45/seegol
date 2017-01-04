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

# extra space in the boot sector, in case I'm strapped for room later
.globl boot_extra

__boot:

    movb    $'B',  %al          # print via interrupt to test bootloader
    movb    $0x0E, %ah          # (BL for "BootLoader"; B for booting)
    int     $0x10

    # TODO init graphics mode

__floppy_boot_load:
    # Load the rest of the OS from the floppy disk
    # Most of this floppy code is adapted from my friends' Bobby Jr. Project:
    # https://github.com/csssuf/bobbyjunior/blob/master/kernel/src/mbr.s

    movw    %cs, %ax            # initialize segment registers
    movw    %ax, %ds
    movw    %ax, %es
    movw    $0,  %ax
    movw    %ax, %ss
    movw    $0x7C00, %sp        # stack lives at 0x7C00; before bootloader

__floppy_reset:
    movw    $0,  %ax
    movb    $0,  %dl            # drive 0
    int     $0x13
    jc      __floppy_reset      # if failure (EFLAGS carry bit set), try again

__floppy_read:
    movw    $0x7E00, %bx        # load the OS after the bootloader

    movb    $2,  %ah            # load to ES:BX
    movb    $8,  %al            # load 8 sectors
    movb    $0,  %ch            # cylinder 0
    movb    $2,  %cl            # sector 2
    movb    $0,  %dh            # head 0
    movb    $0,  %dl            # drive 0
    int     $0x13
    jc      __floppy_read       # if failure (EFLAGS carry bit set), try again

    movb    $'L',  %al          # L for loading (floppy loaded)
    movb    $0x0E, %ah
    int     $0x10

    jmp     main                # jump to the start of the kernel code

boot_extra:
    # Note to self:
    # Indicates extra space in the boot sector, in case I'm strapped for room
    # later on in the project. There will be about 470ish bytes to work with

__boot_sig:
    . = __boot + 510            # append boot signature at the end of the 512
    .byte 0x55                  # boot sector
    .byte 0xAA
