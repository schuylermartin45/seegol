#!/usr/bin/python3
##
## File:    cxpm.py
##
## Author:  Schuyler Martin <sam8050@rit.edu>
##
## Description: Compresses an XPM to my own "cxpm" format that I invented
##              This script takes an XPM file, converting it and storing it as
##              a CXPM file in the "img_cxpm" directory. XPM files use an
##              array of strings to store the image data. I have elected to use
##              arrays of raw bytes instead. This makes the parsing code much
##              easier the CXPM encoding easier to debug.
##
##              The file is formatted as follows:
##                - An array declaration
##                - A line of header information:
##                  {width, height, transparency_code & color_map_size}
##                  + Upper 4 bits: Color code mapped to "transparent"
##                    (0 if not used)
##                  + Lower 4 bits: Size of the color table
##                - The color mapping, 4 bits
##                  {color_code, R, G, B}
##                - Character mappings
##                  + 2 pixels per byte; 4 bits for RGB pixel lookup
##                  + One byte indicates Huffman encoded portion:
##                      {... MARKER, number_of_duplicates, pixel_byte, ...}
##

# Python libraries
import math
import os
import sys
# project libraries

#### GLOBALS    ####
USAGE = "Usage: ./cxpm file"
# file naming convetions
FILE_EXT    = ".cxpm"
# for file path replacement
XPM_FILE_PATH    = "img_xpm/"
CXPM_FILE_PATH   = "img_cxpm/"
# value used to mark run lengths
ENCODE_MARKER = 0
# "cost" for performing a run length encoding
# 1 byte for the marker -> costs 2 pixels
# 1 byte for the run length digit -> costs 2 pixels
# 1 byte for pixels to duplicate -> costs 1 pixel(?)
RUN_BYTE_COST = 5
# 15 colors available since we give up one for the encoding marker
MAX_COLOR_SPACE = 15
# indent for readability
INDENT = "  "
# casting statement pre-pended to every byte array in our table
# so we get:
# unsigned char* arr = { (char[]){1, 2, 3}}, ... }
ROW_ARR_CAST = INDENT + "(const unsigned char[])"
# line that starts the color table in the original file
COLOR_TBL_START = 3
# RGB values to mark something as transparent. This is just a placeholder
TRANSPARENT_RGB = ("00", "00", "00")

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

def rgb_dist(rgb0, rgb1):
    '''
    Calculates the distance between two RGB values
    RGB values are stored as hex strings, as they are read from a text file
    :param: rgb0 First RGB value
    :param: rgb1 Second RGB value
    '''
    return math.sqrt(
          math.pow(int(rgb0[0], 16) - int(rgb1[0], 16), 2)
        + math.pow(int(rgb0[1], 16) - int(rgb1[1], 16), 2)
        + math.pow(int(rgb0[2], 16) - int(rgb1[2], 16), 2)
    )

def closest_color(chr_tbl, rgb_tbl, chr_code, chr_rgb):
    '''
    This function provides a close approximation of a color in the table, based
    on values already in the table. This is used when an image is provided that
    has more colors than CXPM supports.
    :param: chr_tbl Table that maps the XPM characters to the new look-up codes
            This is the table that will be updated by this method
    :param: rgb_tbl Table that maps XPM characters to RGB values
    :param: chr_code XPM character code to find the closest color to
    :param: rgb RGB tuple to find the closest color approximation of
    '''
    closest = 255 * 3
    closest_code = chr_code
    for chr_key in rgb_tbl:
        rgb = rgb_tbl[chr_key]
        # find the closest approximation thus far
        chk_dist = rgb_dist(chr_rgb, rgb)
        if (chk_dist < closest):
            closest = chk_dist
            closest_code = chr_tbl[chr_key]
    # indicate that this character should map to the closest color code
    chr_tbl[chr_code] = closest_code

