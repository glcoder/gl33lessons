CFLAGS  := -pedantic -Wall -O2 -I../../../include -I../../../src/common
LDFLAGS := -static-libgcc -lopengl32 -mwindows

COMMON_BUILD_DIR := ../../../build/common
COMMON_SRC_DIR := ../../../src/common

COMMON_OBJ := \
	$(COMMON_BUILD_DIR)/Logger.o \
	$(COMMON_BUILD_DIR)/OpenGL.o \
	$(COMMON_BUILD_DIR)/GLWindow.o

$(COMMON_BUILD_DIR)/%.o: $(COMMON_SRC_DIR)/%.cpp
	g++ -c $(CFLAGS) $< -o $@

$(LESSON_BUILD_DIR)/%.o: $(LESSON_SRC_DIR)/%.cpp
	g++ -c $(CFLAGS) $< -o $@

all: $(BIN)

$(BIN): $(COMMON_OBJ) $(LESSON_OBJ)
	g++ -o $(BIN) $(COMMON_OBJ) $(LESSON_OBJ) $(LDFLAGS)

clean:
	rm -f $(COMMON_OBJ) $(LESSON_OBJ) $(BIN)
