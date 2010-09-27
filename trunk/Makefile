CPPFLAGS := -Wall -Wextra -O2 -Iinclude
LDFLAGS  := -static-libgcc -lopengl32 -mwindows

BUILD_DIR  := build
SOURCE_DIR := src

OBJ := $(BUILD_DIR)/Logger.o $(BUILD_DIR)/OpenGL.o \
	$(BUILD_DIR)/GLWindow.o $(BUILD_DIR)/main.o

BIN := lesson01.exe

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	g++ -c $(CPPFLAGS) $< -o $@

all: $(BIN)

$(BIN): $(OBJ)
	g++ -o $(BIN) $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(BIN)
