/*
** File:    see_font.h
**
** Author:  Schuyler Martin <sam8050@rit.edu>
**
** Description: Defines a basic font for VGA graphical modes.
**              "Shoyler's Extremely Experimental Font"
*/

/** Headers    **/
#include "../kern/gcc16.h"
#include "see_font.h"

// table of character bit maps
const uint8_t see_font_tbl[SEE_FONT_NUM_CH][SEE_FONT_HEIGHT] =
{
    // 000: NULL
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 001: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 002: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 003: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 004: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 005: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 006: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 007: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 008: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 009: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 010: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 011: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 012: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 013: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 014: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 015: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 016: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 017: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 018: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 019: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 020: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 021: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 022: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 023: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 024: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 025: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 026: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 027: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 028: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 029: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 030: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 031: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 032: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 033: !
    {
        0b00011000,
        0b00011000,
        0b00011000,
        0b00011000,
        0b00011000,
        0b00000000,
        0b00011000,
        0b00011000,
    },
    // 034: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 035: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 036: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 037: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 038: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 039: '
    {
        0b00010000,
        0b00010000,
        0b00010000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 040: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 041: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 042: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 043: +
    {
        0b00000000,
        0b00000000,
        0b00010000,
        0b00010000,
        0b01111110,
        0b00010000,
        0b00010000,
        0b00000000,
    },
    // 044: ,
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00011000,
        0b00011000,
        0b00001000,
    },
    // 045: -
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b01111110,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 046: .
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00011000,
        0b00011000,
    },
    // 047: /
    {
        0b00000001,
        0b00000010,
        0b00000100,
        0b00001000,
        0b00010000,
        0b00100000,
        0b01000000,
        0b10000000,
    },
    // 048: 0
    {
        0b00111100,
        0b01000010,
        0b01000110,
        0b01001010,
        0b01010010,
        0b01100010,
        0b01000010,
        0b00111100,
    },
    // 049: 1
    {
        0b00001000,
        0b00011000,
        0b00001000,
        0b00001000,
        0b00001000,
        0b00001000,
        0b00001000,
        0b01111110,
    },
    // 050: 2
    {
        0b01111000,
        0b00000100,
        0b00000010,
        0b00000110,
        0b00001100,
        0b00011000,
        0b00110000,
        0b01111110,
    },
    // 051: 3
    {
        0b01111100,
        0b00000010,
        0b00000010,
        0b00111110,
        0b00000010,
        0b00000010,
        0b00000010,
        0b01111100,
    },
    // 052: 4
    {
        0b01000010,
        0b01000010,
        0b01000010,
        0b00111110,
        0b00000010,
        0b00000010,
        0b00000010,
        0b00000010,
    },
    // 053: 5
    {
        0b01111110,
        0b01000000,
        0b01000000,
        0b00111100,
        0b00000010,
        0b00000010,
        0b00000010,
        0b01111100,
    },
    // 054: 6
    {
        0b00111110,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01111100,
        0b01000010,
        0b01000010,
        0b00111100,
    },
    // 055: 7
    {
        0b01111110,
        0b00000010,
        0b00000100,
        0b00111110,
        0b00010000,
        0b00100000,
        0b01000000,
        0b01000000,
    },
    // 056: 8
    {
        0b00111100,
        0b01000010,
        0b01000010,
        0b00111100,
        0b00111100,
        0b01000010,
        0b01000010,
        0b00111100,
    },
    // 057: 9
    {
        0b00111100,
        0b01000010,
        0b01000010,
        0b01000010,
        0b00111110,
        0b00000010,
        0b00000010,
        0b01111100,
    },
    // 058: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 059: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 060: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 061: =
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b01111110,
        0b00000000,
        0b01111110,
        0b00000000,
        0b00000000,
    },
    // 062: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 063: ?
    {
        0b00000000,
        0b00111000,
        0b01000100,
        0b00011000,
        0b00010000,
        0b00000000,
        0b00010000,
        0b00010000,
    },
    // 064: TODO
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 065: A
    {
        0b00111100,
        0b01100110,
        0b01000010,
        0b01000010,
        0b01111110,
        0b01000010,
        0b01000010,
        0b01000010,
    },
    // 066: B
    {
        0b01111100,
        0b01000010,
        0b01000010,
        0b01011100,
        0b01011100,
        0b01000010,
        0b01000010,
        0b01111100,
    },
    // 067: C
    {
        0b00111100,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01000000,
        0b00111100,
    },
    // 068: D
    {
        0b01111000,
        0b01000100,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000100,
        0b01111000,
    },
    // 069: E
    {
        0b01111110,
        0b01000000,
        0b01000000,
        0b01111110,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01111110,
    },
    // 070: F
    {
        0b01111110,
        0b01000000,
        0b01000000,
        0b01111110,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01000000,
    },
    // 071: G
    {
        0b00111100,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01001100,
        0b01000010,
        0b01000010,
        0b00111100,
    },
    // 072: H
    {
        0b01000010,
        0b01000010,
        0b01000010,
        0b01111110,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
    },
    // 073: I
    {
        0b01111110,
        0b00001000,
        0b00001000,
        0b00001000,
        0b00001000,
        0b00001000,
        0b00001000,
        0b01111110,
    },
    // 074: J
    {
        0b01111110,
        0b00001000,
        0b00001000,
        0b00001000,
        0b00001000,
        0b00001000,
        0b00001000,
        0b01110000,
    },
    // 075: K
    {
        0b01000010,
        0b01000100,
        0b01001000,
        0b01110000,
        0b01110000,
        0b01001000,
        0b01000100,
        0b01000010,
    },
    // 076: L
    {
        0b01000000,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01111110,
    },
    // 077: M
    {
        0b01100110,
        0b01100110,
        0b01011010,
        0b01011010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
    },
    // 078: N
    {
        0b01000010,
        0b01000010,
        0b01100010,
        0b01010010,
        0b01001010,
        0b01000110,
        0b01000010,
        0b01000010,
    },
    // 079: O
    {
        0b00111100,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b00111100,
    },
    // 080: P
    {
        0b01111100,
        0b01000010,
        0b01000010,
        0b01111100,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01000000,
    },
    // 081: Q
    {
        0b00111100,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01010010,
        0b01001010,
        0b01000100,
        0b00111010,
    },
    // 082: R
    {
        0b01111100,
        0b01000010,
        0b01000010,
        0b01111100,
        0b01110000,
        0b01001000,
        0b01000100,
        0b01000010,
    },
    // 083: S
    {
        0b00111110,
        0b01000000,
        0b01000000,
        0b01110000,
        0b00001100,
        0b00000010,
        0b00000010,
        0b01111100,
    },
    // 084: T
    {
        0b01111110,
        0b00001000,
        0b00001000,
        0b00001000,
        0b00001000,
        0b00001000,
        0b00001000,
        0b00001000,
    },
    // 085: U
    {
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b00111100,
    },
    // 086: V
    {
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b00100100,
        0b00100100,
        0b00011000,
    },
    // 087: W
    {
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b01011010,
        0b01011010,
        0b00100100,
    },
    // 088: X
    {
        0b01000010,
        0b01000010,
        0b01000010,
        0b00111100,
        0b00111100,
        0b01000010,
        0b01000010,
        0b01000010,
    },
    // 089: Y
    {
        0b01000010,
        0b01000010,
        0b01000010,
        0b01000010,
        0b00111100,
        0b00001000,
        0b00001000,
        0b00001000,
    },
    // 090: Z
    {
        0b01111110,
        0b00000010,
        0b00000100,
        0b00001000,
        0b00010000,
        0b00100000,
        0b01000000,
        0b01111110,
    },
    // 091: [
    {
        0b00111100,
        0b00100000,
        0b00100000,
        0b00100000,
        0b00100000,
        0b00100000,
        0b00100000,
        0b00111100,
    },
    // 092: backslash
    {
        0b10000000,
        0b01000000,
        0b00100000,
        0b00010000,
        0b00001000,
        0b00000100,
        0b00000010,
        0b00000001,
    },
    // 093: ]
    {
        0b00111100,
        0b00000100,
        0b00000100,
        0b00000100,
        0b00000100,
        0b00000100,
        0b00000100,
        0b00111100,
    },
    // 094: ^
    {
        0b00010000,
        0b00101000,
        0b01000100,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 095: _
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b01111110,
    },
    // 096: `
    {
        0b01000000,
        0b00110000,
        0b00001000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 097: a
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00111100,
        0b01000010,
        0b01000010,
        0b01000110,
        0b00111010,
    },
    // 098: b
    {
        0b01000000,
        0b01000000,
        0b01000000,
        0b01000000,
        0b01111100,
        0b01000010,
        0b01000010,
        0b01111100,
    },
    // 099: c
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00111110,
        0b01000000,
        0b01000000,
        0b00111110,
    },
    // 100: d
    {
        0b00000010,
        0b00000010,
        0b00000010,
        0b00111110,
        0b01000010,
        0b01000010,
        0b01000110,
        0b00111010,
    },
    // 101: e
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00011100,
        0b00100010,
        0b00111100,
        0b00100000,
        0b00011110,
    },
    // 102: f
    {
        0b00000000,
        0b00001100,
        0b00010010,
        0b00010000,
        0b00111100,
        0b00010000,
        0b00010000,
        0b00010000,
    },
    // 103: g
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00111100,
        0b01100010,
        0b00111110,
        0b00000010,
        0b00011100,
    },
    // 104: h
    {
        0b00000000,
        0b00100000,
        0b00100000,
        0b00100000,
        0b00101100,
        0b00110010,
        0b00100010,
        0b00100010,
    },
    // 105: i
    {
        0b00000000,
        0b00000000,
        0b00011000,
        0b00000000,
        0b00011000,
        0b00001000,
        0b00001000,
        0b00011100,
    },
    // 106: j
    {
        0b00000000,
        0b00000000,
        0b00001100,
        0b00000000,
        0b00000100,
        0b00000100,
        0b01000100,
        0b00111000,
    },
    // 107: k
    {
        0b00100000,
        0b00100000,
        0b00100000,
        0b00100100,
        0b00101000,
        0b00110000,
        0b00101000,
        0b00100100,
    },
    // 108: l
    {
        0b01100000,
        0b00010000,
        0b00010000,
        0b00010000,
        0b00010000,
        0b00010000,
        0b00010000,
        0b00001100,
    },
    // 109: m
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b01011100,
        0b01101010,
        0b01001010,
        0b01001010,
    },
    // 110: n
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b01011100,
        0b01100010,
        0b01000010,
        0b01000010,
    },
    // 111: o
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00111100,
        0b01000010,
        0b01000010,
        0b01000010,
        0b00111100,
    },
    // 112: p
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00011100,
        0b00100010,
        0b00111100,
        0b00100000,
        0b00100000,
    },
    // 113: q
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00110000,
        0b01001000,
        0b01001100,
        0b00110100,
        0b00000110,
    },
    // 114: r
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b01011100,
        0b01100010,
        0b01000000,
        0b01000000,
        0b01000000,
    },
    // 115: s
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00111100,
        0b01100000,
        0b00110000,
        0b00001100,
        0b01111000,
    },
    // 116: t
    {
        0b00010000,
        0b00010000,
        0b01111110,
        0b00010000,
        0b00010000,
        0b00010000,
        0b00010010,
        0b00001100,
    },
    // 117: u
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b01000010,
        0b01000010,
        0b01000110,
        0b00111010,
    },
    // 118: v
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00100010,
        0b00100010,
        0b00010100,
        0b00001000,
    },
    // 119: w
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b01000010,
        0b01011010,
        0b01011010,
        0b00100100,
    },
    // 120: x
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b01000010,
        0b00100100,
        0b00011000,
        0b00100100,
        0b01000010,
    },
    // 121: y
    {
        0b00000000,
        0b00000000,
        0b01000100,
        0b01000100,
        0b00100100,
        0b00011000,
        0b00110000,
        0b01100000,
    },
    // 122: z
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b01111110,
        0b00001000,
        0b00010000,
        0b01111110,
    },
    // 123: {
    {
        0b00111110,
        0b01000000,
        0b00100000,
        0b00010000,
        0b00010000,
        0b00100000,
        0b01000000,
        0b00111110,
    },
    // 124: |
    {
        0b00001000,
        0b00001000,
        0b00001000,
        0b00001000,
        0b00001000,
        0b00001000,
        0b00001000,
        0b00001000,
    },
    // 125: }
    {
        0b01111100,
        0b00000010,
        0b00000100,
        0b00001000,
        0b00001000,
        0b00000100,
        0b00000010,
        0b01111100,
    },
    // 126: ~
    {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00110010,
        0b01001100,
        0b00000000,
        0b00000000,
        0b00000000,
    },
    // 127: DEL (TM)
    {
        0b11101110,
        0b01010101,
        0b01010101,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
    },
};
