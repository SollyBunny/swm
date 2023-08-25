# swm
A [dwm](https://dwm.suckless.org/) clone made in C with even less bloat (and more bugs)  
A lot of things are stolen directly from `dwm` and other WMs because the best documentation for `xlib` is code  
Woudn't recommend using this as a daily driver, but do it anyway :)

## Compiling
Run `make`

## Installing
Run `make install`

## Testing
There are scripts in `test/` to test `swm`  
  
`conf.sh`:  
> Contains display to use  

`x.sh`:  
> Run on a new `X` server using `startx`

`xephyr.sh`:  
> Starts `XEPHYR` window 1920x1080  

`xephyrsmall.sh`:  
> Starts `XEPHYR` window 900x1080  

`run.sh`:  
> Compiles and runs `swm` on `XEPHYR`  

`valgrind.sh`:  
> Debug compiles and runs `swm` on `XEPHYR` with `valgrind`  


## Features
* Virtual desktops
* Task bar
* Root text
* Window movement/resizement
* Window stack
* Alt tab
* Key bind config
* GAPs

## TODO
* Fix all the bugs caused by edge cases I have no idea about
* Handle variable screen size
* Multiscreen XINERAMA
* Option for tiling
* Drawing on root text
* Variable color themes
* Option for window borders
* Other stuff
* Clean my messy code

