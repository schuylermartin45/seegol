/*
** File:    boot_linker.ld
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Based on code from:
** http://dc0d32.blogspot.com/2010/06/real-mode-in-c-with-gcc-writing.html
**
** Description: Linker script to configure organization of boot sector
*/

ENTRY(main);
SECTIONS
{ 
    . = 0x7C00;
    .text : AT(0x7C00)
    {
        *(.text);
    }
    .sig : AT(0x7DFE)
    {
        SHORT(0xaa55);
    }
    .eh_frame : AT(0x7E00)
    {
        *(.eh_frame);
    }
}


/* TODO, was working
ENTRY(main);
SECTIONS
{
    . = 0x7C00;
    .text : AT(0x7C00)
    {
        _text = .;
        *(.text);
        _text_end = .;
    }
    .data :
    {
        _data = .;
        *(.bss);
        *(.bss*);
        *(.data);
        *(.rodata*);
        *(COMMON)
        _data_end = .;
    }
    .sig : AT(0x7DFE)
    {
        SHORT(0xaa55);
    }
    .eh_frame : AT(7E00)
    {
        0x7E00 = 0x7C00 + 0x200
    }
    /DISCARD/ :
    {
        *(.note*);
        *(.iplt*);
        *(.igot*);
        *(.rel*);
        *(.comment);
    
    TODO Add any unwanted sections spewed out by gcc and flags here
    }
}
*/
