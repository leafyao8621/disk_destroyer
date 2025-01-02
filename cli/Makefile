CPP = g++
CPPFLAGS = -Wall -Wextra -Werror -pedantic
SRC = $(wildcard src/*.cpp) $(wildcard src/*/*.cpp)
OBJ = $(SRC:.cpp=.o)
BIN = dide

%.o: %.cpp
	$(CPP) $(CPPFLAGS) -O3 -c $< -o $@

$(BIN): $(OBJ)
	$(CPP) $(OBJ) -o $(BIN)

.PHONY: clean valgrind
clean:
	@rm $(OBJ)
	@rm $(BIN)
valgrind: $(BIN)
	@valgrind -s --leak-check=full --show-leak-kinds=all ./$(BIN) -p test -v ~/dummy.img
