CXX := g++
DEBUG_FLAGS := -g3
RELEASE_FLAGS := -O3
CXXFLAGS := -std=c++20 -Wall -Wextra -pedantic
LDFLAGS := -L../lib -lPNG -Wl,-rpath,../lib

SRC := pngEncoder.cpp
OBJ := $(SRC:.cpp=.o)
TARGET := pngEncoder

.PHONY: all clean debug release

all: release

debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(TARGET)

release: CXXFLAGS += $(RELEASE_FLAGS)
release: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ) $(TARGET)