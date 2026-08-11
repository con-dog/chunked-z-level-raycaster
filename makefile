CC ?= cc

TARGET := main
SOURCES := \
	main.c \
	config/sdl/sdl.c \
	utils/math-utils.c
OBJECTS := $(SOURCES:.c=.o)

CPPFLAGS += $(shell pkg-config sdl3 --cflags)
CFLAGS ?= -O3
CFLAGS += -std=c11 -Wall -Wextra
LDLIBS += $(shell pkg-config sdl3 --libs)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) $(LDLIBS) -o $@

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)

rebuild: clean all

.PHONY: all clean rebuild
