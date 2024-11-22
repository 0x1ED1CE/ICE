# [HOW TO BUILD FOR SDL2]
# On Windows you will need MinGW installed
#
# Run `make CC=gcc PLATFORM=sdl` in this directory
# Make will generate libice.a
#
# To compile for Windows:
# gcc -I. demos/c99/triangle.c -L. -lice -lm -lmingw32 -lSDL2main -lSDL2 -lopengl32 -o test.exe
#
# To compile for Linux:
# gcc -I. demos/c99/triangle.c -L. -lice -lm -lSDL2main -lSDL2 -lGL -o test


# [HOW TO BUILD FOR DOS]
# First you need DJGPP installed and MinGW if you're on Windows:
# https://github.com/andrewwutw/build-djgpp/releases
#
# Once installed, run `make` in this directory
# Make will generate libice.a
#
# To compile a demo program:
# i586-pc-msdosdjgpp-gcc -I. demos/c99/triangle.c -L. -lice -lm -Llib/tinygl/lib -lTinyGL -o test.exe
#
# To compile with 3dfx Voodoo support:
# Edit platform/dos/video.c and change vesa.h to mesa.h
# Run make again
# Swap '-Llib/tinygl/lib -lTinyGL' with '-Llib/mesa/lib -ligl -lgl'
# i586-pc-msdosdjgpp-gcc -I. demos/c99/triangle.c -L. -lice -lm -Llib/mesa/lib -ligl -lgl -o test.exe
#
# To run, the following files must be in the same directory as the DOS program:
# CWSDPMI.EXE
# GL.DXE      (Only if using 3dfx Voodoo)
# GLIDE3X.DXE (Only if using 3dfx Voodoo)
#
# CWSDPMI.EXE should already be included but it can also be obtained from:
# https://sandmann.dotster.com/cwsdpmi/
#
# GL.DXE can be found under lib/mesa/lib
#
# GLIDE3X.DXE can be obtained from:
# https://github.com/basecq/q2dos/tree/master/dos/3rdparty/lib_dxe/voodoo
#
# Make sure to choose the correct DXE driver for the Voodoo card you're using


CC       = i586-pc-msdosdjgpp-gcc
AR       = ar
CFLAGS   = -std=gnu99 -Wall -Wextra -Wno-attributes -s -O2
PLATFORM = dos

all: libice.a

libice.a:
	$(CC) $(CFLAGS) -I. -c platform/$(PLATFORM)/*.c
	$(AR) -rcs libice.a *.o

clean:
	$(RM) *.a *.o
