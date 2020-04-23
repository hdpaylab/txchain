CPP	= g++

CSDKDIR	= HdacCSDK2

SDKLIBS	= -lsecp256k1 -lssl -lcrypto -ldl -lboost_system -lboost_thread -lpthread
LIBS	= -lzmq -lpq -lleveldb

INCLUDE	= -I/usr/include/postgresql -IHdacCSDK2/include -Ilib

CPPFLAGS = -O2 $(INCLUDE) -Wall -W -std=c++11 -Wwrite-strings -Wno-unused-parameter

SRCS	= xmain.cpp xpub.cpp xsub.cpp \
	  xverify.cpp xldbio.cpp params.cpp \
	  lib/leveldb.cpp lib/xsz.cpp util.cpp

OBJS	= $(SRCS:.cpp=.o)

SRCS2	= main.cpp pub.cpp sub.cpp \
	  verify.cpp ldbio.cpp params.cpp \
	  lib/leveldb.cpp lib/xsz.cpp util.cpp

OBJS2	= $(SRCS2:.cpp=.o)

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


all: lib tx tx2

lib: $(ALIBS)

$(ALIBS): 
	cd $(CSDKDIR); make; cd ..
#	ln -f -s $(CSDKDIR)/lib/*.a .

tx: $(OBJS)
	$(CPP) $(CPPFLAGS) -o $@ $(OBJS) $(ALIBS) $(SDKLIBS) $(LIBS)

tx2: $(OBJS2)
	$(CPP) $(CPPFLAGS) -o $@ $(OBJS2) $(ALIBS) $(SDKLIBS) $(LIBS) 

clean:
	rm -f tx tx2 $(OBJS) *.o *.out *.ver *.a *.log
	rm -rf testdb xtestdb
	cd test; make clean; cd ..

cleanall:
	cd $(CSDKDIR); make clean; cd ..
	make clean
