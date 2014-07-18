#include "PapiCollector.h"

#include "util.h"
#include "definitions.h"
#include "OutStreams.h"

// constructor
PapiCollector::PapiCollector()
{
    times_.resize(get_max_threads());
    timeVec_.resize(get_max_threads());

//#define outsize(x) std::cout << #x << ".size(0) = " << x.size() << " -> " << get_num_threads() << std::endl;
//
//    outsize(numbers_);
//    outsize(times_);
//    outsize(counts_);
//    outsize(timeVec_);

}

void PapiCollector::start() {
    //if(debug_)
        //std::cerr << "PAPI:: starting counters, " << events_.size() << " events being counted" << std::endl;


#pragma omp parallel
    {
        int tid = get_thread_num();

        double ttmp = getTime();
        assert( tid < timeVec_.size() );
        timeVec_[tid] = -ttmp;
    }
    //counting_ = true;
}

void PapiCollector::stop() {

    #pragma omp parallel
    {
        int tid = get_thread_num();
        assert( tid < timeVec_.size() );
        timeVec_[tid] += getTime();
    }

    // accumulate the counters for each thread into counts_
    for(int tid=0; tid<get_num_threads(); tid++) {
        assert( tid < timeVec_.size() );
        times_[tid] += timeVec_[tid];
    }
}

// return the name of event
std::string PapiCollector::eventName(int eventIdx) {
    return "NOPAPI HAS BEEN SPECIFIED";
}

// return the Papi id of event
int PapiCollector::eventNumber(int eventIdx) {
    return -1;
}

int PapiCollector::numEvents() {
    return 0;
}

double PapiCollector::time(int tid) {
    assert( tid < times_.size() );
    return times_[tid];
}

double PapiCollector::time() {
    assert( get_max_threads() == times_.size() );
    return vecMean(times_);
}

long long PapiCollector::count(int, int) {
    assert(false);
    return -1;
}

long long PapiCollector::count(int){
    assert(false);
    return -1;
}

void PapiCollector::print(){
//    int numThreads=get_num_threads();

    outstreams os;
    os.fid() << "wall time " << time() << " seconds" << std::endl;
    os.fid()  << "PAPI-WRAP :: no hardware events to print" << std::endl;
}
