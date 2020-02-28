PROGRAM = speedup
CPP = g++

LIB = -lzmq -lpq

INCLUDE = -I/usr/include/postgresql

CPPFLAGS = $(INCLUDE) -Wall -W -std=c++11 -Wwrite-strings

SRC = main.cpp data_get.cpp publish.cpp

OBJ = $(SRC:.cpp=.o)

all: $(PROGRAM)
$(PROGRAM): $(OBJ)
	$(CPP) $(CPPFLAGS) -o $@ $(OBJ) $(LIB)

clean:
	rm -f $(PROGRAM) $(OBJ) *.o
