CXXFLAGS = -std=c++17
LDXXFLAGS = `sdl2-config --cflags --libs` -lSDL2 -lSDL2_image -pthread

BIN = bin/A-Star
SRC = src/*.cpp imgui/*.cpp imgui/backends/imgui_impl_sdl2.cpp imgui/backends/imgui_impl_sdlrenderer.cpp
INC = -Iimgui/

.PHONY: all run debug release clean

all: exec run clean

exec: $(SRC) src/*.hpp imgui/*.h imgui/backends/imgui_impl_sdl2.h imgui/backends/imgui_impl_sdlrenderer.h
	mkdir bin && \
	$(CXX) $(CXXFLAGS) -o $(BIN) $(SRC) $(LDXXFLAGS) $(INC)

run: $(BIN)
	./$?

debug: CXXFLAGS += -ggdb
debug: exec

release: CXXFLAGS += -O3
release: exec

clean:
	rm -rf bin
