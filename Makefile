OS := $(shell uname)
ARCH := $(shell uname -m)

LEAP_LIBRARY := /lib/Leap/

JazzHand: jazzhand.cpp task_runner.cpp
	$(CXX) -Wall -g jazzhand.cpp task_runner.cpp -o jazzhand -L$(LEAP_LIBRARY) -lLeap

clean:
	rm -rf jazzhand
