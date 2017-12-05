CC=g++
LDFLAGS= -lvl6180_pi -L /usr/local/lib

all:omnitest

omnitest: src/main.cpp
	$(CC) src/main.cpp -o omnitest $(LDFLAGS)

.PHONY: clean
clean:
	rm omnitest
