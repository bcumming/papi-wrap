#CXX=g++ -fopenmp
CXX=CC

PAPI_PATH=/project/csstaff/bcumming/repos/papi/install
PAPI_LIB_PATH=/project/csstaff/bcumming/repos/papi/install/lib
LDFLAGS=$(PAPI_LIB_PATH)/libpapi.a $(PAPI_LIB_PATH)/libpfm.a
IFLAGS=-I$(PAPI_PATH)/include

CXXFLAGS=$(IFLAGS)

HEADERS=Papi.h util.h definitions.h PapiEventSet.h


all: libcscsperf

Papi.o: Papi.cpp $(HEADERS)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

PapiEventSet.o: PapiEventSet.cpp $(HEADERS)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

PapiCollector.o: PapiCollector.cpp $(HEADERS)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

PapiCollectors.o: PapiCollectors.cpp $(HEADERS)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

#CSCSPerfWrapper.o: CSCSPerfWrapper.cpp $(HEADERS)
#	$(CXX) -c $(CXXFLAGS) -o $@ $<

libcscsperf: Papi.o PapiEventSet.o PapiCollector.o PapiCollectors.o
	ar rcs libcscsperf.a  $(LDFLAGS) $^

clean:
	rm -f *.o *.a

