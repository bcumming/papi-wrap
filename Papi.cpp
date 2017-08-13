#include <iostream>
#include <algorithm>

#include <cstdlib>
#include <cstring>

#include <papi.h>

#include "Papi.h"
#include "util.h"

#ifdef PW_MPI
#include <mpi.h>
#endif

#include "OutStreams.h"

bool isNotPESsuccess(PapiEventSetReturn val) {
    return (val!=PESsuccess);
}

// initialise Papi instance
Papi *Papi::instance_ = 0;

Papi *Papi::instance() {
    //return instance_ ? instance_ : (instance_ = new Papi);
    if( instance_==0 ){
        instance_ = new Papi;
        instance_->init();
    }
    return instance_;
}

void Papi::init() {
    // only initialise if not already initialised
    if(setup_)
        return;

    int papi_error;

    // set debugging if requested by environment variable
    char *debugStr = getenv("CSCSPERF_DEBUG");
    debug_ = (debugStr != NULL);
    if(debug_)
        outstreams().fid() << "PAPI:: debug mode on" << std::endl;

    // initialize the papi library */
    papi_error = PAPI_library_init(PAPI_VER_CURRENT);
    if (papi_error != PAPI_VER_CURRENT) {
        outstreams().fid() << "PAPI library init error!" << std::endl;
        exit(1);
    }

    // allow multiplexing
    if( PAPI_multiplex_init() != PAPI_OK ){
        outstreams().fid() << "Papi:: Could not initialize the multiplexing in Papi." << std::endl;
    }

    // assume fixed thread affinity, otherwise this approach fails
    papi_error = PAPI_thread_init((long unsigned int (*)()) get_thread_num);
    if ( papi_error != PAPI_OK ){
        outstreams().fid() << "Could not initialize the library with with threading."
             << std::endl;
        exit(1);
    }
    numThreads_ = get_max_threads();

    timeVec_.resize(numThreads_);

    // determine the number of hardware counters
    int num_hwcntrs;
    papi_error = num_hwcntrs = PAPI_num_counters();
    if (papi_error <= PAPI_OK){
        std::cerr << "PAPI:: unable to determine number of hardware counters" << std::endl;
        papi_print_error(papi_error);
    }
    //if( debug_ )
        //std::cerr << "PAPI:: there are " << num_hwcntrs
                  //<< " hardware counters available" << std::endl;

    // get user-defined list of hardware counters from environment variable CSCSPERF_EVENTS
    char *counter_str = getenv("CSCSPERF_EVENTS");
    // if the environment variable is not set, use an empty string
    std::string papi_counters = (counter_str==0) ? std::string() : std::string(counter_str);

    std::vector<int> events;
    parseTokenString(papi_counters, events);

    if (debug_){
        outstreams().fid() << "PAPI:: CSCSPERF_EVENTS = " <<  papi_counters << std::endl;
        outstreams().fid() << "PAPI:: requested " << events.size() << " events" << std::endl;
    }

    // add events to event set
    eventSet_.initialize();
    for(int i=0; i<events.size(); i++){
        if( eventSet_.addEvent(events[i])==PESsuccess )
            events_.push_back(events[i]);
    }

    if (debug_){
        outstreams().fid() << "PAPI:: " <<  events_.size() << " events being monitored" << std::endl;
    }

    if(numEvents()==0){
        setup_ = true;
        return;
    }

    // allocate space for counters
    counters_.resize(numThreads_);
    for(int i=0; i<numThreads_; i++)
        counters_[i].resize(numEvents());

    setup_ = true;
}

void Papi::papi_print_error(int ierr) {
    char errstring[PAPI_MAX_STR_LEN];
    #if PAPI_VERSION_MAJOR(PAPI_VERSION)>4
    PAPI_perror(errstring);
    #else
    PAPI_perror(ierr, errstring, PAPI_MAX_STR_LEN );
    #endif

    #ifdef PW_MPI
    outstreams().fid() << "PAPI error " << errstring << std::endl;
    #else
    std::cerr << "PAPI error " << errstring << std::endl;
    #endif
}

void Papi::startCounters() {
    if(!setup_)
        init();
    if(counting()){
        std::cerr << "PAPI counters warning : cannot start papi counters when they are already running"
                  << std::endl;
        return;
    }

    //if(debug_)
        //std::cerr << "PAPI:: starting counters, " << events_.size() << " events being counted" << std::endl;

    #pragma omp parallel
    {
        int tid = get_thread_num();
        if(numEvents()){
            int papi_error = PAPI_start(eventSet_.getEventSet(tid));
            if (papi_error != PAPI_OK){
                std::cerr << "PAPI error : unable to start counters" << std::endl;
                papi_print_error(papi_error);
                exit(-1);
            }
        }

        double ttmp = getTime();
        timeVec_[tid] = -ttmp;
    }
    counting_ = true;
}


void Papi::stopCounters() {
    if(!counting()){
        std::cerr << "PAPI counters warning : cannot stop papi counters when they are have not been started"
                  << std::endl;
        return;
    }
    #pragma omp parallel
    {
        int tid = get_thread_num();
        if(numEvents()){
            int papi_error = PAPI_stop(eventSet_.getEventSet(tid), &counters_[tid][0]);
            if (papi_error != PAPI_OK){
                std::cerr << "PAPI error : unable to stop counters" << std::endl;
                papi_print_error(papi_error);
                exit(-1);
            }
        }
        timeVec_[tid] += getTime();
    }
    counting_ = false;

    //if(debug_)
        //std::cerr << "PAPI:: counters stopped" << std::endl;

}

