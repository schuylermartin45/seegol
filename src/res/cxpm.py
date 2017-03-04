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
##                - Pixel information, an array of bytes:
##                  + Each "color code" byte represents 2 pixel color codes
##                  + Regions start at the 0,0 and are built from left to right
##                  + CWHX paints a region starting with an upper-left pixel
##                    at X, color code C, and a W,H-sized rectangle
##                  + C0 Paints one byte worth of color coding (2 pixels)
##                  + Regions are sequential, moving a cursor left to right
##                  + 0XY indicates the upper-left coordinate of next left-most
##                    region when the cursor has reached the end of the frame
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

#### Class      ####

class Point:
    '''
    Simple point class for storing 2D information
    '''
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __str__(self):
        return "(" + str(self.x) + "," + str(self.y) + ")"

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
            elif (color_chk =="None"):
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

    # First Pass: replace characters in the table with 2, 4 bit encodings
    # and combine 4 bits into 8 bit values; 2 pixels per byte
    # this is stored as an array of integers for easier parsing later
    xpm_pass_1 = []
    for i in range(pixel_map_start, pixel_map_start + dim_h):
        line = []
        l_idx = xpm_data[i].index('"') + 1
        r_idx = xpm_data[i].rindex('"')
        for j in range(l_idx, r_idx, 2):
            hi = cxpm_color_tbl[xpm_data[i][j]] << 4
            # if an odd length, copy a pixel over
            if (j + 1 >= r_idx):
                lo = hi
            else:
                lo = cxpm_color_tbl[xpm_data[i][j + 1]]
            line.append(hi + lo)
        xpm_pass_1.append(line)

    # Second Pass: 2D Huffman encoding
    compressed = ""
    # cursor holds the current upper-left coordinate designating the start of
    # a rectangular region
    cursor = Point(0, 0)
    # determine if we have represented the whole image
    total_pixels = dim_w * dim_h
    pixel_cntr = 0
    while (pixel_cntr < total_pixels):
        cur_val = xpm_pass_1[cursor.y][cursor.x]
        # find max region size in both directions
        max_w = 0
        max_h = 0
        # x direction
        for x in range(cursor.x, len(xpm_pass_1[cursor.y])):
            if (cur_val != xpm_pass_1[cursor.y][x]):
                break
            max_w += 1
        # y direction
        for y in range(cursor.y, len(xpm_pass_1)):
            if (cur_val != xpm_pass_1[y][cursor.x]):
                break
            max_h += 1
        # Now check to see if that region stays uniform, shrinking it if it
        # doesn't. For example, if we just used the values from the previous
        # check, we could accidentally throw out color information if the
        # cursor was on an edge.
        w_val = max_w
        h_val = max_h
        for y in range(cursor.y, cursor.y + max_h):
            for x in range(cursor.x, cursor.x + max_w):
                if (xpm_pass_1[y][x] != cur_val):
                    # minimize width and height
                    if (x < w_val):
                        w_val = x
                    if (y < h_val):
                        h_val = y
        # store region data
        if ((w_val == 1) and (h_val == 1)):
            compressed += (
                str(cur_val) + ","
                + str(ENCODE_MARKER) + ", "
            )
        else:
            compressed += (
                str(cur_val) + ","
                + str(w_val) + ","
                + str(h_val) + ","
                + str(cursor.x) + ", "
            )

        # fill the region with a marker in the image array so that we can
        # easily find the upper-left starting point when we move in the y
        # y direction
        for y in range(cursor.y, cursor.y + h_val):
            for x in range(cursor.x, cursor.x + w_val):
                xpm_pass_1[y][x] = ENCODE_MARKER

        # advance the cursor
        cursor.x += w_val
        # skip marked/covered regions
        if (cursor.x < (dim_w // 2)):
            if (xpm_pass_1[cursor.y][cursor.x] == ENCODE_MARKER):
                for x in range(cursor.x, len(xpm_pass_1[cursor.y])):
                    if (xpm_pass_1[cursor.y][x] != ENCODE_MARKER):
                        cursor.x = x
                        break
                # in case there are 0s leading to the end boundry, force
                # the end of line check
                if (xpm_pass_1[cursor.y][cursor.x] == ENCODE_MARKER):
                    cursor.x = dim_w // 2

        # if we've reached the end of the line, skip to the next left-most
        # upper-left region position and put in an end marker
        if (cursor.x >= (dim_w // 2)):
            # find the next coordinate to start looking at
            for y in range(cursor.y, len(xpm_pass_1)):
                for x in range(0, len(xpm_pass_1[y])):
                    if (xpm_pass_1[y][x] != ENCODE_MARKER):
                        cursor.x = x
                        cursor.y = y
                        break
                else:
                    continue
                break
            # put in an end-of-line marker 
            compressed += (
                str(ENCODE_MARKER) + ","
                + str(cursor.x) + ","
                + str(cursor.y) + ","
            )
            # add a newline for readability
            compressed += "\n";

        # count the total number of pixels drawn by this region
        pixel_cntr += w_val * h_val * 2

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
