CXX_DEFAULT ?= g++-15
CXX := $(CXX_DEFAULT)
CXXFLAGS = -Wall -Wextra -I$(INC_DIR) -std=c++20 -O3 # do not change to := , lazy evaluating INC_DIR

SRC_DIR  := src
OBJ_DIR  := obj
BIN_DIR  := bin
INC_DIR  := include
LIB_PATH := /usr/bin
BIN_NAME := debugger

TARGET := $(BIN_DIR)/$(BIN_NAME)

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
SRCS := $(filter-out $(SRC_DIR)/main.cpp, $(SRCS))
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

all : check_dirs $(TARGET)

install:
	install -Dm 755 $(TARGET) $(LIB_PATH)/$(BIN_NAME)

$(TARGET) : $(SRC_DIR)/main.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

check_dirs:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR) 

.PHONY: clean check_dirs all install
