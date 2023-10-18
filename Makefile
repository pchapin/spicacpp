#
# Makefile for the Spica C++ library.
#

CXX=g++
CXXFLAGS=-std=c++20 -Wall -c -O
LIBLINK=ar
LIBLINKFLAGS=-r -c
LINK=g++
LINKFLAGS=
SOURCES=base64.cpp       \
	BitFile.cpp          \
	config.cpp           \
	crc.cpp              \
	Date.cpp             \
	get_switch.cpp       \
	RexxString.cpp       \
	string_utilities.cpp \
	synchronize.cpp      \
	Timer.cpp            \
	UnitTestManager.cpp  \
	VeryLong.cpp         \
	tests/BinomialHeap_tests.cpp \
	tests/BoundedList_tests.cpp  \
	tests/Graph_tests.cpp        \
	tests/RexxString_tests.cpp   \
	tests/sort_tests.cpp         \
	tests/Timer_tests.cpp        \
	tests/VeryLong_tests.cpp
OBJECTS=$(SOURCES:.cpp=.o)
LIBRARY=libSpicaCpp.a
EXECUTABLE=u_tests

%.o:	%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@
	
all:	$(EXECUTABLE)

$(LIBRARY):	$(OBJECTS)
	$(LIBLINK) $(LIBLINKFLAGS) $@ $(OBJECTS)

$(EXECUTABLE):	u_tests.o $(LIBRARY)
	$(LINK) $(LINKFLAGS) -o $(EXECUTABLE) u_tests.o -L. -lSpicaCpp

# File Dependencies
###################

# Module dependencies -- Produced with 'depend' on Wed Oct 18 14:32:14 2023

base64.o:	base64.cpp base64.hpp 

BitFile.o:	BitFile.cpp BitFile.hpp 

config.o:	config.cpp config.hpp 

crc.o:	crc.cpp crc.hpp 

Date.o:	Date.cpp Date.hpp 

get_switch.o:	get_switch.cpp get_switch.hpp 

RexxString.o:	RexxString.cpp environ.hpp RexxString.hpp synchronize.hpp 

string_utilities.o:	string_utilities.cpp string_utilities.hpp 

Timer.o:	Timer.cpp Timer.hpp environ.hpp 

UnitTestManager.o:	UnitTestManager.cpp UnitTestManager.hpp 

VeryLong.o:	VeryLong.cpp VeryLong.hpp 

#####
# Dependencies below are managed by hand.

tests/BinomialHeap_tests.o:	tests/BinomialHeap_tests.cpp BinomialHeap.hpp

tests/BoundedList_tests.o:	tests/BoundedList_tests.cpp BoundedList.hpp

tests/Graph_tests.o:	tests/Graph_tests.cpp Graph.hpp

tests/RexxString_tests.o:	tests/RexxString_tests.cpp RexxString.hpp

tests/sort_tests.o:	tests/sort_tests.cpp sorters.hpp

tests/Timer_tests.o:	tests/Timer_tests.cpp Timer.hpp

tests/VeryLong_tests.o:	tests/VeryLong_tests.cpp VeryLong.hpp

u_tests.o:	u_tests.cpp u_tests.hpp UnitTestManager.hpp

# Additional Rules
##################
clean:
	rm -f *.bc *.o $(LIBRARY) $(EXECUTABLE) *.s *.ll *~
	(cd tests; make clean)
