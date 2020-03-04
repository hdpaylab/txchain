CPP = g++

LIBS = -lzmq -lpq -lpthread

INCLUDE = -I/usr/include/postgresql

CPPFLAGS = $(INCLUDE) -Wall -W -std=c++11 -Wwrite-strings

SRCS = main.cpp xdb.cpp xpub.cpp xsub.cpp 

OBJS = $(SRCS:.cpp=.o)


all: tx tx2

tx: $(OBJS)
	$(CPP) $(CPPFLAGS) -o $@ $(OBJS) $(LIBS)

tx2: $(OBJS)
	$(CPP) $(CPPFLAGS) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f tx tx2 $(OBJS) *.o *.out
