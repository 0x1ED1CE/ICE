PLATFORM ?= SDL
BIN      ?= ../bin/ice.exe
MAIN     ?= demos/3d.c

ifeq ($(PLATFORM),DOS)
    CC   = i586-pc-msdosdjgpp-gcc -std=gnu99 -Wall -Wextra -Wno-attributes -s -O2
    LIBS = -lm -ligl -lgl
else ifeq ($(PLATFORM),SDL)
    CC   = gcc -std=gnu99 -Wall -Wextra -Wno-attributes -s -O2
    LIBS = -lm -lmingw32 -lSDL2main -lSDL2 -lopengl32 -mwindows
endif

all: $(PLATFORM)

DOS: \
	ice.o \
	iso_vm.o \
	bindings/iso/iso_aux.h \
	bindings/iso/iso_ice_clock.h \
	bindings/iso/iso_ice_video.h \
	bindings/iso/iso_ice_audio.h \
	bindings/iso/iso_ice_input.h \
	bindings/iso/iso_ice_cache.h \
	platform/dos/clock.h \
	platform/dos/video.h \
	platform/dos/mesa.h \
	platform/dos/audio.h \
	platform/dos/sb16.h \
	platform/dos/input.h \
	platform/dos/cache.h \
	platform/dos/ice.c \
	$(MAIN)

	$(CC) \
	-I../src \
	-I../src/demos \
	$(MAIN) \
	ice.o \
	iso_vm.o \
	$(LIBS) \
	-o $(BIN)

SDL: \
	ice.o \
	iso_vm.o \
	bindings/iso/iso_aux.h \
	bindings/iso/iso_ice_clock.h \
	bindings/iso/iso_ice_video.h \
	bindings/iso/iso_ice_audio.h \
	bindings/iso/iso_ice_input.h \
	bindings/iso/iso_ice_cache.h \
	platform/sdl/clock.h \
	platform/sdl/video.h \
	platform/sdl/audio.h \
	platform/sdl/input.h \
	platform/sdl/cache.h \
	platform/sdl/ice.c \
	$(MAIN)
	
	$(CC) \
	-I../src \
	-I../src/demos \
	$(MAIN) \
	ice.o \
	iso_vm.o \
	$(LIBS) \
	-o $(BIN)

ice.o: \
	ice.h \
	modules/clock.h \
	modules/video.h \
	modules/audio.h \
	modules/input.h \
	modules/cache.h \
	lib/stb/stb_image.h
	
	$(CC) \
	-I../src \
	-Iplatform/$(PLATFORM) \
	-c platform/$(PLATFORM)/ice.c \
	-o ice.o

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

glad.o: \
	lib/glad/glad.h \
	lib/glad/glad.c \
	lib/glad/khrplatform.h

	$(CC) \
	-Ilib/glad \
	-c lib/glad/glad.c

mesa:
	$(MAKE) \
	-f Makefile.DJ \
	-C lib/mesa \
	CC=i586-pc-msdosdjgpp-gcc \
	FX=1

clean:
	$(RM) \
	etch.o \
	iso_vm.o \
	glad.o \
	ice.o
	
#	$(MAKE) \
#	-f Makefile.DJ \
#	-C F:\etc\projects\ice\v8\src\lib\mesa \
#	clean