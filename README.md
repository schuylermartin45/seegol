# HSC: Shoyler's Extremely Experimental Graphical Operating Library - SeeGOL
#### Author:  Schuyler Martin <sam8050@rit.edu>

![alt tag](/media/hsc.png)


## About
SeeGOL (pronounced _sea-gull_) is my final graduate project. The end goal is to
develop a graphics library/windowing system to 16-bit x86 processors (i386 and
above).

### Original Plan
I originally wanted to develop a library for the IBM PCjr. That has proven to
be too difficult using modern tools. There is a branch, `pcjr_8086_compat`,
which includes my attempt at converting gcc's i386 assembly to only valid
instructions on the 8086.

#### Notes on the PCjr
The PCjr model I have has 128kb of RAM and a 3.5in floppy drive that can only
read the older "1mb" IBM floppy format (as opposed to the much more common
2.44mb format most people are probably familiar with).


## Setup

### Dependencies
These programs are necessary to build and install 
- dd
- make
- makedepend
TODO [more here]
An emulator (such as QEMU) may also be helpful in running the OS.

### Compiling from Source
To compile all of the x86 and C files and put them into a single flat binary:
```shell
make see_gol
```
To build the complete OS image, targetted to boot from a floppy:
```shell
make floppy.img
```
To remove all binaries and OS images:
```shell
make clean
```

### Running with QEMU
There is a provided bash script so that the OS can be easily run with QEMU
```shell
./tools/i386_qemu.sh
```

### Running with Physical Hardware
Assuming the floppy is mounted at `/dev/fd0` (which is common on Linux systems)
simply run:
```shell
make floppy
```
Otherwise dd the image file to the floppy:
```shell
dd if=/path/to/floppy.img of=/path/to/floppy 
```
After making any necessary changes to the boot order via a machine's BIOS
settings, SeeGOL should be bootable from this floppy on almost any Intel x86
computer. Modern machines that do not have a floppy drive can usually boot
See warning notes below for further information.

### Warnings
Obviously this project comes without any warranties. The software provided has
not been extensively tested across all possible hardware configurations.
Although unlikely, since this OS does interact directly with the hardware, it
could potentially damage it.

Run at your own risk.

#### Note on running this project on anything that isn't a PCjr
Although SeeGOL should run on any x86 system created after the PCjr (the x86
architecture is "backwards compatible to the dawn of time"), SeeGOL is not
guaranteed to run properly on anything other than the PCjr. The main concern is
that the PCjr uses the CGA graphics chip, as opposed to the much more common
VGA chip. Most modern graphics cards, on-processor graphics chips, and
emulators provide backwards-compatibility support for VGA. Although CGA and VGA
both come from IBM and work in similar ways, VGA is not a direct descendant of
CGA. Therefore the OS may not appear like it does on the PCjr across all
systems or it may not work at all.


## Directories
Here is a quick description of the top-level directories. Some of these
directories have their own READMEs.

### bin/
Stores binary objects (compiled code).

### dgb_bin/
Stores compile-time debugging information. These are files that list the source
code and compiled assembly instructions together with memory addresses.

### docs/
Extensive documentation and presentations on the project.

### img/
Stores the image files that can be dd'd to storage media and used to boot the
OS.

### media/
Contains media content not directly packaged into the OS.

### old/
Deprecated code that is kept around as reference material.

### src/
Source code directory for the project.

### tools/
Includes scripts and other helpful tools used to run and debug the OS.


## User Manual
TODO


## Design
TODO


## Rolling Status Log
This a higher-level discussion of the project status while in development.

### January 4th, 2017
- I decided to make the project public. I was going to wait until more had been
  completed but enough people have seemed interested in seeing what I had done
  that I figured I would publicly show my progress.
- I have booted SeeGOL on three platforms so far:
  - Modern x86 System (1st generation Core Series): Main function is reached
    but some functions defined outside of the main C file do not execute.
  - QEMU: Same behavior as with the physical x86 machine.
  - PCjr: Everything written in x86 assembly works. Nothing happens when the
    code written in C executes (more on that below).
