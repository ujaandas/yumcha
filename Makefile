CXX = clang++
CXXFLAGS = -std=gnu++23 -Wall -framework Cocoa
TARGET = wm
SRC = $(wildcard *.cpp *.mm)

all: $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
