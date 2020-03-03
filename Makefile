PROGRAM = tx
CPP = g++

LIB = -lzmq -lpq -lpthread

INCLUDE = -I/usr/include/postgresql

CPPFLAGS = $(INCLUDE) -Wall -W -std=c++11 -Wwrite-strings

SRC = main.cpp data_control.cpp publish.cpp subscribe.cpp 

OBJ = $(SRC:.cpp=.o)

all: $(PROGRAM)
$(PROGRAM): $(OBJ)
	$(CPP) $(CPPFLAGS) -o $@ $(OBJ) $(LIB)

clean:
	rm -f $(PROGRAM) $(OBJ) *.o
