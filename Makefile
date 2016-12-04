CPPFLAGS = -std=c++1y -I /usr/local/Cellar/boost/1.60.0_2/include

all: main

main: main.cpp
	g++ $(CPPFLAGS) -o $@ $<

main.pp: main.cpp
	g++ -E $(CPPFLAGS) -o $@ $<

main1: main1.cpp
	g++ $(CPPFLAGS) -o $@ $<
