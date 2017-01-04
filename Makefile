##
## File:    Makefile
##
## Author:  Schuyler Martin <sam8050@rit.edu>
##
## Description: Defines make targets for the project
##

#
# Source and bin paths
#
BIN      = bin/
SRC	     = src/
KERN     = $(SRC)kern/
GL       = $(SRC)gl/
USR      = $(SRC)usr/

#
# Source file groupings
#
C_SRC_KERN = $(KERN)main.c \
             $(KERN)asm_lib.c \
             $(KERN)kio.c
C_SRC_GL   =
C_SRC_USR  =

#
# Source and object files
#

# All source files, with paths
C_SRC = $(C_SRC_KERN) $(C_SRC_GL) $(C_SRC_USR)
S_SRC = $(KERN)boot.s
# All source files with no paths
C_SRC_FILES = $(notdir $(C_SRC))
S_SRC_FILES = $(notdir $(S_SRC))
# Object files stored in bin/
C_OBJ = $(patsubst %.c,$(BIN)%.o,$(C_SRC_FILES))
S_OBJ = $(patsubst %.s,$(BIN)%.o,$(S_SRC_FILES))

SRCS  = $(C_SRC) $(S_SRC)
OBJS  = $(C_OBJ) $(S_OBJ)

##
## Compiler && Assembler control
##

#
# Add directories that have header files; for #include's
#
INCLUDES = -I. -I./$(SRC) -I./$(KERN) -I./$(GL) -I./$(USR)

#
# Compiler setup
#
CC = gcc
CFLAGS = -g -Os -march=i686 -m32 -ffreestanding -Wall -Werror \
		 -Wl,--oformat=binary $(INCLUDES)

#
# Assembler setup
#
AS = as
ASFLAGS = --32

#
# Linker setup
#
LD = ld
LDFLAGS = -melf_i386 -static -Tlinker.ld -nostdlib --nmagic

#
# Make compiling, assembling, and linking rules
# Notes:
#     - *.lst files contain helpful dumps of the assembly code
#     - TODO: ?? Files are compiled as ELF and copied to flat binaries
#       TODO: objcopy -O binary $@ $@
#

$(BIN)%.o: $(KERN)%.s
	##
	## MAKE: compile kern/
	##
	$(AS) $(ASFLAGS) -o $@ $< -a=$@.lst
$(BIN)%.o: $(KERN)%.c
	##
	## MAKE: compile kern/
	##
	$(CC) $(CFLAGS) -o $@ -c $< -Wa,-aln=$@.lst
$(BIN)%.o: $(GL)%.c
	##
	## MAKE: compile gl/
	##
	$(CC) $(CFLAGS) -o $@ -c $< -Wa,-aln=$@.lst
$(BIN)%.o: $(USR)%.c
	##
	## MAKE: compile usr/
	##
	$(CC) $(CFLAGS) -o $@ -c $< -Wa,-aln=$@.lst

#
# Compiling, assembling, and linking the project
# 1) Compile using file pattern rules
# 2) Link object files using a manual link script
# 3) Convert from ELF to single binary format
#
see_gol: depend linker.ld $(OBJS)
	##
	## MAKE: see_gol
	##
	$(LD) $(LDFLAGS) -o $(BIN)os.elf $(OBJS)
	objcopy -S -O binary $(BIN)os.elf $(BIN)os.b

#
# Targets for building a floppy image
# 1) dd zeros-out the whole floppy image
# 2) dd actually writes the complete flat binary to the floppy image
#
floppy.img: see_gol
	##
	## MAKE: floppy.img
	##
	dd if=/dev/zero of=$(BIN)floppy.img bs=1024 count=1440
	dd if=$(BIN)os.b of=$(BIN)floppy.img

#
# Targets for copying floppy image onto actual floppy
#
floppy: floppy.img
	##
	## MAKE: floppy
	##
	dd if=$(BIN)floppy.img of=/dev/fd0

#
# Clean out bin/ directory
#
clean:
	##
	## MAKE: clean
	##
	rm -f $(BIN)*

#
# makedepend is a program which creates dependency lists by
# looking at the #include lines in the source files
#
# Backup created by makedepend (Makefile.bak) is removed because we have git
#
# Note on sed command:
# 
# This sed command will fix the object file pathings generated by
# makedepend. This should not change any other part of the Makefile because
# the paths are stored in string vars.
#
# "Brian is not the messiah. He's a very naughty boy!"
#
depend:
	##
	## MAKE: depend
	##
	makedepend -p$(BIN) $(INCLUDES) $(C_SRC)
	sed -i "s#$(BIN)$(SRC)[a-z]\+/#$(BIN)#g" Makefile
	rm Makefile.bak

# DO NOT DELETE

bin/main.o: src/kern/gcc16.h src/kern/debug.h src/kern/kio.h
bin/main.o: src/kern/asm_lib.h src/kern/types.h
bin/asm_lib.o: src/kern/gcc16.h src/kern/asm_lib.h
bin/kio.o: src/kern/gcc16.h src/kern/kio.h src/kern/asm_lib.h