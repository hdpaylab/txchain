CPP	= g++

CSDKDIR	= HdacCSDK2

SDKLIBS	= -lsecp256k1 -lssl -lcrypto -ldl -lboost_system -lboost_thread -lpthread
LIBS	= -lzmq -lpq -lleveldb

INCLUDE	= -I/usr/include/postgresql -IHdacCSDK2/include

CPPFLAGS = -O2 $(INCLUDE) -Wall -W -std=c++11 -Wwrite-strings 

SRCS	= xmain.cpp xpub.cpp xsub.cpp \
	  xverify.cpp xleveldb.cpp params.cpp util.cpp

OBJS	= $(SRCS:.cpp=.o)

SRCS2	= main.cpp pub.cpp sub.cpp \
	  verify.cpp leveldb.cpp params.cpp util.cpp

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
	rm -rf testdb
	cd test; make clean; cd ..

cleanall:
	cd $(CSDKDIR); make clean; cd ..
	make clean
