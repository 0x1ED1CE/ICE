PLATFORM ?= DOS
BIN      ?= ../bin/ice.exe
MAIN     ?= demos/triangle.c

ifeq ($(PLATFORM),DOS)
    CC   = i586-pc-msdosdjgpp-gcc -std=gnu99 -Wall -Wextra -Wno-attributes -s -O2
    LIBS = -lm -Llib/mesa/lib -ligl -lgl
else ifeq ($(PLATFORM),SDL)
    CC   = gcc -std=gnu99 -Wall -Wextra -Wno-attributes -s -O2
    LIBS = -lm -lmingw32 -lSDL2main -lSDL2 -lopengl32 -mwindows
endif

all: \
	ice.o \
	iso_vm.o

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