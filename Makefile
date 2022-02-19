CFLAGS = -std=c++17 -O2 -ggdb
LDFLAGS = #-lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
LDFLAGS = `sdl2-config --cflags --libs` -lSDL2

bin/A-Star: src/*.cpp src/*.hpp
	mkdir bin && g++ $(CFLAGS) -o ./bin/A-Star src/*.cpp $(LDFLAGS)

.PHONY: test clean

test: bin/A-Star 
	./bin/A-Star

clean:
	rm -rf bin
