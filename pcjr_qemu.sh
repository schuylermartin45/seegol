#/bin/bash
##
## File:    pcjr_qemu.sh
##
## Author:  Schuyler Martin <sam8050@rit.edu>
##
## Description: Wrapper script that starts a QEMU VM that emulates the PCjr (as
##              closesly as possible) and loads SeeGOL via a floppy image
##

qemu-system-i386 -std-vga -fda bin/floppy.img -serial stdio
