jazzhan-d
------

HackNC Fall 2014

Leap Motion Input -> X11 Mouse + arbitrary system commands

Configuration is done at compile time in config.h

# Windows Build

I don't really know how to windows, but this should get you running.

Install MinGW and the following packages:
* mingw32-binutils
* mingw32-gcc
* mingw32-gcc-g++
* mingw32-libgcc
* mingw32-libgmp
* mingw32-libgomp
* mingw32-libiconv
* mingw32-libintl
* mingw32-libmpc
* mingw32-libmpfr
* mingw32-libpthread-old
* mingw32-libpthreadgc
* mingw32-libquadmath
* mingw32-libssp
* mingw32-libstdc++
* mingw32-libz
* mingw32-make
* mingw32-mingwrt
* mingw32-w32api

Download the MinGW files from `https://community.leapmotion.com/t/mingw-support-please/270/4`
Copy the .h files to `MinGW/include` and the .dll to `MinGW/bin` and also to `MinGW/lib`. Make sure bin is in system path.

Run mingw32-make in the folder. Make sure to edit the commands to run windows commands.

LICENSE
------
MIT License
