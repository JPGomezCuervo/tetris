# Compiler and flags
#-fsanitize=address
CC = gcc
CFLAGS = -Wall -Wextra -ggdb  -Wunused-function -Wmissing-prototypes -Wunreachable-code
LDLIBS = -lraylib -lm

# Target executable
TARGET = tetris

# Source files
SRCS = main.c tetris.c

# Object files (automatically generated from source files)
OBJS = $(SRCS:.c=.o)

# Rule to build the final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDLIBS)

# Rule to compile .c files into .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(TARGET) $(OBJS)
