SHELL := /bin/bash

# program name
TARGET = 403 

DIR        = $(CURDIR)
SRC_DIR    = source
OBJ_DIR    = object
HEADER_DIR = header

# compiler
CC      = gcc
CFLAGS  = -g -Wall 
LIBS    = -lm
INCLUDE = -I $(HEADER_DIR) 

# variables for DPL assignment rules 
ERROR_EXAMPLES   = $(shell echo error{1..5})
ERROR_EXAMPLES_X = $(addsuffix x,$(ERROR_EXAMPLES))
ERROR_EXAMPLES_P = $(addsuffix p,$(ERROR_EXAMPLES))
FEATURE_EXAMPLES   = arrays conditionals recursion iteration functions lambda dictionary problem return
FEATURE_EXAMPLES_X = $(addsuffix x,$(FEATURE_EXAMPLES))
FEATURE_EXAMPLES_P = $(addsuffix p,$(FEATURE_EXAMPLES))
EXAMPLES   = $(ERROR_EXAMPLES)   $(FEATURE_EXAMPLES)
EXAMPLES_X = $(ERROR_EXAMPLES_X) $(FEATURE_EXAMPLES_X)
EXAMPLES_P = $(ERROR_EXAMPLES_P) $(FEATURE_EXAMPLES_P)

# execute even if a file has these names
.PHONY: default all clean $(ERROR_EXAMPLES) $(ERROR_EXAMPLES_X)

#### COMPILING RULES #################################################

default: $(TARGET)
all: default

SOURCE  = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCE))
HEADERS = $(wildcard $(HEADER_DIR)/*.h)

test:
	@ echo $(SOURCE) $(OBJECTS) $(HEADERS)

$(OBJECTS): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.c $(HEADERS) 
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

# preserve target if make killed; do not delete intermediate object files
.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET) $(LIBS)
	chmod +x dpl

#### OTHER RULES #####################################################

# clean directory
clean:
	rm -f $(OBJECTS) $(TARGET)

#### DPL ASSIGNMENT RULES ############################################

$(EXAMPLES):
	@ echo Displaying input program for $@...
	@ cat $@.403

# "|| true" is to suppress error messages
$(EXAMPLES_X): $(TARGET)
	@- $(CURDIR)/$(TARGET) $(patsubst %x,%.403,$@) || true 

# "|| true" is to suppress error messages
$(EXAMPLES_P): $(TARGET)
	@- $(CURDIR)/$(TARGET) -p $(patsubst %p,%.403,$@) || true 
