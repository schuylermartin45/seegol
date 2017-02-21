##
## File:    boot.s
##
## Author:  Schuyler Martin <sam8050@rit.edu>
##
## Description: Boot loader
##

# force 16-bit assembly
.code16gcc

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

__floppy_boot_load:
    # Load the rest of the OS from the floppy disk
    # Most of this floppy code is adapted from my friends' Bobby Jr. Project:
    # https://github.com/csssuf/bobbyjunior/blob/master/kernel/src/mbr.s

    # initialize segment registers
    movw    %cs, %ax            # cs holds the segment where code is exec'ing
    movw    %ax, %ds            # (if we have segment registers start in the
    movw    %ax, %es            # same spot, all addresses should be similar)

    # stack memory set-up
    movw    $0, %ax
    movw    %ax, %ss
    movw    $0x7C00, %sp        # stack starts at bootloader and grows down
    movw    %sp, %bp            # bp and sp start at the same location

__floppy_reset:
    movw    $0,  %ax
    movb    $0,  %dl            # drive 0
    int     $0x13
    jc      __floppy_reset      # if failure (EFLAGS carry bit set), try again

__floppy_read:
    movw    $0x7E00, %bx        # load the OS after the bootloader

    movb    $2,  %ah            # load to ES:BX
    movb    $40, %al            # load N sectors (512-bytes each)
    movb    $0,  %ch            # cylinder 0
    movb    $2,  %cl            # sector 2
    movb    $0,  %dh            # head 0
    movb    $0,  %dl            # drive 0
    int     $0x13
    jc      __floppy_read       # if failure (EFLAGS carry bit set), try again

    movb    $'L',  %al          # L for loading (floppy loaded)
    movb    $0x0E, %ah
    int     $0x10

__boot_video_init:
    movb    $0x03, %al          # resets video mode; clears BIOS messages 
    movb    $0x00, %ah
    int     $0x10

    movb    $0x00, %al          # hides cursor 
    movb    $0x01, %ah
    movw    $0x2607, %cx
    int     $0x10

    call    main                # jump to the start of the kernel code

boot_extra:
    # Note to self:
    # Indicates extra space in the boot sector, in case I'm strapped for room
    # later on in the project. There will be about 470ish bytes to work with

__boot_sig:
    . = __boot + 510            # append boot signature at the end of the 512
    .byte 0x55                  # boot sector
    .byte 0xAA
