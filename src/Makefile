PLATFORM = dos
BIN      = ../bin/ice
MAIN     = demos/3d.c

CC=gcc \
-std=gnu99 \
-Wall \
-Wextra \
-Wno-attributes \
-s \
-O2

ifeq ($(PLATFORM),dos)
CC=i586-pc-msdosdjgpp-gcc \
-std=gnu99 \
-Wall \
-Wextra \
-Wno-attributes \
-s \
-O2
BIN=../bin/ice.exe
endif

all: \
	$(PLATFORM).o \
	etch.o \
	iso_vm.o \
	bindings/iso/iso_aux.h \
	bindings/iso/iso_ice_clock.h \
	bindings/iso/iso_ice_video.h \
	bindings/iso/iso_ice_audio.h \
	bindings/iso/iso_ice_input.h \
	bindings/iso/iso_ice_cache.h \
	$(MAIN)
	
	$(CC) \
	-I../src \
	$(MAIN) \
	dos.o \
	etch.o \
	iso_vm.o -lm \
	-o $(BIN)

dos.o: \
	ice.h \
	modules/clock.h \
	modules/video.h \
	modules/audio.h \
	modules/input.h \
	modules/cache.h \
	lib/stb/stb_image.h \
	platform/dos/clock.h \
	platform/dos/video.h \
	platform/dos/audio.h \
	platform/dos/input.h \
	platform/dos/cache.h \
	platform/dos/ice.c
	
	$(CC) \
	-I../src \
	-Iplatform/dos \
	-c platform/dos/ice.c \
	-o dos.o

etch.o: \
	lib/etch/etch.h \
	lib/etch/etch.c
	
	$(CC) \
	-Ilib/etch \
	-c lib/etch/etch.c \
	-o etch.o

iso_vm.o: \
	lib/iso/iso.h \
	lib/iso/iso_vm.h \
	lib/iso/iso_vm.c
	
	$(CC) \
	-Ilib/iso \
	-c lib/iso/iso_vm.c

clean:
	$(RM) \
	etch.o \
	iso_vm.o \
	dos.o