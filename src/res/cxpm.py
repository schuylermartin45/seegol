#!/usr/bin/python3
##
## File:    cxpm.py
##
## Author:  Schuyler Martin <sam8050@rit.edu>
##
## Description: Compresses an XPM to my own "cxpm" format that I invented
##              The file is formatted as follows:
##                - An array declaration
##                - A line of header information:
##                  <width> <height> <color_map_size>
##                - The color mapping (up to 15 colors)
##                  <ch_color><RGB_HEX_CODE>
##                - Character mappings
##                  + 2 pixels per byte; 4 bits for RGB pixel lookup
##                  + One byte indicates Huffman encoded portion:
##                      <MARKER><hex_byte_num_duplicates><pixel_byte>
##

# Python libraries
import os
import sys
# project libraries

#### GLOBALS    ####
USAGE = "Usage: ./cxpm file"
FILE_EXT = ".cxpm"
# ASCII value of non-visible character used to mark duplicate length
ENCODE_MARKER = 1
# base used to use Huffman encoding; 36 gives us 0-9 and A-Z
BASE_ENCODE = 36
# "cost" for performing a run length encoding
# 1 byte for the marker -> costs 2 pixels
# 1 byte for the run length digit -> costs 2 pixels
# 1 byte for pixels to duplicate -> costs 1 pixel(?)
RUN_BYTE_COST = 5
# 14 colors available since the NULL byte is not allowed in a C string
# and another byte is used as a marker
MAX_COLOR_SPACE = 14

#### FUNCTIONS  ####

def read_file(fd):
    '''
    Reads in the file, line by line
    :param: fd Name of the file
    :return: Contents of the file, as an array of line strings
    '''
    data = []
    for line in open(fd):
        data += [line]
    return data

def write_file(data, fd):
    '''
    Writes to a file, line by line
    :param: data Lines of the file to write
    :param: fd Name of the file to write
    '''
    fptr = open(fd, 'w', encoding="latin-1")
    for line in data:
        fptr.write(line)
    fptr.close()

def int_str(num, base):
    '''
    Converts an integer to a string, using a given base
    Bases > 10 start at A and then follow the ASCII chart
    :param: num Integer to convert
    :param: base Base to put representation in
    :return: Return the string equivalent
    '''
    s = ""
    # perform the conversion, which reverses the digit order
    while(num > 0):
        mod = num % base
        if (mod > 9):
            s += chr((mod - 10) + ord('A'))
        else:
            s += chr(mod + ord('0'))
        num //= base
    # reverse the string
    return s[::-1]

#### MAIN       ####

def main():
    '''
    Main execution point of the program
    '''
    if (len(sys.argv) != 2):
        print(USAGE)
        return 1
    # read in data and prep for the new version of the file data
    xpm_data = read_file(sys.argv[1])
    cxpm_data = []

    # read XPM header info
    header = xpm_data[2][1:-3].split()
    dim_w = int(header[0])
    dim_h = int(header[1])
    color_space = int(header[2])
    # the actual amount of color being used
    cxpm_color_space = color_space
    if (color_space > MAX_COLOR_SPACE):
        cxpm_color_space = color_space

    # start building the file again
    # this line defines the character array structure
    cxpm_data.append(xpm_data[1])
    # this line contains the header information
    cxpm_data.append("\""
        + header[0] + " "
        + header[1] + " "
        + str(cxpm_color_space) + "\",\n"
    )
    # line that starts the color table
    COLOR_TBL_START = 3
    # reduce encoding on the color table; remapping characters
    cxpm_color_tbl = {}
    # re-assign color mappings to 4 bit values
    new_key = ENCODE_MARKER + 1
    for i in range(COLOR_TBL_START, COLOR_TBL_START + cxpm_color_space):
        cxpm_color_tbl[xpm_data[i][1]] = chr(new_key)
        # strip pixel mapping down to <key_byte><R_HEX><G_HEX><B_HEX>
        cxpm_data.append( "\"" + chr(new_key) + xpm_data[i][6:])
        new_key += 1
    # start in the original file where the pixel mapping occurs
    pixel_map_start = COLOR_TBL_START + color_space

    # first pass: replace characters in the table with 4 bit encodings
    # and combine 4 bits into 8 bit values
    xpm_pass_1 = []
    for i in range(pixel_map_start, pixel_map_start + dim_h):
        line = ""
        # perform the bit packing (2, 4 bit values into one byte)
        for j in range(1, len(xpm_data[i]) - 4, 2):
            # TODO key error; use another pixel
            ch0 = ord(cxpm_color_tbl[xpm_data[i][j]]) << 4
            ch1 = ord(cxpm_color_tbl[xpm_data[i][j + 1]])
            new_ch = ch0 + ch1
            # escape quotes and backslash so C doesn't freak out
            if (chr(new_ch) == "\""):
                line += "\\\""
            elif (chr(new_ch) == "\\"):
                line += "\\\\"
            else:
                line += chr(new_ch)
        # TODO odd length case; pad with another pixel (copy)
        xpm_pass_1.append(line + "\n")

    # second pass: Huffman encoding
    for i in range(0, len(xpm_pass_1)):
        compressed = ""
        # start at the first character
        cur_ch = xpm_pass_1[i][0]
        ch_cntr = 1
        # scan for duplicates
        for j in range(1, len(xpm_pass_1[i])):
            # count duplicates
            if (cur_ch == xpm_pass_1[i][j]):
                ch_cntr += 1
            # write on change in character and reset for next run length
            else:
                # only compress when it's worth the cost of marking a run
                # of repeated values
                if (ch_cntr > RUN_BYTE_COST):
                    # for run lengths that can't be expressed in one digit in
                    # the base, break the encoding in segments
                    digits = int_str(ch_cntr, BASE_ENCODE)
                    for digit in digits:
                        compressed += chr(ENCODE_MARKER) + digit + cur_ch
                else:
                    for k in range(0, ch_cntr):
                        compressed += cur_ch
                cur_ch = xpm_pass_1[i][j]
                ch_cntr = 1
        # commit the new line to the compressed file
        compressed = '"' + compressed + '",\n'
        cxpm_data.append(compressed)
    # close array, on the last line, also correcting for the omitted ,
    # on the last line
    cxpm_data[-1] = cxpm_data[-1][:-4] + "\"};"

    # dump the new data into a file
    out_fd = os.path.splitext(sys.argv[1])[0] + FILE_EXT
    write_file(cxpm_data, out_fd)

if __name__ == "__main__":
    main()
