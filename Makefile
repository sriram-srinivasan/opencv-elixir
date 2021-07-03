# Makefile for building the NIF
#
# Makefile targets:
#
# all/install   build and install the NIF
# clean         clean build products and intermediates
#
# Variables to override:
#
# MIX_APP_PATH  path to the build directory
#
# CC            C compiler
# CROSSCOMPILE	crosscompiler prefix, if any
# CFLAGS	compiler flags for compiling all C files
# ERL_CFLAGS	additional compiler flags for files using Erlang header files
# ERL_EI_INCLUDE_DIR include path to ei.h (Required for crosscompile)
# ERL_EI_LIBDIR path to libei.a (Required for crosscompile)
# LDFLAGS	linker flags for linking all binaries
# ERL_LDFLAGS	additional linker flags for projects referencing Erlang libraries

CC=g++
CFLAGS ?= --std=c++11 -g -Wall -Wextra -Wno-unused-parameter -pedantic -I/usr/local/include/opencv4/
BUILD=obj

ERL_EI_INCLUDE_DIR=/Users/s/.asdf/installs/erlang/23.3.1/usr/include
ERL_EI_LIBDIR=/Users/s/.asdf/installs/erlang/23.3.1/usr/lib
LDFLAGS += -fPIC -bundle -flat_namespace -undefined suppress -L/usr/local/lib -ljpeg -lpng -lopencv_core -lopencv_imgcodecs
CFLAGS += -fPIC 

NIF = cam.so

all: $(NIF)

# Set Erlang-specific compile and linker flags
ERL_CFLAGS ?= -I$(ERL_EI_INCLUDE_DIR)
ERL_LDFLAGS ?= -L$(ERL_EI_LIBDIR) -lei

SRC =  $(wildcard clib/*.cpp)
HEADERS =$(wildcard clib/*.hpp)
OBJ = $(SRC:clib/%.cpp=$(BUILD)/%.o)

$(OBJ): $(HEADERS) $(SRC) Makefile

$(BUILD)/%.o: clib/%.cpp
	$(CC) -c $(ERL_CFLAGS) $(CFLAGS) -o $@ $<

$(NIF): $(OBJ)
	$(CC) -o $@ $(ERL_LDFLAGS) $(LDFLAGS) $^

clean:
	$(RM) $(NIF) $(OBJ)


.PHONY: all clean calling_from_make install
