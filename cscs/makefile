F90				:= ftn
CXX				:= CC
CC      		:= cc
NVCC    		:= nvcc
PAPI_PATH		:= /opt/cray/pe/papi/default
PAPI_INC_PATH	:= $(PAPI_PATH)/include
PAPI_LIB_PATH	:= $(PAPI_PATH)/lib64
IFLAGS			:= -I$(PAPI_INC_PATH) -I./include
CFLAGS			:= $(IFLAGS) -g 
LDFLAGS			:= -L$(PAPI_LIB_PATH) -lpapi -lpfm 

ifeq ($(PE_ENV),GNU)
	CFLAGS += -fopenmp
endif

ifeq ($(PE_ENV),INTEL)
	CFLAGS += -qopenmp
endif

ifeq ($(PE_ENV),PGI)
	CFLAGS += -mp
    $(error papi.h: pgi will not compile because caddr_t was never approved by POSIX and is obsolete)
endif

ifeq ($(PE_ENV),CRAY)
	CFLAGS += -homp
endif

HEADERS=Papi.h util.h definitions.h PapiEventSet.h PapiCollector.h PapiCollectors.h OutStreams.h

%.o: %.cpp 
	$(CXX) $(CFLAGS) -c $< -o $@

%.o: %.f90
	$(F90) $(CFLAGS) -c $< -o $@

libpapi_wrap.a: Papi.o PapiEventSet.o PapiCollector.o PapiCollectors.o papi_wrap.o m_papi_wrap.o OutStreams.o
	ar rcs $@ $^
	ranlib $@

clean:
	rm -f *.o *.a *.mod

