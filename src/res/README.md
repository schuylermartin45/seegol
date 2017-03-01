# HSC: Shoyler's Extremely Experimental Graphical Operating Library - SeeGOL
#### Author:  Schuyler Martin <sam8050@rit.edu>

# src/res/
Image Resource Directory. This directory holds resource files, non-code
entities (i.e. artwork, images, assets). Ownership infomation can be found
below. This OS is free and built for academic purposes; not for profit.

## ImageMagick Dependency
Currently images are down-sampled and color quantized using `convert` provided
by the ImageMagick toolkit. If I have time, I will write an OpenCV program to
perform these computations for me.

## Image Build Chain
All the tools to build images are included in the src/res/ directory.
#### Program Pipeline
Original image -> xpm_convert.sh -> cxpm.py -> Image to be imported into the OS
#### File Location Pipeline
img_original/ -> img_xpm/ -> img_cxpm

## Legal: Image Ownership
1. Aperture Science logo - Valve Corporation
2. CSH logo - Computer Science House (Rochester Institute of Technology)
3. HSC logo and Rick and Morty caricature - Kailey A. Martin
4. Dark Side of the Moon album artwork, Hammers from The Wall, and Wish
   You Were Here album artwork - Pink Floyd
5. Deep 13 logo - Mystery Science Theater 3000, Shout Factory
6. Initech logo - Office Space, Twentieth Century Fox
7. Jurassic Park logo, InGen logo, DNA guy - Jurassic Park, Universal Studios
