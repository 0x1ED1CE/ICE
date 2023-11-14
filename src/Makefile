CC=gcc \
-std=gnu99 \
-Wall \
-Wextra \
-Wno-attributes \
-s \
-O2

PLATFORM = dos
BIN      = ../bin/ice

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
	ice.h \
	main.c \
	$(PLATFORM).o \
	etch.o \
	iso_vm.o \
	iso_aux.o \
	api/iso/iso_ice_audio.h \
	api/iso/iso_ice_video.h \
	api/iso/iso_ice_clock.h
	
	$(CC) \
	-I../src \
	main.c \
	$(PLATFORM).o \
	etch.o \
	iso_vm.o -lm \
	iso_aux.o \
	-o $(BIN)

dos.o: \
	ice.h \
	modules/audio.h \
	modules/video.h \
	modules/clock.h \
	lib/stb/stb_image.h \
	platform/dos/ice.c \
	platform/dos/audio.h \
	platform/dos/video.h \
	platform/dos/clock.h
	
	$(CC) \
	-I../src \
	-Iplatform/dos \
	-c platform/dos/ice.c \
	-o dos.o

etch.o: \
	lib/etch/etch.h \
	lib/etch/etch_texture.c
	
	$(CC) \
	-Ilib/etch \
	-c lib/etch/etch_texture.c \
	-o etch.o

iso_vm.o: \
	lib/iso/iso.h \
	lib/iso/iso_vm.h \
	lib/iso/iso_vm.c
	
	$(CC) \
	-Ilib/iso \
	-c lib/iso/iso_vm.c

iso_aux.o: \
	lib/iso/iso.h \
	lib/iso/iso_vm.h \
	lib/iso/iso_aux.h \
	lib/iso/iso_aux.c \
	
	$(CC) \
	-Ilib/iso \
	-c lib/iso/iso_aux.c

clean:
	$(RM) \
	etch.o \
	dos.o \
	iso_vm.o \
	iso_aux.o \
	iso_ice_audio.o \
	iso_ice_video.o \
	iso_ice_clock.o