#!/usr/bin/python3
##
## File:    cxpm.py
##
## Author:  Schuyler Martin <sam8050@rit.edu>
##
## Description: Compresses an XPM to my own "cxpm" format
##

# Python libraries
import os
import sys
# project libraries

#### GLOBALS    ####
USAGE = "Usage: ./cxpm file"
FILE_EXT = ".cxpm"

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
    fptr = open(fd, 'w')
    for line in data:
        fptr.write(line)
    fptr.close()

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
    encode_w = int(header[3])

    # start building the file again
    cxpm_data.append(xpm_data[1])
    cxpm_data.append(xpm_data[2])
    # line that 
    COLOR_SPACE_START = 3
    # reduce encoding on color table
    for i in range(COLOR_SPACE_START, COLOR_SPACE_START + color_space):
        cxpm_data.append(xpm_data[i][:2] + xpm_data[i][6:])

    # non-visible character used to mark duplicate length
    ENCODE_MARKER = chr(1)
    pixel_map_start = COLOR_SPACE_START + color_space
    for i in range(pixel_map_start, pixel_map_start + dim_h):
        compressed = ""
        # start at the first character, ignoring the leading "
        cur_ch = xpm_data[i][1]
        ch_cntr = 1
        # scan for duplicates, skipping over "",\n characters
        for j in range(2, len(xpm_data[i]) - 2):
            # count duplicates
            if (cur_ch == xpm_data[i][j]):
                ch_cntr += 1
            # write on change in character and reset for next run length
            else:
                # only compress when it's worth the cost of marking
                # TODO encode with one HEX (or greater base) digit
                if (ch_cntr > 3):
                    compressed += ENCODE_MARKER + str(ch_cntr) + cur_ch
                else:
                    for k in range(0, ch_cntr):
                        compressed += cur_ch
                cur_ch = xpm_data[i][j]
                ch_cntr = 1
        # commit the new line to the compressed file
        compressed = "\"" + compressed + "\",\n"
        cxpm_data.append(compressed)
    # close array, on the last line, also correcting for the omitted ,
    # on the last line
    cxpm_data[-1] = cxpm_data[-1][:-4] + "\"};"

    # dump the new data into a file
    out_fd = os.path.splitext(sys.argv[1])[0] + FILE_EXT
    write_file(cxpm_data, out_fd)

if __name__ == "__main__":
    main()
