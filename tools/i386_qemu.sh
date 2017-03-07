#/bin/bash
##
## File:    i386_qemu.sh
##
## Author:  Schuyler Martin <sam8050@rit.edu>
##
## Description: Wrapper script that starts a QEMU VM that emulates the PCjr (as
##              closesly as possible) and loads SeeGOL via a floppy image
##

# -i386 is the closest we can get to a pure 8080 or 8088 emulation
# -fda  loads a floppy disk image
# -boot sets the initial boot device
qemu-system-i386 -vga std -fda img/floppy.img -boot a
