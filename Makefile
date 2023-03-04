build:
	gcc main.c -o swm -lX11 -lXft -lfontconfig -I/usr/include/freetype2 -O4

install:
	rm "/usr/local/bin/swm" -f
	ln -s "`pwd`/swm" "/usr/local/bin/swm"

uninstall:
	rm "/usr/local/bin/swm" -f
