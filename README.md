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