def huff_run(byte, byte_cntr):
    '''
    Compresses byte data using a Huffman encoding scheme
    :param: byte Repeated byte to compress
    :param: byte_cntr Number of occurances of a byte
    :return: String of comma separated integers (to be stored in an array)
    '''
    compressed = ""
    while ((byte_cntr - 255) > 0):
        compressed += (str(ENCODE_MARKER) + ","
            + str(255) + ","
            + str(byte) + ",")
    compressed += (str(ENCODE_MARKER) + ","
        + str(byte_cntr) + ","
        + str(byte) + ",")
    return compressed

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

    # modify array naming
    xpm_data[1] = xpm_data[1].replace("static", "static const");
    # mark as a "unint8_t" without needing to use types file
    xpm_data[1] = xpm_data[1].replace("char", "unsigned char");
    # mark array as "compressed XPM"
    xpm_data[1] = xpm_data[1].replace("_xpm", "_cxpm");

    # read XPM header info
    header = xpm_data[2][1:-3].split()
    dim_w = int(header[0])
    dim_h = int(header[1])
    color_space = int(header[2])
    # the actual amount of color being used
    cxpm_color_space = color_space
    if (color_space > MAX_COLOR_SPACE):
        cxpm_color_space = MAX_COLOR_SPACE

    # start building the file again
    # this line defines the character array structure
    cxpm_data.append(xpm_data[1])
    # this line contains the header information
    cxpm_data.append(ROW_ARR_CAST + "{"
        + header[0] + ","
        + header[1] + ","
        + str(cxpm_color_space) + "},\n"
    )
    # reduce encoding on the color table; remapping characters to 4-bit ints
    cxpm_color_tbl = {}
    # record mappings of the character codes to RGB values in case we need to
    # approximate colors later
    chr_rgb_tbl = {}
    # re-assign color mappings to 4 bit values
    new_key = ENCODE_MARKER + 1
    for i in range(0, color_space):
        # first N colors are left in the table
        if (i < MAX_COLOR_SPACE):
            cxpm_color_tbl[xpm_data[i + COLOR_TBL_START][1]] = new_key
            # sometimes files have these strings instead of RGB codes
            chk_idx = xpm_data[i + COLOR_TBL_START].find('",')
            color_chk = xpm_data[i + COLOR_TBL_START][5:chk_idx]
            # store these RGB values as strings, to keep the typing
            if (color_chk == "white"):
                rgb = ("FF", "FF", "FF")
            elif (color_chk == "black"):
                rgb = ("00", "00", "00")
            elif(color_chk =="None"):
                rgb = TRANSPARENT_RGB
                # encode transparency code in upper 4 bits of color table size
                # by rebuilding the header information
                bit_packed = (new_key << 4) + cxpm_color_space
                cxpm_data[1] = (ROW_ARR_CAST + "{"
                    + header[0] + ","
                    + header[1] + ","
                    + str(bit_packed) + "},\n"
                )
            else:
                rgb = (
                    xpm_data[i + COLOR_TBL_START][6:8],
                    xpm_data[i + COLOR_TBL_START][8:10],
                    xpm_data[i + COLOR_TBL_START][10:12],
                )
            chr_rgb_tbl[xpm_data[i + COLOR_TBL_START][1]] = rgb
            # strip pixel mapping down to { key_byte, R_byte, G_byte, B_byte }
            cxpm_data.append(ROW_ARR_CAST + "{" +
                str(new_key) + ","
                + "0x" + rgb[0] + ","
                + "0x" + rgb[1] + ","
                + "0x" + rgb[2] + ","
                + "},\n"
            )
            new_key += 1
        # once we get past the limits of CXPM, we try to render new colors
        # to the closest approximations
        else:
            chr_code = xpm_data[i + COLOR_TBL_START][1]
            chr_rgb = (
                xpm_data[i + COLOR_TBL_START][6:8],
                xpm_data[i + COLOR_TBL_START][8:10],
                xpm_data[i + COLOR_TBL_START][10:12],
            )
            closest_color(cxpm_color_tbl, chr_rgb_tbl, chr_code, chr_rgb)
    # start in the original file where the pixel mapping occurs
    pixel_map_start = COLOR_TBL_START + color_space

    # first pass: replace characters in the table with 2, 4 bit encodings
    # and combine 4 bits into 8 bit values; 2 pixels per byte
    # this is stored as an array of integers for easier parsing later
    xpm_pass_1 = []
    for i in range(pixel_map_start, pixel_map_start + dim_h):
        line = []
        for j in range(1, len(xpm_data[i]) - 4, 2):
            hi = cxpm_color_tbl[xpm_data[i][j]] << 4
            lo = cxpm_color_tbl[xpm_data[i][j + 1]]
            line.append(hi + lo)
        xpm_pass_1.append(line)

    # second pass: Huffman encoding
    for i in range(0, len(xpm_pass_1)):
        compressed = ""
        # a NULL byte is added to the end of the array to make parsing easier
        # prevents the last byte(s) from being left out
        xpm_pass_1[i].append(0)
        cur_byte = xpm_pass_1[i].pop(0)
        byte_cntr = 1
        # scan for duplicates
        while (len(xpm_pass_1[i]) > 0):
            # count duplicates
            top_byte = xpm_pass_1[i].pop(0)
            # count duplicates
            if (cur_byte == top_byte):
                byte_cntr += 1
            # write on byte change and reset for next run length
            else:
                # only compress when it's worth the cost of marking a run
                # of repeated values
                if (byte_cntr > RUN_BYTE_COST):
                    # for run lengths that can't be expressed in 8 bits,
                    # break the encoding in segments
                    compressed += huff_run(cur_byte, byte_cntr)
                else:
                    for k in range(0, byte_cntr):
                        compressed += str(cur_byte) + ","
                cur_byte = top_byte
                byte_cntr = 1

        # commit the new line to the compressed file
        cxpm_data.append(ROW_ARR_CAST + "{" + compressed + "},\n")
    # close the outer array
    cxpm_data.append("};")

    # dump the new data into a file
    out_fd = os.path.splitext(sys.argv[1])[0] + FILE_EXT
    # change path
    out_fd = out_fd.replace(XPM_FILE_PATH, CXPM_FILE_PATH)
    # file header data
    header_comment = ("/* " + os.path.basename(out_fd)
        + " generated by cxpm.py */\n")
    cxpm_data = [header_comment] + cxpm_data
    # write the file
    write_file(cxpm_data, out_fd)
    print("Compressed and written to: " + out_fd)

if __name__ == "__main__":
    main()
