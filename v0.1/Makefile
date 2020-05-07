CPP	= g++

CSDKDIR	= ../HdacCSDK2

SDKLIBS	= -lsecp256k1 -lssl -lcrypto -ldl -lboost_system -lboost_thread -lpthread
LIBS	= -lzmq -lpq -lleveldb

INCLUDE	= -I/usr/include/postgresql -I../HdacCSDK2/include -I../lib

CPPFLAGS = -O2 $(INCLUDE) -Wall -W -std=c++11 -Wwrite-strings -Wno-unused-parameter

SRCS	= xmain.cpp xpub.cpp xsub.cpp \
	  xverify.cpp xldbio.cpp params.cpp \
	  ../lib/leveldb.cpp ../lib/xsz.cpp util.cpp

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


all: lib tx 

lib: $(ALIBS)

$(ALIBS): 
	cd $(CSDKDIR); make; cd ..
#	ln -f -s $(CSDKDIR)/lib/*.a .

tx: $(OBJS)
	$(CPP) $(CPPFLAGS) -o $@ $(OBJS) $(ALIBS) $(SDKLIBS) $(LIBS)

clean:
	rm -f tx $(OBJS) *.o *.out *.ver *.a *.log
	rm -rf test.db 
