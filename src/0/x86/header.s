##
## File:    TODO.s
##
## Author:  Schuyler Martin <sam8050@rit.edu>
##
## Description: TODO
##

###############################################################################
##
## BEGIN Data Section
###############################################################################
.data

###############################################################################
##
## BEGIN Text Section
###############################################################################
.text

###############################################################################
##
## BEGIN Declare & export labels as C functions
###############################################################################
.globl TODO_func
.type TODO_func, @function

## ============================================================================
## C Function: TODO [C-function header]
##
## TODO expl
##
## @param name expl
## @return
## ============================================================================
TODO_func:
    pushq   %rbp                # stack set-up
    movq    %rsp,       %rbp
    movq    $0,         %rax    # zero-out/clear the return register


    popq    %rbp                # stack tear-down and return
    ret
