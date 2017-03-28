#/bin/bash
##
## File:    i386_qemu_floppy.sh
##
## Author:  Schuyler Martin <sam8050@rit.edu>
##
## Description: Wrapper script that starts a QEMU VM that emulates an i386
##              computer and loads SeeGOL via a floppy image
##

# -i386 is the closest we can get to a pure 8080 or 8088 emulation
# -vga std Default to VGA standard
# -rtc sets the real time clock
# -fda  loads a floppy disk image
# -boot sets the initial boot device
qemu-system-i386 -vga std -rtc base=localtime,clock=host \
    -fda img/floppy.img -boot a
