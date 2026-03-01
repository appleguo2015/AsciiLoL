CXX = g++
CXXFLAGS = -std=c++11
TARGET = asciilol
SRC = main.cpp

all: $(TARGET)

$(TARGET): $(SRC) stb_image.h
	$(CXX) $(CXXFLAGS) -o $@ $< -lm

clean:
	rm -f $(TARGET)

.PHONY: all clean run