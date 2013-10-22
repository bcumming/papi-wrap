CXX=CC -g -O3 -mavx -fopenmp 
CXX=CC

PAPI_PATH=/project/csstaff/bcumming/repos/papi/install
PAPI_LIB_PATH=/project/csstaff/bcumming/repos/papi/install/lib
LDFLAGS=../libcscsperf.a $(PAPI_LIB_PATH)/libpapi.a $(PAPI_LIB_PATH)/libpfm.a
IFLAGS=-I$(PAPI_PATH)/include -I../

#CXXFLAGS=-fopenmp $(IFLAGS) -D_USE_MPI
CXXFLAGS=$(IFLAGS)

all: test

test: test.cpp
	$(CXX) $(CXXFLAGS) test.cpp $(LDFLAGS) -o test

clean:
	rm -f test

