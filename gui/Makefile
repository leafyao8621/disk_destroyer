CC = g++
SRC = $(wildcard src/*.cpp) $(wildcard src/*/*.cpp)
RES_OUT_SRC = res_out/gdide.cpp
RES_OUT = res_out/gdide.o
OBJ = $(SRC:.c=.o)
CFLAGS = -Wall -Wextra -pedantic -Werror
GTK = `pkg-config --cflags --libs gtk+-3.0`
BIN = gdide


GLIB_COMPILE_RESOURCES = $(shell pkg-config --variable=glib_compile_resources gio-2.0)

resources = $(shell $(GLIB_COMPILE_RESOURCES) --sourcedir=res --generate-dependencies res/gdide.gresource.xml)

$(RES_OUT_SRC): $(resources)
	$(GLIB_COMPILE_RESOURCES) res/gdide.gresource.xml --target=$(RES_OUT_SRC) --sourcedir=res --generate-source

$(RES_OUT): $(RES_OUT_SRC)
	$(CC) -c $(RES_OUT_SRC) -o $(RES_OUT) $(GTK)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(GTK)

$(BIN): $(OBJ) $(RES_OUT)
	$(CC) $(OBJ) $(LIB) $(RES_OUT) -o $(BIN) $(GTK)

.PHONY: clean all
all: $(BIN) $(RES_OUT)
clean: $(BIN) $(RES_OUT)
	@rm $(OBJ) $(BIN) $(RES_OUT) $(RES_OUT_SRC)
