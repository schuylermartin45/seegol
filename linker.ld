/*
** File:    boot_linker.ld
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Based on code from:
** http://dc0d32.blogspot.com/2010/06/real-mode-in-c-with-gcc-writing.html
**
** Description: Linker script to configure organization of boot sector using
**              the compiled ELF files
*/

ENTRY(main)
/* Link to a flat binary format */
OUTPUT_FORMAT(binary)

SECTIONS
{
    /*
    ** This is where the bootsector lives which has 510 bytes to play with.
    ** Initially this script placed the 2-byte boot loader signature at 0x7DFE
    ** (making the full 512 byte boot sector). But for whatever reason when I
    ** converted from the ELF format to a "flat" binary the boot signature
    ** would be placed in the first two bytes of the sector (0x0) and thus
    ** create an invalid boot sector (as well as pushing back the rest of the
    ** instructions back by 2 bytes) 
    */
    . = 0x7C00;
    .text : AT(0x7C00)
    {
        _TEXT_BEGIN = .;
        /* boot loader loads other sections of the OS into memory */
        bin/boot.o (.text);
        /*
        ** 0x7C00 + 512 = 0x7E00
        ** This is where we can work before the 1mb barrier (and where ever
        ** video memory starts for CGA systems)
        */
        *(.text);
        _TEXT_END = .;
    }
    .data : ALIGN(0x1000) {
        _DATA_BEGIN = .;
        *(.data);
        *(.rodata);
        _DATA_END = .;
    }
    .bss : ALIGN(0x1000) {
        _BSS_BEGIN = .;
        *(.COMMON);
        *(.bss);
        _BSS_END = .;
    }
    /* Save space. Memory is expensive in 1984. */
    /DISCARD/ :
    {
        *(.note*);
        *(.iplt*);
        *(.igot*);
        *(.rel*);
        *(.comment);
    }
}
