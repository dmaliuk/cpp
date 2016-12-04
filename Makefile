CPPFLAGS = -std=c++1y -I /usr/local/Cellar/boost/1.60.0_2/include
CPPFLAGS1 = -std=c++1y -I /Users/dmaliuk/repos/nnvm/include/dmlc

all: main

main: main.cpp
	g++ $(CPPFLAGS) -o $@ $<

main.pp: main.cpp
	g++ -E $(CPPFLAGS) -o $@ $<

main1: main1.cpp
	g++ $(CPPFLAGS1) -o $@ $<
