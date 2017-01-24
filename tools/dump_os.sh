#/bin/bash
##
## File:    dump_os.sh
##
## Author:  Schuyler Martin <sam8050@rit.edu>
##
## Description: Script that dumps flat OS binary (os.b) to console
##              Dumping just the ELF file (os.b) is simply:
##                  'objectdump -S bin/os.elf'
##

# -b binary specifies a flat binary format
# -m i386 is the closest we can get to a pure 8080 or 8088 emulation
# --adjust-vma TODO is a flag that was suggested w/o explination online
# -D for dissassembly
objdump -b binary -m i386 --adjust-vma=0xabcd1000 -D bin/os.b
