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
DBG_BIN  = dbg_bin/
IMG      = img/
SRC	     = src/
KERN     = $(SRC)kern/
VGA      = $(KERN)vga/
GL       = $(SRC)gl/
RES      = $(SRC)res/
IMG_ORIG = $(RES)img_original/
IMG_XPM  = $(RES)img_xpm/
IMG_CXPM = $(RES)img_cxpm/
USR      = $(SRC)usr/

#
# Source file groupings; automatically add every C file in each directory
#
C_SRC_KERN = $(shell find $(KERN) -name '*.c')
C_SRC_GL   = $(shell find $(GL)   -name '*.c')
C_SRC_USR  = $(shell find $(USR)  -name '*.c')

#
# Image files
#
IMG_SRC_ORIG = $(shell find $(IMG_ORIG) -name '*.*')
# explicitly list image file types
IMG_SRC_OFT0 = $(IMG_SRC_ORIG:.png=.xpm)
IMG_SRC_OFT1 = $(IMG_SRC_OFT0:.jpg=.xpm)
IMG_SRC_XPM  = $(subst $(IMG_ORIG),$(IMG_XPM),$(IMG_SRC_OFT1))
IMG_SRC_XFT0 = $(IMG_SRC_XPM:.xpm=.cxpm)
IMG_SRC_CXPM = $(subst $(IMG_XPM),$(IMG_CXPM),$(IMG_SRC_XFT0))

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
INCLUDES = -I. -I./$(SRC) -I./$(KERN) -I.$(VGA) -I./$(GL) -I./$(USR)

#
# Compiler setup
# Note: we prepend all C files with a directive to target Real Mode
#
# C Compiler optimizations from:
# http://ptspts.blogspot.com/2013/12/how-to-make-smaller-c-and-c-binaries.html
#
# Flags that seem to make no difference:
# --------------------------------------
# gcc:
#   -Wl,-z,norelro
#   -Wl,--hash-style=gnu
# ld:
#   -z norelro
#
# Enabling this flag reduces binary size but does not allow the OS to boot on
# SOME systems. It will boot in the Distributed Systems Lab but not on my
# personal test machine.
#   -fomit-frame-pointer
#
CC = gcc
CFLAGS = -Os -s -march=i686 -m32 -std=c99 -ffreestanding -Wall -Werror \
		 -fno-stack-protector -ffunction-sections \
		 -fno-unwind-tables -fno-asynchronous-unwind-tables \
		 -falign-functions=1 -falign-jumps=1 -falign-loops=1 \
		 -fdata-sections -Wl,--gc-sections -mpreferred-stack-boundary=2 \
		 -fmerge-all-constants -fno-ident -fno-pie\
		 -Wno-trigraphs -Wl,--oformat=binary $(INCLUDES)

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
# Image conversion scripts setup
#
SCRIPT_XPM  = $(RES)xpm_convert.sh
SCRIPT_CXPM = $(RES)cxpm.py

#
# Make compiling, assembling, and linking rules
# Notes:
#     - *.lst files contain helpful dumps of the assembly code
#     - Files are compiled as an ELF and copied to flat binaries using objcopy
#
$(BIN)%.o: $(KERN)%.s
	## MAKE: assemble kern/
	$(AS) $(ASFLAGS) -o $@ $< -aln=dbg_$@.lst
$(BIN)%.o: $(KERN)%.c
	## MAKE: compile kern/
	$(CC) $(CFLAGS) -o $@ -c $< -Wa,-aln=dbg_$@.lst
$(BIN)%.o: $(VGA)%.c
	## MAKE: compile kern/vga/
	$(CC) $(CFLAGS) -o $@ -c $< -Wa,-aln=dbg_$@.lst
$(BIN)%.o: $(GL)%.c
	## MAKE: compile gl/
	$(CC) $(CFLAGS) -o $@ -c $< -Wa,-aln=dbg_$@.lst
$(BIN)%.o: $(USR)%.c
	## MAKE: compile usr/
	$(CC) $(CFLAGS) -o $@ -c $< -Wa,-aln=dbg_$@.lst

#
# Rules for manipulating and compressing images to be included in the OS
#
$(IMG_CXPM)%.cxpm: $(IMG_ORIG)%.* $(SCRIPT_XPM) $(SCRIPT_CXPM) 
	$(SCRIPT_XPM) $<
	$(SCRIPT_CXPM) $(patsubst $(IMG_CXPM)%.cxpm,$(IMG_XPM)%.xpm,$@)

