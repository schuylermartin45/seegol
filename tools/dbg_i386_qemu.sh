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
# -s    opens port 1234 for gdb debugging: (gdb) target remote localhost:1234
# -fda  loads a floppy disk image
# -boot sets the initial boot device
qemu-system-i386 -s -fda bin/floppy.img -boot a
