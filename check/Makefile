#
# Makefile for the Spica C++ unit library test program.
#

CXX=g++
CXXFLAGS=-std=c++20 -Wall -I.. -c -O
LINKFLAGS=
SOURCES=u_tests.cpp        \
	BinomialHeap_tests.cpp \
	BoundedList_tests.cpp  \
	Graph_tests.cpp        \
	RexxString_tests.cpp   \
	sort_tests.cpp         \
	Timer_tests.cpp        \
	VeryLong_tests.cpp
OBJECTS=$(SOURCES:.cpp=.o)
LIBRARY=libSpicaCpp.a
EXECUTABLE=u_tests

%.o:	%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@
	
all:	$(EXECUTABLE)

$(EXECUTABLE):	$(OBJECTS) ../$(LIBRARY)
	$(CXX) $(LINKFLAGS) -o $(EXECUTABLE) $(OBJECTS) -L.. -lSpicaCpp

# File Dependencies
###################

# Module dependencies -- Produced with 'depend' on Thu Jul 13 11:08:49 2023


u_tests.o:	u_tests.cpp u_tests.hpp ../UnitTestManager.hpp 

BinomialHeap_tests.o:	BinomialHeap_tests.cpp ../BinomialHeap.hpp u_tests.hpp ../UnitTestManager.hpp 

BoundedList_tests.o:	BoundedList_tests.cpp ../BoundedList.hpp u_tests.hpp ../UnitTestManager.hpp 

Graph_tests.o:	Graph_tests.cpp ../Graph.hpp u_tests.hpp ../UnitTestManager.hpp 

RexxString_tests.o:	RexxString_tests.cpp ../environ.hpp ../RexxString.hpp 

sort_tests.o:	sort_tests.cpp ../sorters.hpp u_tests.hpp ../UnitTestManager.hpp 

VeryLong_tests.o:	VeryLong_tests.cpp ../VeryLong.hpp u_tests.hpp ../UnitTestManager.hpp 

Timer_tests.o:	Timer_tests.cpp ../environ.hpp ../Timer.hpp 


# Additional Rules
##################
clean:
	rm -f *.bc *.o $(EXECUTABLE) *.s *.ll *~
