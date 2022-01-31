# ✨ Daniël Kamp's supersonic Makefile, oh yeah ✨

CC=g++
CFLAGS = -I/usr/local/include -Wall -fexceptions -std=c++20 -framework CoreMIDI -framework CoreAudio -framework CoreFoundation
LDFLAGS= -ljack -lncurses

SRC_DIR=src
OBJ_DIR=obj
SOURCES=$(wildcard $(SRC_DIR)/*.cpp)
OBJECTS=$(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

# Make all
all: donut

# Link all
donut: $(OBJECTS)
	$(CC) -o $@ $(CFLAGS) $^ $(LDFLAGS)

# Separate rule for main.cpp since it doesn't have a header
obj/main.o: src/main.cpp src/Header/Global.h src/Header/TestSynth.h
	mkdir -p $(OBJ_DIR)
	$(CC) -c $< $(CFLAGS) -o $@

# Make all .cpp source files into .o files, like magic ✨
obj/%.o: src/%.cpp src/Header/%.h
	$(CC) -c $< $(CFLAGS) -o $@

# Install to system path
install:
	mv donut /usr/var/bin/

# Clean up
clean:
	rm donut *.o

TOP_TARGETS := all clean

SUBDIRS := $(wildcard */.)

$(TOP_TARGETS): $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: $(TOPTARGETS) $(SUBDIRS)