CPP = g++

CSDKDIR	= HdacCSDK2

SDKLIBS	= -lsecp256k1 -lssl -lcrypto -ldl -lboost_system -lboost_thread -lpthread
LIBS	= -lzmq -lpq 

INCLUDE	= -I/usr/include/postgresql -IHdacCSDK2/include

CPPFLAGS = $(INCLUDE) -Wall -W -std=c++11 -Wwrite-strings

SRCS	= main.cpp xdb.cpp xpub.cpp xsub.cpp xparams.cpp ex.cpp 

OBJS	= $(SRCS:.cpp=.o)

ALIBS	= \
	$(CSDKDIR)/lib/libhelpers.a \
	$(CSDKDIR)/lib/libkeys.a \
	$(CSDKDIR)/lib/libprimitives.a \
	$(CSDKDIR)/lib/librpc.a \
	$(CSDKDIR)/lib/libscripts.a \
	$(CSDKDIR)/lib/libentities.a \
	$(CSDKDIR)/lib/libstructs.a \
	$(CSDKDIR)/lib/libutils.a \
	$(CSDKDIR)/lib/libcrypto.a \


all: $(ALIBS) tx tx2

$(ALIBS): 
	cd $(CSDKDIR); make; cd ..
	ln -f -s $(CSDKDIR)/lib/*.a .

tx: $(OBJS)
	$(CPP) $(CPPFLAGS) -o $@ $(OBJS) $(ALIBS) $(SDKLIBS) $(LIBS)

tx2: $(OBJS)
	$(CPP) $(CPPFLAGS) -o $@ $(OBJS) $(ALIBS) $(SDKLIBS) $(LIBS) 

clean:
	cd $(CSDKDIR); make clean; cd ..
	rm -f tx tx2 $(OBJS) *.o *.out *.a
