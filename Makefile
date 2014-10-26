OS := $(shell uname)
ARCH := $(shell uname -m)

LEAP_LIBRARY := /lib/Leap/

JazzHand: jazzhand.cpp
	$(CXX) -Wall -g jazzhand.cpp -o jazzhand -L$(LEAP_LIBRARY) -lLeap

clean:
	rm -rf jazzhand
