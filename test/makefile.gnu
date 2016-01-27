# intel toolchain
CXX=CC
CC=cc
F90=ftn

PAPI_PATH=/opt/cray/papi/default
PAPIWRAP_PATH=../
PAPI_LIB_PATH=$(PAPI_PATH)/lib
LDFLAGS=$(PAPIWRAP_PATH)/libpapi_wrap.a $(PAPI_LIB_PATH)/libpapi.a $(PAPI_LIB_PATH)/libpfm.a -lstdc++

LDFLAGS_FORTRAN=-L$(PAPI_LIB_PATH) -lpapi -lpfm
LDFLAGS_FORTRAN+=-L$(PAPIWRAP_PATH) -lpapi_wrap

IFLAGS=-I$(PAPI_PATH)/include -I..

CXXFLAGS=$(IFLAGS) -fopenmp -O2
CCFLAGS=$(IFLAGS) -fopenmp -O2

all: matmul dot saxpy

matmul: matmul.cpp
	$(CXX) $(CXXFLAGS) matmul.cpp $(LDFLAGS) -o matmul

dot: dot.c
	$(CC) $(CCFLAGS) dot.c $(LDFLAGS) -o dot

saxpy: saxpy.f90
	ftn -O2 -fopenmp saxpy.f90 -o saxpy -I.. -I/opt/cray/papi/default/include -L.. -lpapi_wrap -lstdc++

static: static.c
	$(CC) $(CCFLAGS) static.c $(LDFLAGS) -o static

#stream: stream.cu
#	nvcc -arch=sm_35 $(CCFLAGS) stream.cu $(LDFLAGS) -o stream

clean:
	rm -f dot matmul saxpy static

