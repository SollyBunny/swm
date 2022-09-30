SCREEN=:5

build:
	gcc main.c -o swm -lX11 -lXft -I/usr/include/freetype2
	DISPLAY=$(SCREEN) ./swm

valgrind:
	gcc main.c -o swm -lX11 -lXft -I/usr/include/freetype2 -Wall -ggdb3 -g
	DISPLAY=$(SCREEN) valgrind --leak-check=full \
	         --show-leak-kinds=all \
	         --track-origins=yes \
	         --log-file=valgrind-out.txt \
	         ./swm
	cat valgrind-out.txt
	rm valgrind*
	
start:
	Xephyr -br -ac -noreset -screen 1920x1080 $(SCREEN)
startsmall:
	#Xephyr -br -ac -noreset -screen 500x500 $(SCREEN)
	Xephyr -br -ac -noreset -screen 900x1080 $(SCREEN)
