# ===============================
# Sort n Splode — build natiu Windows (MinGW-w64)
# ===============================
CXX      := g++
CXXFLAGS := -Wall -O3 -std=c++17

# Carpeta on tens descomprimit raylib (ha de tenir /include i /lib a dins)
RAYLIB_PATH := raylib

SRC_DIR  := src
BUILD_DIR := build

SRC := $(wildcard $(SRC_DIR)/*.cc)
HDR := $(wildcard $(SRC_DIR)/*.hh)
OBJ := $(SRC:$(SRC_DIR)/%.cc=$(BUILD_DIR)/%.o)

TARGET := game.exe

INCLUDES := -I$(SRC_DIR) -I$(RAYLIB_PATH)/include
LIBPATHS := -L$(RAYLIB_PATH)/lib

# -static-libgcc / -static-libstdc++ -> evita haver de distribuir les DLL del
# runtime de MinGW juntament amb el game.exe.
LDFLAGS := -lraylib -lopengl32 -lgdi32 -lwinmm -static-libgcc -static-libstdc++

# ===============================
# Regles
# ===============================
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBPATHS) $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cc $(HDR) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean
