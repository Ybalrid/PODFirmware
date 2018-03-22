CC=g++
LDFLAGS= -lvl6180_pi -lmma8451_pi -L /usr/local/lib `sdl2-config --libs` -lSDL2_ttf
CFLAGS= -g -Wall -Wpedantic -O3 `sdl2-config --cflags`
IFLAGS= -Iinclude

all:omnitest

omnitest: src/main.cpp include/sensor.hpp
	$(CC) $(CFLAGS) $(IFLAGS) src/main.cpp -o omnitest $(LDFLAGS)

.PHONY: clean
clean:
	rm omnitest
