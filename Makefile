CPP = g++

CSDKDIR	= HdacCSDK2

LIBS	= -lzmq -lpq -lpthread

INCLUDE	= -I/usr/include/postgresql

CPPFLAGS = $(INCLUDE) -Wall -W -std=c++11 -Wwrite-strings

SRCS	= main.cpp xdb.cpp xpub.cpp xsub.cpp 

OBJS	= $(SRCS:.cpp=.o)

ALIBS	= $(CSDKDIR)/lib/libcrypto.a \
	$(CSDKDIR)/lib/libentities.a \
	$(CSDKDIR)/lib/libhelpers.a \
	$(CSDKDIR)/lib/libkeys.a \
	$(CSDKDIR)/lib/libprimitives.a \
	$(CSDKDIR)/lib/librpc.a \
	$(CSDKDIR)/lib/libscripts.a \
	$(CSDKDIR)/lib/libstructs.a \
	$(CSDKDIR)/lib/libutils.a


all: $(CSDKDIR) tx tx2

$(CSDKDIR):
	cd $(CSDKDIR); make; cd ..
	ln -f -s $(CSDKDIR)/lib/*.a .

tx: $(OBJS)
	$(CPP) $(CPPFLAGS) -o $@ $(OBJS) $(LIBS) $(ALIBS)

tx2: $(OBJS)
	$(CPP) $(CPPFLAGS) -o $@ $(OBJS) $(LIBS) $(ALIBS)

clean:
	cd $(CSDKDIR); make clean; cd ..
	rm -f tx tx2 $(OBJS) *.o *.out
