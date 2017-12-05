CC=g++
LDFLAGS= -lvl6180_pi -L /usr/local/lib

all:omnitest

omnitest:main.cpp
	$(CC) main.cpp -o omnitest $(LDFLAGS)

.PHONY: clean
clean:
	rm omnitest
