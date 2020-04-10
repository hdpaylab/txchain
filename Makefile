CPP = g++

CSDKDIR	= HdacCSDK2

SDKLIBS	= -lsecp256k1 -lssl -lcrypto -ldl -lboost_system -lboost_thread -lpthread
LIBS	= -lzmq -lpq -lleveldb

INCLUDE	= -I/usr/include/postgresql -IHdacCSDK2/include

CPPFLAGS = -O2 $(INCLUDE) -Wall -W -std=c++11 -Wwrite-strings 

SRCS	= main.cpp xpub.cpp xsub.cpp \
	  xparams.cpp xmsq.cpp xverify.cpp ex.cpp xleveldb.cpp util.cpp

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
	rm -f tx tx2 $(OBJS) *.o *.out *.ver *.a *.log
	rm -rf testdb
