# Compiler settings
CC := gcc
CCFLAGS := -std=gnu99 -s -mtune=i586 -O2 -fgnu89-inline -funroll-loops -ffast-math -fomit-frame-pointer -fcommon
LDFLAGS := -laldmb -ldumb -lm -lalleg

# Directories
SRC_DIR := src
OBJ_DIR := obj

# Find source and object files
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(subst $(SRC_DIR),$(OBJ_DIR),$(patsubst %.c,%.o,$(SRCS)))
OEDT := $(OBJ_DIR)/editor.o $(OBJ_DIR)/map.o

# Targets
TARGET1 := paciana.exe
TARGET2 := editor.exe

# Default build target
.PHONY: all
all: $(TARGET1) $(TARGET2)

# Linking
$(TARGET1): $(filter-out $(OBJ_DIR)/editor.o, $(OBJS))
	$(CC) $^ -o $@ $(LDFLAGS)

$(TARGET2): $(OEDT)
	$(CC) $^ -o $@ $(LDFLAGS)

# Compiling
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(OBJ_DIR)
	$(CC) -c $< -o $@ $(CCFLAGS)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

# Cleaning
clean:
	deltree /y $(OBJ_DIR)
	del $(TARGET1) $(TARGET2)

