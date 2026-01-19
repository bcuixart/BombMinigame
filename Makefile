# ===============================
# Configuració comuna
# ===============================
CXX      ?= g++
CXXFLAGS = -Wall -O3 -std=c++17

SRC = main.cc GameManager.cc GameObject.cc Bomb.cc BombHouse.cc
HDR = GameManager.hh GameObject.hh Bomb.hh BombHouse.hh
OBJ = $(SRC:.cc=.o)

TARGET = game

# ===============================
# Plataforma
# ===============================
PLATFORM ?= linux

ifeq ($(PLATFORM),linux)
    TARGET := game
    LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

ifeq ($(PLATFORM),windows)
    TARGET := game.exe
    CXX := x86_64-w64-mingw32-g++
    LDFLAGS = -lraylib -lopengl32 -lgdi32 -lwinmm
endif

# ===============================
# Regles
# ===============================
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cc $(HDR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
