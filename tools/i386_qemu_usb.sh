#/bin/bash
##
## File:    i386_qemu_usb.sh
##
## Author:  Schuyler Martin <sam8050@rit.edu>
##
## Description: Wrapper script that starts a QEMU VM that emulates an i386
##              computer and loads SeeGOL via a usb image.
##

# -i386 is the closest we can get to a pure 8080 or 8088 emulation
# -vga std Default to VGA standard
# -hda  loads a HDD/USB disk image
# -boot sets the initial boot device
qemu-system-i386 -vga std -hda img/usb.img -boot a
