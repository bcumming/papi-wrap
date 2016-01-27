CXX=CC
F90=ftn

PAPI_PATH=/opt/cray/papi/default
PAPI_LIB_PATH=$(PAPI_PATH)/lib64
LDFLAGS=$(PAPI_LIB_PATH)/libpapi.a $(PAPI_LIB_PATH)/libpfm.a
IFLAGS=-I$(PAPI_PATH)/include -I.

CXXFLAGS=$(IFLAGS) -fopenmp

HEADERS=Papi.h util.h definitions.h PapiEventSet.h PapiCollector.h PapiCollectors.h OutStreams.h


all: lib

Papi.o: Papi.cpp $(HEADERS)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

OutStreams.o: OutStreams.cpp $(HEADERS)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

PapiEventSet.o: PapiEventSet.cpp $(HEADERS)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

PapiCollector.o: PapiCollector.cpp $(HEADERS)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

PapiCollectors.o: PapiCollectors.cpp $(HEADERS)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

papi_wrap.o: papi_wrap.cpp papi_wrap.h $(HEADERS)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

m_papi_wrap.o: m_papi_wrap.f90
	$(F90) -c m_papi_wrap.f90 -o m_papi_wrap.o

lib: Papi.o PapiEventSet.o PapiCollector.o PapiCollectors.o papi_wrap.o m_papi_wrap.o OutStreams.o
	ar rcs libpapi_wrap.a  $(LDFLAGS) $^

clean:
	rm -f *.o *.a *.mod

