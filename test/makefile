# gnu toolchain
CXX=CC g -O2 -mavx -fopenmp
cc=cc -O2 -mavx -fopenmp

# cray toolchain
CXX=CC
CC=cc -lcray-c++-rts

PAPI_PATH=/project/csstaff/bcumming/repos/papi/install
PAPI_LIB_PATH=/project/csstaff/bcumming/repos/papi/install/lib
LDFLAGS=../libcscsperf.a $(PAPI_LIB_PATH)/libpapi.a $(PAPI_LIB_PATH)/libpfm.a
IFLAGS=-I$(PAPI_PATH)/include -I..

CXXFLAGS=$(IFLAGS)
CCFLAGS=$(IFLAGS)

all: matmul dot

matmul: matmul.cpp
	$(CXX) $(CXXFLAGS) matmul.cpp $(LDFLAGS) -o matmul

dot: dot.c
	$(CC) $(CCFLAGS) dot.c $(LDFLAGS) -o dot

clean:
	rm -f dot matmul