#
# Compiling, assembling, and linking the project
# 1) Compile using file pattern rules
# 2) Link object files using a manual link script to a flat binary
#
see_gol: build_depends res_img depend linker.ld $(OBJS)
	## MAKE: see_gol
	$(LD) $(LDFLAGS) -o $(BIN)os.b $(OBJS)

PYTHON_3   := $(shell command -v python3 2> /dev/null)
PYTHON_3.5 := $(shell command -v python3.5 2> /dev/null)
MK_DEPEND  := $(shell command -v makedepend 2> /dev/null)
IMG_MAGICK := $(shell command -v convert 2> /dev/null)
QEMU_i386  := $(shell command -v qemu-system-i386 2> /dev/null)
build_depends:
ifndef PYTHON_3
    $(error "Python 3 is missing. It is required to compress image data.")
endif
ifndef IMG_MAGICK
    $(error "ImageMagick is missing. It is required to compress image data.")
endif
ifndef MK_DEPEND
    $(error "makedepend is missing. It is required to build SeeGOL.")
endif
ifndef PYTHON_3.5
    $(warning "Python 3.5 is missing. <3.5 may fail to compress image data.")
endif
ifndef QEMU_i386
    $(warning "QEMU is missing. QEMU provides an easy way to run SeeGOL.")
endif

#
# Converting OS resource images directive (images drawn by the OS)
# If the image conversion scripts are changed, re-run image creation
#
res_img: $(IMG_SRC_CXPM)

#
# Targets for building a floppy image
# 1) dd zeros-out the whole image
# 2) dd actually writes the complete flat binary to the floppy image
#
floppy.img: see_gol
	## MAKE: floppy.img
	dd if=/dev/zero of=$(IMG)floppy.img bs=512 count=2880
	dd if=$(BIN)os.b of=$(IMG)floppy.img

#
# QEMU emulation shorthand
#
floppy_qemu: floppy.img
	./tools/i386_qemu_floppy.sh

#
# Targets for copying floppy image onto actual floppy
#
floppy: floppy.img
	## MAKE: floppy (targets /dev/fd0)
	dd if=$(IMG)floppy.img of=/dev/fd0
	sync

#
# Targets for building a USB image
# 1) dd zeros-out the whole image
# 2) dd actually writes the complete flat binary to the USB image
#
usb.img: see_gol
	## MAKE: usb.img
	dd if=/dev/zero of=$(IMG)usb.img bs=512 count=2880
	dd if=$(BIN)os.b of=$(IMG)usb.img

#
# QEMU emulation shorthand
#
usb_qemu: usb.img
	./tools/i386_qemu_usb.sh

#
# Targets for copying floppy image onto actual floppy
#
usb: usb.img
	## MAKE: usb (targets /dev/sdj; default for my desktop)
	## I'd prefer something safer than assuming a particular mount point
	## but this still requires root to run, so I'm ok with that.
	dd if=$(IMG)usb.img of=/dev/sdj
	sync

#
# Clean out bin/ directory
#
clean:
	## MAKE: clean
	rm -f $(BIN)*
	rm -f $(DBG_BIN)*
	rm -f $(IMG)*
	rm -f $(IMG_XPM)*
	rm -f $(IMG_CXPM)*

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
	## MAKE: depend
	makedepend -p$(BIN) $(INCLUDES) $(C_SRC)
	sed -i "s#$(BIN)$(SRC)[a-z]\+/#$(BIN)#g" Makefile
	rm Makefile.bak

# DO NOT DELETE

