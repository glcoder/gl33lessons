CPPFLAGS := -Wall -Wextra -O3 -Iinclude
LDFLAGS  := -static-libgcc -static-libstdc++ -lopengl32 -mwindows

BUILD_DIR  := build
SOURCE_DIR := src

OBJ := $(BUILD_DIR)/common.o $(BUILD_DIR)/Logger.o \
	$(BUILD_DIR)/OpenGL.o $(BUILD_DIR)/GLWindow.o \
	$(BUILD_DIR)/Shader.o $(BUILD_DIR)/Texture.o \
	$(BUILD_DIR)/Mesh.o $(BUILD_DIR)/Camera.o \
	$(BUILD_DIR)/Light.o $(BUILD_DIR)/Material.o \
	$(BUILD_DIR)/main.o

BIN := lesson06.exe

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	g++ -c $(CPPFLAGS) $< -o $@

all: $(BIN)

$(BIN): $(OBJ)
	g++ -o $(BIN) $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(BIN)
