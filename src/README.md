# HSC: Shoyler's Extremely Experimental Graphical Operating Library - SeeGOL
#### Author:  Schuyler Martin <sam8050@rit.edu>

# src/
Source Code Directory

## Style
The project follows a fairly rigid convention, for a school project.

### General rules
* There are no tabs (except in the Makefile(?)). Only 4 spaces.
* Everything follows the 80 character line limit. Code that exceeds that is
  entered on a new line and tabbed in. Sometimes it is made to look pretty.
* Variables that are of equal character length make me hot.
  https://xkcd.com/276/
* `*` is touching the type to define a pointer. Ex: `char* str;`
* `*` is touching the variable to dereference the pointer. Ex: `*ptr = 42;`
* No parentheses around `return` statements.
* `if` and `switch` statements always use parentheses. There exists a space
  between the keyword and the left parenth. Ex: `if (true)`
* Composed boolean statements have parentheses around each clause.
  Blame my PASCAL days for this one. Ex: `((x && y) || (w && z))`
* Loop statements always use parentheses. No space exists between the keyword
  and the left parenth. Ex: `while(true)`

### Macros and Variables Naming
* Macros are in all upper case, separated by `_` to denote new words.
* Variables are in all lower case, separated by `_` to denote new words.
* All primative data types are specified using aliases defined in `type.h`.
  These include the size of the variable in the name

### Function Naming
* `[protection]file_func_name()`
* `__` indicates a private function. These functions are generally labeled with
  `static` and not included in the header files. They should not be accessible
  from outside this file.
* `_` indicates a protected function. These functions should have limited
  access (i.e. can only be used by one other function). There is generally no
  better way to manage these functions other than by following this convention.

## src/kern/
Kernel, bootloader, and other key project files. Also includes the KIO package
which, at the time of writing, is the only library that handles console I/O.

## src/gl/
Graphics library files. These provide user-level functions to make GUIs and
other graphical components.

## src/res/
Resource files, non-code (i.e. artwork, images, assets). Ownership infomation
can be found in the src/res/README.md file.

## src/usr/
User level code (i.e. programs) including the `SeeSH` the first and only shell
available shell written for SeeGOL.

SeeSH is aware of all programs "installed" on the system and comes with a few
built-in commands. To install a program, a user program must follow the
convention and structure laid out in the `program.h` file. Look at `hsc_tp.*`
and `seesh.c` for an example on how to write and integrate a program into
SeeSH. As of now, a program is manually "installed" by including the program's
header file in `seesh.c`, adding that program's init function to the program
list of function pointers, and incrementing the `PROG_COUNT` macro that tracks
the number of programs installed.
