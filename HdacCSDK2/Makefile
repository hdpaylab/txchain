
HDACLIB	= hdaclib.a

all: $(HDACLIB)

$(HDACLIB): crypto
	cd crypto; make; cd ..
	cd entities; make; cd ..
	cd helpers; make; cd ..
	cd keys; make; cd ..
	cd primitives; make; cd ..
	cd rpc; make; cd ..
	cd script; make; cd ..
	cd structs; make; cd ..
	cd utils; make; cd ..
	cd test; make; cd ..
	ar cru $(HDACLIB) */*.o

clean:	
	cd crypto; make clean; cd ..
	cd entities; make clean; cd ..
	cd helpers; make clean; cd ..
	cd keys; make clean; cd ..
	cd primitives; make clean; cd ..
	cd rpc; make clean; cd ..
	cd script; make clean; cd ..
	cd structs; make clean; cd ..
	cd utils; make clean; cd ..
	cd test; make clean; cd ..
