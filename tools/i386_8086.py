#!/usr/bin/python3
##
## File:    i386_8086.py
##
## Author:  Schuyler Martin <sam8050@rit.edu>
##
## Description: Script that will convert 32bit instructions emitted by gcc's
##              Real Mode compilation process into true 16bit instructions
##

# Python libraries
import re
import sys

#### GLOBALS    ####
USAGE = "./i386_8086.py in_file [out_file]"

# comment used to indicate a change/insertion from the script
CHG_COMMENT = "    # Code changed by i386_8086.py"
INS_COMMENT = "    # Code inserted by i386_8086.py"

# register mappings
REG_TBL = {
    '%eax' : '%ax',
    '%ebx' : '%bx',
    '%ecx' : '%cx',
    '%edx' : '%dx',
    '%esi' : '%si',
    '%edi' : '%di',
    '%esp' : '%sp',
    '%ebp' : '%bp',
}

# instruction mappings
INS_TBL = {
    'movl'   :   'movw',
    'movsl'  :   'movsw',
    'movsbl' :   'movsbw',
    'leal'   :   'leaw',
    'pushl'  :   'pushw',
    'popl'   :   'popw',
    'pushal' :   'pushaw',
    'popal'  :   'popaw',
    'pushfl' :   'pushfw',
    'popfl'  :   'popfw',
    'incl'   :   'incw',
    'decl'   :   'decw',
    'addl'   :   'addw',
    'subl'   :   'subw',
    'imull'  :   'imulw',
    'mull'   :   'mulw',
    'idivl'  :   'idivw',
    'divl'   :   'divw',
    'andl'   :   'andw',
    'orl'    :   'orw',
    'xorl'   :   'xorw',
    'cmpl'   :   'cmpw',
    'testl'  :   'testw',
}

#### MAIN       ####

def main():
    '''
    Main execution point of the program
    '''
    # usage check
    argc = len(sys.argv)
    if ((argc < 2) and (argc > 3)):
        print(USAGE, file=sys.stderr)
        exit(1)

    # original assembly code, at various stages/passes
    asmi386_p1 = ""
    asmi386_p2 = ""
    # new assembly code
    asm8086 = ""
    # read the original file
    for line in open(sys.argv[1]):
        asmi386_p1 += line
    # first pass: handle the inability to scale 16bit registers
    # by adding additional instructions
    for line in asmi386_p1.splitlines():
        # if a scale pattern is found, surround the instruction with equivalent
        # operations
        match = re.search(r'(|-)([0-9]+)\((%[a-z]+)\)', line)
        if (match):
            # take the line apart into the separte pieces we want
            whtspc = re.match(r'\s*', line).group(0)
            val = match.group(1) + match.group(2)
            reg = match.group(3)
            new_op = re.sub('(|-)([0-9]+)\((%[a-z]+)\)', reg, line)
            asmi386_p2 += (whtspc + "imul    $" + val + ", " + reg + ", " + reg
                + INS_COMMENT +"\n")
            asmi386_p2 += new_op + CHG_COMMENT + "\n"
            # TODO DIV only has
            asmi386_p2 += (whtspc + "idiv    " + reg + INS_COMMENT +"\n")
        else:
            asmi386_p2 += line + "\n"
    # second pass: change registers and instructions
    for line in asmi386_p2.splitlines():
        tokens = re.split(r'(\s+)', line)
        for tok in tokens:
            # skip just whitespace-only tokens args
            if (tok.isspace()):
                asm8086 += tok
                continue
            # instructions
            if (tok in INS_TBL):
                asm8086 += INS_TBL[tok]
                continue
            # check registers
            for reg in REG_TBL:
                if (reg in tok):
                    asm8086 += tok.replace(reg, REG_TBL[reg])
                    break
            else:
                asm8086 += tok
        asm8086 += "\n"
    if (argc == 3):
        # write to a file
        fptr = open(sys.argv[2], 'w')
        for line in asm8086:
            fptr.write(line)
        fptr.close()
    else:
        # print the new assembly to STDOUT
        print(asm8086, end="", flush=True)

if __name__ == "__main__":
    main()
