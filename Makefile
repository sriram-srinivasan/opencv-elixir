CC=g++
CFLAGS = --std=c++11 -g -Wall -Wextra -Wno-unused-parameter -pedantic -I/usr/local/include/opencv4/

ERL_ROOT=$(shell erl -noinput -noshell -eval 'io:format(code:root_dir()).' -s erlang halt)
ERL_EI_INCLUDE_DIR=$(ERL_ROOT)/usr/include
ERL_EI_LIBDIR=$(ERL_ROOT)/usr/lib
SO = c_lib/cam.so

#Linux
#LDFLAGS += -fPIC -shared -L/usr/local/lib -ljpeg -lopencv_core -lopencv_imgcodecs

#Mac
LDFLAGS += -fPIC -bundle -flat_namespace -undefined suppress \
         -L/usr/local/lib -ljpeg -lopencv_core -lopencv_imgcodecs -lopencv_videoio -lopencv_objdetect

C_OBJ_DIR=_build/c_obj
$(shell mkdir -p $(C_OBJ_DIR))
$(shell mkdir -p c_lib)

all: $(SO)

# Set Erlang-specific compile and linker flags
ERL_CFLAGS ?= -I$(ERL_EI_INCLUDE_DIR)
ERL_LDFLAGS ?= -L$(ERL_EI_LIBDIR) -lei

SRC =  $(wildcard c_src/*.cpp)
HEADERS =$(wildcard c_src/*.hpp)
OBJ = $(SRC:c_src/%.cpp=$(C_OBJ_DIR)/%.o)

$(OBJ): $(HEADERS) $(SRC) Makefile

$(C_OBJ_DIR)/%.o: c_src/%.cpp
	$(CC) -c $(ERL_CFLAGS) $(CFLAGS) -o $@ $<

$(SO): $(OBJ)
	$(CC) -o $@ $(ERL_LDFLAGS) $(LDFLAGS) $^

clean:
	$(RM) $(SO) $(OBJ)

.PHONY: all clean calling_from_make install
