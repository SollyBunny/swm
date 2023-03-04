. ./conf.sh
cd ..
gcc main.c -o swm -lX11 -lXft -lfontconfig -I/usr/include/freetype2 -O4
startx ./swm