- gcc does not compile to true 16-bit x86. Modern x86 systems will accept and
  run many 32-bit instructions in real mode despite not being in protected
  mode. To get around this, I think I may switch to using the "Amsterdam
  Compiler Kit" which claims it can compile to use only 16-bit instructions. If
  this does not work, I might be stuck writing some plugins for gcc.

### January 17, 2017
- I am (for now) giving up on targeting the PCjr as the primary platform for
  this project. I can not find or implement a reliable tool set that will
  compile C to 8086 assembly. I will now be targeting i386 processors instead.
  SeeGOL should be able run on almost any modern x86 machine in Real Mode.

### February 1, 2017
- I just solved a major bug with the kernel. It appears as though there is a
  difference between the `.code16` and `.code16gcc` assembly directives. The
  former causes strange stack behavior and my best guess is that gcc is not
  properly handling segmentation addressing based on the initialization
  process in the bootloader. I can't find any documentation online that
  describes the directives but it makes a clear difference. I can now write
  messages to text video. I plan to improve the I/O tools shortly and then
  move to starting my work with the graphics modules.

### February 13, 2017
- My graphics work from the Systems Programming class has been imported and
  significantly improved. Right now only Mode13h is supported but I have laid
  out the groundwork for other graphics modes. User programs (installed via the
  SeeSH shell interface) must access the graphics hardware through the Graphics
  Library (GL) package. This is actually enforced by design, unlike my work in
  Sys Prog.

### February 27, 2017
A few major bugs/realizations came out of this past weekend, while laying the
ground work for SeeGOL's image drawing capabilities.
- gcc's 16-bit compilation process does not use the segment registers. I
  previously thought that I was able to address 20 bits of memory (up to the
  A20 gate, 1mb barrier) but that is not the case. Since gcc does not manage
  the segment registers in Real Mode; I only have 64kb of memory to work with.
  I found this out the hard way while developing the image drawing code in the
  GL package, running out of memory while trying to import XPM files.
- XPM is a file format developed for X11 that stores image data as an indexed
  array of byte data. The file format is C code; you can simply include the
  file and to access an array of strings, each character byte representing a
  a value in a color look-up table. This would be an ideal image format for
  SeeGOL, as the image data can be baked into the OS, without the need of a
  reading image files...which would require a file system...that I don't have.
- However in my initial testing I found that the XPM file format is not space
  efficient...and ate up all my text memory. So I came up with a modified
  version of XPM: CXPM, Compressed X Pixel Map (the first proprietary element
  in SeeGOL that isn't prefixed with 'See'). To save space, the CXPM format is
  limited to less than 16 colors (each byte represents 2 pixels, identified
  by 4 bits) and then Huffman encoded in one dimension, per scanline. The
  run-length encoding greatly reduces the space used since so few colors are
  used. The full details of the spec are described currently in the `cxpm.py`
  file under `src/res/`. The spec may be further modified and I hope to write
  an OpenCV program that will quantize images and write them to the CXPM
  program, further automating the process. Currently I'm relying on Gimp and
  a Python script to convert color-quantized images to XPM and then to CXPM.
- One major (and painful to figure out) bug I ran into is that Python3 defaults
  to encoding strings in UTF-8. UTF-8 is a variable length encoding scheme and
  when I initially created CXPM files, it looked like text information was
  being saved correctly (confirmed through vim, less, cat, etc). However, when
  I compiled my code, I noticed that extra color information was being added to
  my test image, causing the images to "bleed out" of frame. Turns out that
  the UTF-8 encoding scheme was putting the extra bytes in my C strings and I
  was trying to interpret those bytes as color. To solve this issue, I switched
  the encoding scheme to 'latin-1', which is a fixed length 8-bit encoding
  scheme...so when I write 8 bits of bit-packed color information in Python,
  I get 8 bits and not occassionally 16 bits.
