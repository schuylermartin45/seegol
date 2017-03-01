#!/bin/bash
#
# File:    cxpm_all.sh
#
# Author:  Schuyler Martin <sam8050@rit.edu>
#
# Description: Converts all files in img_xpm to cxpm files stored in img_cxpm
#

for file in img_xpm/*.xpm; do
    echo ${file}
    ./cxpm.py ${file}
done
