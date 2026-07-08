CC = gcc
CFLAGS = -Wall -Wextra -O2 -Isrc/core -Isrc/entidades -Isrc/ambiente
LDFLAGS = 

SRCDIR = src
OBJDIR = obj
BINDIR = .

SRC_DIRS = $(SRCDIR)/core $(SRCDIR)/entidades $(SRCDIR)/ambiente

SOURCES = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.c))
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))
EXECUTABLE = $(BINDIR)/aventureiro

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@) 2>/dev/null || true
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(EXECUTABLE)

.PHONY: all clean
