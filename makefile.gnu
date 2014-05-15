CXX=g++
F90=ftn

#PAPI_PATH=/project/csstaff/bcumming/repos/papi/install
PAPI_PATH=/scratch/santis/bcumming/papi/
PAPI_LIB_PATH=/project/csstaff/bcumming/repos/papi/install/lib
LDFLAGS=$(PAPI_LIB_PATH)/libpapi.a $(PAPI_LIB_PATH)/libpfm.a
IFLAGS=-I$(PAPI_PATH)/include -I.

CXXFLAGS=$(IFLAGS)

HEADERS=Papi.h util.h definitions.h PapiEventSet.h PapiCollector.h PapiCollectors.h


all: lib

Papi.o: Papi.cpp $(HEADERS)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

PapiEventSet.o: PapiEventSet.cpp $(HEADERS)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

PapiCollector.o: PapiCollector.cpp $(HEADERS)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

PapiCollectors.o: PapiCollectors.cpp $(HEADERS)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

papi_wrap.o: papi_wrap.cpp papi_wrap.h $(HEADERS)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

lib: Papi.o PapiEventSet.o PapiCollector.o PapiCollectors.o papi_wrap.o
	ar rcs libpapi_wrap.a  $(LDFLAGS) $^

clean:
	rm -f *.o *.a *.mod
