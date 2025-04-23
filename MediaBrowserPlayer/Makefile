# ==================== Compiler and flags ====================
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -g \
            -I./include \
            -I./include/model \
            -I./include/view \
            -I./include/controller

LDFLAGS := -lSDL2 -lSDL2_mixer -ltag

# ==================== Folder structure ====================
SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin
TARGET := $(BIN_DIR)/MediaBrowserPlayer

# ==================== Source and object files ====================
SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# ==================== Rules ====================
all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ==================== Utilities ====================
run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

rebuild: clean all

# ==================== Phony ====================
.PHONY: all clean run rebuild