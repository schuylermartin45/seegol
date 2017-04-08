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

__drive_boot_load:
    # Load the rest of the OS from the "floppy" disk (legacy boot process)
    # Most of this floppy code is adapted from my friends' Bobby Jr. Project:
    # https://github.com/csssuf/bobbyjunior/blob/master/kernel/src/mbr.s

    # ***Note about INT 13h and %dl***
    # All INT 13h instructions use %dl to identify the drive to interact with.
    # Although I can't find a resource that confirms this, every BIOS (emulated
    # and physical) I've found will set %dl with the correct drive location
    # when the boot sector is loaded. This bootloader assumes that this is the
    # case so SeeGOL can be booted from a floppy or a USB drive. There is an
    # INT 13h function (%ah = 8) that will probe the BIOS and return various
    # statistics about the boot drive (legacy floppy) "geometry".
    # %dl values:
    #   0x00 - 1st floppy disk ("drive A:")
    #   0x01 - 2nd floppy disk ("drive B:")
    #   0x80 - 1st hard disk (and in my experience, USB devices)
    #   0x81 - 2nd hard disk

    # initialize segment registers
    movw    %cs, %ax            # cs holds the segment where code is exec'ing
    movw    %ax, %ds            # (if we have segment registers start in the
    movw    %ax, %es            # same spot, all addresses should be similar)

    # stack memory set-up
    movw    $0, %ax
    movw    %ax, %ss
    movw    $0x7C00, %sp        # stack starts at bootloader and grows down
    movw    %sp, %bp            # bp and sp start at the same location

__drive_reset:
    movw    $0,  %ax
    int     $0x13
    jc      __drive_reset       # if failure (EFLAGS carry bit set), try again

__drive_read:
    movw    $0x7E00, %bx        # load the OS after the bootloader

    movb    $2,  %ah            # INT13h read function 
    movb    $64, %al            # load 64 sectors (512-bytes each) all at once
                                # this should be than enough sectors the OS
                                # at time of writing is about ~46 sectors long
    movb    $0,  %ch            # cylinder 0
    movb    $2,  %cl            # sector 2
    movb    $0,  %dh            # head 0
    int     $0x13               # INT 13h will set the EFLAGS carry bit to
                                # indicate a failure. However, this gets set if
                                # we over-estimate the number of sectors we ask
                                # the BIOS chip to read in. So as a hack, we
                                # ignore the error checking here. The user will
                                # quickly realize if there's a failure b/c
                                # SeeGOL won't load properly...if at all.
                                # 

    movb    $'L',  %al          # L for loading (floppy loaded)
    movb    $0x0E, %ah
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
