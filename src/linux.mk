# Compiler settings
CC := gcc
CCFLAGS := -std=gnu99 -fgnu89-inline -O2 -s -ffast-math -fomit-frame-pointer
LDFLAGS := -laldmb -ldumb -lm -lalleg

# Directories
SRC_DIR := src
OBJ_DIR := obj

# Find source and object files
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(subst $(SRC_DIR),$(OBJ_DIR),$(patsubst %.c,%.o,$(SRCS)))
OEDT := $(OBJ_DIR)/editor.o $(OBJ_DIR)/map.o

# Targets
TARGET1 := paciana
TARGET2 := editor

# Default build target
.PHONY: all
all: $(TARGET1) $(TARGET2)

# Linking
$(TARGET1): $(filter-out $(OBJ_DIR)/editor.o, $(OBJS))
	$(CC) $^ -o $@ $(LDFLAGS)

$(TARGET2): $(OEDT)
	$(CC) $^ -o $@ $(LDFLAGS)

# Compiling
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) -c $< -o $@ $(CCFLAGS)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Cleaning
clean:
	rm -rf $(OBJ_DIR)
	rm $(TARGET1) $(TARGET2)

