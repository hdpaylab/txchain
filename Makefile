CPP	= g++

CSDKDIR	= HdacCSDK2

SDKLIBS	= -lsecp256k1 -lssl -lcrypto -ldl -lboost_system -lboost_thread -lpthread
LIBS	= -lzmq -lpq -lleveldb

INCLUDE	= -I/usr/include/postgresql -IHdacCSDK2/include -Ilib

CPPFLAGS = -O2 $(INCLUDE) -Wall -W -std=c++11 -Wwrite-strings -Wno-unused-parameter

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


all: lib lib2 test

lib: $(ALIBS)

lib2: 
	cd lib2; make; cd ..

$(ALIBS): 
	cd $(CSDKDIR); make; cd ..

test:
	cd test; make; cd ..

bak:
	tar cf ~/BAK/txchain-`date +"%y%m%d"`.tar M* README* HISTORY */*.[hc]* */Make* */README* */*/*.[hc]* */*/Make* */*/*/*.[hc]* 
	gzip -f ~/BAK/txchain-`date +"%y%m%d"`.tar

cleanall:
	cd $(CSDKDIR); make clean; cd ..
	cd lib2; make clean; cd ..
	make clean

clean:
	cd v0.1; make clean; cd ..
	cd v0.2; make clean; cd ..
	cd v0.3; make clean; cd ..
	cd test; make clean; cd ..
