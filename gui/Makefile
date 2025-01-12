CC = g++
SRC = $(wildcard src/*.cpp) $(wildcard src/*/*.cpp) $(wildcard src/*/*/*.cpp)
RES_OUT_SRC = res_out/gdide.cpp
RES_OUT = res_out/gdide.o
OBJ = $(SRC:.cpp=.o)
GTK = $(shell pkg-config --cflags --libs gtk+-3.0)
BIN = gdide


GLIB_COMPILE_RESOURCES = $(shell pkg-config --variable=glib_compile_resources gio-2.0)

resources = $(shell $(GLIB_COMPILE_RESOURCES) --sourcedir=res --generate-dependencies res/gdide.gresource.xml)

$(RES_OUT_SRC): $(resources)
	$(GLIB_COMPILE_RESOURCES) res/gdide.gresource.xml --target=$(RES_OUT_SRC) --sourcedir=res --generate-source

$(RES_OUT): $(RES_OUT_SRC)
	$(CC) $(GTK) -g -c $(RES_OUT_SRC) -o $(RES_OUT)
%.o: %.cpp
	$(CC) $(GTK) -g -c $< -o $@

$(BIN): $(OBJ) $(RES_OUT)
	$(CC) $(GTK) $(OBJ) $(LIB) $(RES_OUT) -o $(BIN)

.PHONY: clean all install
all: $(BIN)
install: all
	@cp $(BIN) /usr/local/bin
	@cp desktop/gdide.desktop /usr/share/applications
clean:
	@rm $(OBJ)
	@rm $(BIN)
	@rm $(RES_OUT)
	@rm $(RES_OUT_SRC)