bin/clock.o: src/kern/gcc16.h src/kern/clock.h src/kern/types.h
bin/clock.o: src/kern/asm_lib.h src/kern/kio.h
bin/vga/vga13.o: src/kern/gcc16.h src/kern/asm_lib.h
bin/vga/vga13.o: src/kern/gcc16.h src/kern/types.h
bin/vga/vga13.o: src/kern/vga/vga13.h src/kern/types.h
bin/vga/vga13.o: src/kern/vga/vga.h
bin/vga/vga.o: src/kern/gcc16.h src/kern/vga/vga.h src/kern/types.h
bin/asm_lib.o: src/kern/gcc16.h src/kern/asm_lib.h src/kern/types.h
bin/main.o: src/kern/gcc16.h src/kern/asm_lib.h src/kern/types.h
bin/main.o: src/kern/clock.h src/kern/kio.h src/usr/seesh.h
bin/main.o: src/kern/gcc16.h src/kern/types.h
bin/rng.o: src/kern/gcc16.h src/kern/rng.h src/kern/types.h
bin/rng.o: src/kern/clock.h
bin/kio.o: src/kern/gcc16.h src/kern/kio.h src/kern/types.h
bin/pane.o: src/kern/gcc16.h src/gl/pane.h src/kern/types.h
bin/pane.o: src/kern/vga/vga.h src/kern/gcc16.h src/kern/types.h
bin/pane.o: src/gl/gl_lib.h src/kern/kio.h src/kern/gcc16.h
bin/pane.o: src/kern/types.h src/gl/gl_lib.h
bin/see_font.o: src/kern/gcc16.h src/gl/see_font.h src/kern/types.h
bin/gl_lib.o: src/kern/gcc16.h src/gl/gl_lib.h src/kern/debug.h
bin/gl_lib.o: src/kern/gcc16.h src/kern/kio.h src/kern/types.h
bin/gl_lib.o: src/kern/kio.h src/kern/vga/vga13.h src/kern/gcc16.h
bin/gl_lib.o: src/kern/types.h src/kern/vga/vga.h src/gl/img_tbl.h
bin/gl_lib.o: src/gl/img_fids.h src/res/img_cxpm/hsc_logo.cxpm
bin/gl_lib.o: src/res/img_cxpm/dark_side_of_the_moon.cxpm
bin/gl_lib.o: src/gl/see_font.h src/kern/types.h
bin/slidedeck.o: src/kern/gcc16.h src/usr/slidedeck.h
bin/slidedeck.o: src/kern/types.h src/usr/program.h src/kern/kio.h
bin/slidedeck.o: src/kern/gcc16.h src/kern/types.h src/gl/img_fids.h
bin/slidedeck.o: src/gl/pane.h src/kern/vga/vga.h src/kern/gcc16.h
bin/slidedeck.o: src/kern/types.h src/gl/gl_lib.h
bin/usr_clock.o: src/kern/gcc16.h src/usr/usr_clock.h
bin/usr_clock.o: src/kern/types.h src/usr/program.h src/kern/clock.h
bin/usr_clock.o: src/kern/gcc16.h src/kern/types.h src/kern/kio.h
bin/usr_clock.o: src/gl/gl_lib.h src/kern/vga/vga.h src/kern/gcc16.h
bin/usr_clock.o: src/kern/types.h src/gl/img_fids.h
bin/seesh.o: src/kern/gcc16.h src/usr/seesh.h src/kern/types.h
bin/seesh.o: src/kern/clock.h src/kern/gcc16.h src/kern/types.h
bin/seesh.o: src/kern/kio.h src/gl/gl_lib.h src/kern/vga/vga.h
bin/seesh.o: src/kern/gcc16.h src/kern/types.h src/gl/img_fids.h
bin/seesh.o: src/gl/pane.h src/usr/program.h src/usr/hsc_tp.h
bin/seesh.o: src/usr/slidedeck.h src/usr/slideshow.h
bin/seesh.o: src/usr/trench_run.h src/usr/usr_clock.h
bin/trench_run.o: src/kern/gcc16.h src/usr/trench_run.h
bin/trench_run.o: src/kern/types.h src/usr/program.h src/kern/clock.h
bin/trench_run.o: src/kern/gcc16.h src/kern/types.h src/kern/kio.h
bin/trench_run.o: src/kern/rng.h src/gl/gl_lib.h src/kern/vga/vga.h
bin/trench_run.o: src/kern/gcc16.h src/kern/types.h src/gl/pane.h
bin/hsc_tp.o: src/kern/gcc16.h src/usr/hsc_tp.h src/kern/types.h
bin/hsc_tp.o: src/usr/program.h src/kern/kio.h src/kern/gcc16.h
bin/hsc_tp.o: src/kern/types.h src/gl/gl_lib.h src/kern/vga/vga.h
bin/hsc_tp.o: src/kern/gcc16.h src/kern/types.h src/gl/img_fids.h
bin/hsc_tp.o: src/gl/pane.h
bin/slideshow.o: src/kern/gcc16.h src/usr/slideshow.h
bin/slideshow.o: src/kern/types.h src/usr/program.h src/kern/kio.h
bin/slideshow.o: src/kern/gcc16.h src/kern/types.h src/gl/gl_lib.h
bin/slideshow.o: src/kern/vga/vga.h src/kern/gcc16.h src/kern/types.h
bin/slideshow.o: src/gl/img_fids.h
