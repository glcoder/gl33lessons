CPPFLAGS := -std=c++0x -pedantic -Wall -Wextra -O3
LDFLAGS  := -static-libgcc -static-libstdc++

BUILD_DIR  := build
SOURCE_DIR := src

OBJ := $(BUILD_DIR)/crc32.o $(BUILD_DIR)/objcon.o $(BUILD_DIR)/main.o

BIN := objcon.exe

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	g++ -c $(CPPFLAGS) $< -o $@

all: $(BIN)

$(BIN): $(OBJ)
	g++ -o $(BIN) $(LDFLAGS) $(OBJ)

clean:
	rm -f $(OBJ) $(BIN)
