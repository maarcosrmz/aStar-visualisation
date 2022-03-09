CXXFLAGS = -std=c++17
LDXXFLAGS = `sdl2-config --cflags --libs` -lSDL2 -lSDL2_image -pthread

BIN = bin/A-Star
SRC = src/*.cpp src/**/*.cpp

.PHONY: all run debug release clean

all: exec run clean

exec: $(SRC) src/*.hpp src/**/*.h
	mkdir bin && \
	$(CXX) $(CXXFLAGS) -o $(BIN) $(SRC) $(LDXXFLAGS)

run: $(BIN)
	./$?

debug: CXXFLAGS += -ggdb
debug: exec

release: CXXFLAGS += -O3
release: exec

clean:
	rm -rf bin
