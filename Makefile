CXX = g++
CXXFLAGS = -Wall -O2
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -O2

SRC = main.cc GameManager.cc GameObject.cc
HDR = GameManager.hh GameObject.hh
OBJ = $(SRC:.cc=.o)

TARGET = game

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cc $(HDR)
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJ) $(TARGET)
