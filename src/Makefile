# [HOW TO BUILD]
# First you need DJGPP installed as well as MinGW if you're on Windows:
# https://github.com/andrewwutw/build-djgpp/releases
#
# Once installed, run `make` in this directory
# Make will generate libice.a


# [HOW TO LINK AND COMPILE A DOS PROGRAM]
# There are 4 static libraries needed to link against:
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


# [HOW TO BUILD MESA] (OPTIONAL)
# This is only a brief guide on how to recompile Mesa 6.2.
# For more detailed instructions, refer to: https://github.com/sezero/MesaFX-6.2
#
# dxe3gen.exe is needed in order to create libigl.a (included in DJGPP)
# libigl.a is only needed if you wish to use 3dfx Voodoo cards. If you are 
# using software rendering then only libgl.a will be created.
#
# Run the following commands to build Mesa:
# cd lib/mesa
# make -f Makefile.DJ FX=1
#
# If you want to enable software rendering, set FX=0
#
# You should now have libgl.a, libigl.a and GL.DXE under lib/mesa/lib


# [HOW TO RUN]
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


PLATFORM = dos
CC       = i586-pc-msdosdjgpp-gcc -std=gnu99 -Wall -Wextra -Wno-attributes -s -O2

all: libice.a

libice.a:
	$(CC) -I. -c platform/$(PLATFORM)/*.c
	ar -rcs libice.a *.o

clean:
	$(RM) *.a *.o
