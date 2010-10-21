CPPFLAGS := -pedantic -std=c99 -Wall -Wextra -O3 -Iinclude
LDFLAGS  := 

BUILD_DIR  := build
SOURCE_DIR := src

OBJ := $(BUILD_DIR)/crc32.o $(BUILD_DIR)/main.o

BIN := modcon.exe

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	gcc -c $(CPPFLAGS) $< -o $@

all: $(BIN)

$(BIN): $(OBJ)
	gcc -o $(BIN) $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(BIN)
