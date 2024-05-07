# [HOW TO BUILD FOR DOS]
# First you need DJGPP installed as well as MinGW if you're on Windows:
# https://github.com/andrewwutw/build-djgpp/releases
#
# Once installed, run `make` in this directory
# Make will generate libice.a
#
# To build a DOS program, there are 4 static libraries needed to link against:
# libice.a - The library you just made
# libm.a   - Standard math library
# libigl.a - GLIDE import library
# libgl.a  - Mesa OpenGL library
#
# libigl.a and libgl.a are generated from the Mesa graphics library
# Prebuilt binaries should already exist under lib/mesa/lib
#
# Use the following command template to link and compile:
# i586-pc-msdosdjgpp-gcc <source.c> -lice -lm -ligl -lgl -o <output.exe>
#
# Example:
# i586-pc-msdosdjgpp-gcc -I. demos/triangle.c -L. -lice -lm -Llib/mesa/lib -ligl -lgl -o test.exe


# [HOW TO BUILD FOR SDL2]
# For Windows you will need MinGW installed
#
# Run `make CC=gcc PLATFORM=sdl` in this directory
# Make will generate libice.a
#
# To build a SDL2 program, these are the libraries you need to link against:
# libice.a      - The library you just made
# libm.a        - Standard math library
# libmingw32.a  - MinGW library (only on Windows)
# libSDL2main.a - SDL2 main library
# libSDL2.a     - SDL2 library
# libGL.a       - OpenGL library (libopengl32.a on Windows)
#
# Use the following command template to link and compile:
# gcc <source.c> -lice -lm -lSDL2main -lSDL2 -lGL -o <output.exe>
#
# Windows example:
# gcc -I. demos/triangle.c -L. -lice -lm -lmingw32 -lSDL2main -lSDL2 -lopengl32 -o test.exe


# [HOW TO RUN ON DOS]
# The following files must be in the same directory as the DOS program:
# CWSDPMI.EXE
# GL.DXE
# GLIDE3X.DXE
#
# CWSDPMI.EXE should already be included but it can also be obtained from:
# https://sandmann.dotster.com/cwsdpmi/
#
# Only GL.DXE is required if you are using software rendering
# GLIDE3X.DXE is required if you are using hardware rendering
#
# GL.DXE can be found under lib/mesa/lib
#
# GLIDE3X.DXE can be obtained from:
# https://github.com/basecq/q2dos/tree/master/dos/3rdparty/lib_dxe/voodoo
# Make sure to choose the correct DXE driver for the Voodoo card you're using


CC       = i586-pc-msdosdjgpp-gcc
CFLAGS   = -std=gnu99 -Wall -Wextra -Wno-attributes -s -O2
PLATFORM = dos

all: libice.a

libice.a:
	$(CC) $(CFLAGS) -I. -c platform/$(PLATFORM)/*.c
	ar -rcs libice.a *.o

clean:
	$(RM) *.a *.o