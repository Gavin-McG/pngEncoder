CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -pedantic -fPIC
LDFLAGS := -shared
SRC_DIR := src
LIB_DIR := lib

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(SRC_DIR)/%.o,$(SRCS))
TARGET := $(LIB_DIR)/libPNG.so

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(LIB_DIR)
	$(CXX) $(LDFLAGS) -o $@ $^

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	$(RM) -r $(LIB_DIR) $(SRC_DIR)/*.o