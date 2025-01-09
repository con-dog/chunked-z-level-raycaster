# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra $(shell pkg-config sdl3 sdl3-ttf sdl3-image libcjson --cflags)
LIBS = $(shell pkg-config sdl3 sdl3-ttf sdl3-image libcjson --libs)

# Directory structure
ASSETS_DIR = assets
CONFIG_DIR = config
DATA_DIR = data
IO_DIR = io
OBJECTS_DIR = objects
TYPES_DIR = types
UTILS_DIR = utils

# Include paths
INCLUDES = \
    -I$(ASSETS_DIR) \
    -I$(CONFIG_DIR) \
    -I$(DATA_DIR) \
    -I$(IO_DIR) \
    -I$(OBJECTS_DIR) \
    -I$(TYPES_DIR) \
    -I$(UTILS_DIR)

# Source files using find to recursively get all .c files
ASSETS_SRC = $(shell find $(ASSETS_DIR) -name '*.c')
CONFIG_SRC = $(shell find $(CONFIG_DIR) -name '*.c')
DATA_SRC = $(shell find $(DATA_DIR) -name '*.c')
IO_SRC = $(shell find $(IO_DIR) -name '*.c')
OBJECTS_SRC = $(shell find $(OBJECTS_DIR) -name '*.c')
TYPES_SRC = $(shell find $(TYPES_DIR) -name '*.c')
UTILS_SRC = $(shell find $(UTILS_DIR) -name '*.c')

# Debug prints
$(info =====================================)
$(info Source files found:)
$(info ASSETS_SRC = $(ASSETS_SRC))
$(info CONFIG_SRC = $(CONFIG_SRC))
$(info DATA_SRC = $(DATA_SRC))
$(info IO_SRC = $(IO_SRC))
$(info OBJECTS_SRC = $(OBJECTS_SRC))
$(info TYPES_SRC = $(TYPES_SRC))
$(info UTILS_SRC = $(UTILS_SRC))
$(info =====================================)

# All source files
SRC = \
    $(ASSETS_SRC) \
    $(CONFIG_SRC) \
    $(DATA_SRC) \
    $(IO_SRC) \
    $(OBJECTS_SRC) \
    $(TYPES_SRC) \
    $(UTILS_SRC)

# Object files
OBJ = $(SRC:.c=.o)

$(info Combined sources: $(SRC))
$(info Object files to be created: $(OBJ))

# Main target
TARGET = main

# Targets
all: $(TARGET)

$(TARGET): main.o $(OBJ)
	@echo "Linking $@ with objects: $^"
	$(CC) $^ $(LIBS) -o $@

# General rule for object files
%.o: %.c
	@echo "Compiling $< into $@"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Specific rule for main.o
main.o: main.c
	@echo "Compiling main.c"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "Cleaning project..."
	rm -f $(TARGET) $(OBJ) main.o
	find . -type f -name "*.o" -delete
	find . -type f -name "*.so" -delete
	find . -type f -name "*.a" -delete
	find . -type f -name "*.d" -delete
	find . -type f -name "*.gch" -delete

rebuild: clean all

.PHONY: all clean rebuild