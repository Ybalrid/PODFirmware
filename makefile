CC=g++
LDFLAGS= -lvl6180_pi -L /usr/local/lib
CFLAGS= -g -Wall -Wpedantic
IFLAGS= -Iinclude

all:omnitest

omnitest: src/main.cpp include/sensor.hpp
	$(CC) $(CFLAGS) $(IFLAGS) src/main.cpp -o omnitest $(LDFLAGS)

.PHONY: clean
clean:
	rm omnitest
