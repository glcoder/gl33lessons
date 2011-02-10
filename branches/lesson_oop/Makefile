CPPFLAGS := -Wall -Wextra -O3 -Iinclude
LDFLAGS  := -static-libgcc -static-libstdc++ -lopengl32 -mwindows

BUILD_DIR  := build
SOURCE_DIR := src

TARGET := Win32

OBJ := $(BUILD_DIR)/Log.o \
	$(BUILD_DIR)/OpenGL.o \
	$(BUILD_DIR)/OpenGL_$(TARGET).o \
	$(BUILD_DIR)/Input_$(TARGET).o \
	$(BUILD_DIR)/Timer_$(TARGET).o \
	$(BUILD_DIR)/Window_$(TARGET).o \
	$(BUILD_DIR)/main.o

BIN := lesson.exe

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	g++ -c $(CPPFLAGS) $< -o $@

all: $(BIN)

$(BIN): $(OBJ)
	g++ -o $(BIN) $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(BIN)
