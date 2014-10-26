OS := $(shell uname)
ARCH := $(shell uname -m)

LEAP_LIBRARY := /lib/Leap/

Sample: GestureControler.cpp
	$(CXX) -Wall -g GestureControler.cpp -o GestureControler -L$(LEAP_LIBRARY) -lLeap

clean:
	rm -rf GestureControler
