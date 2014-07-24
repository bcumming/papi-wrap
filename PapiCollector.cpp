#include "PapiCollector.h"

#include "util.h"
#include "definitions.h"
#include "Papi.h"
#include "OutStreams.h"

// constructor
PapiCollector::PapiCollector() {
    int numEvents = Papi::instance()->numEvents();
    int numThreads = Papi::instance()->numThreads();

    // store a list of the papi event ids being recorded
    for(int i=0; i<numEvents; i++)
        numbers_.push_back(Papi::instance()->eventNumber(i));

    // allocate space for holding cumulative counter values
    // one set of counters is recorded for each thread
    counts_.resize(numThreads);
    for(int tid=0; tid<numThreads; tid++)
        counts_[tid].resize(numEvents,0LL);

    times_.resize(numThreads);
}

void PapiCollector::start() {
    // stop the counters
    Papi::instance()->startCounters();
}

void PapiCollector::stop() {
    // stop the counters
    Papi::instance()->stopCounters();

    // accumulate the counters for each thread into counts_
    int numEvents = Papi::instance()->numEvents();
    int numThreads = Papi::instance()->numThreads();
    for(int tid=0; tid<numThreads; tid++){
        for(int i=0; i<numEvents; i++){
            counts_[tid][i] += Papi::instance()->counter(tid,i);
        }
        times_[tid] += Papi::instance()->time(tid);
    }
}

// return the name of event
std::string PapiCollector::eventName(int eventIdx) {
    return Papi::instance()->eventName(eventIdx);
}

// return the Papi id of event
int PapiCollector::eventNumber(int eventIdx) {
    return Papi::instance()->eventNumber(eventIdx);
}

int PapiCollector::numEvents() {
    return numbers_.size();
}

double PapiCollector::time(int tid) {
    return times_[tid];
}

double PapiCollector::time() {
    return vecMean(times_);
}

long long PapiCollector::count(int tid, int eventIdx) {
    return counts_[tid][eventIdx];
}

long long PapiCollector::count(int eventIdx){
    long long sum(0);

    for(int tid=0; tid<Papi::instance()->numThreads(); tid++)
        sum += count(tid, eventIdx);

    return sum;
}

void PapiCollector::print(){
    int numThreads=Papi::instance()->numThreads();
    int numEvents = Papi::instance()->numEvents();

    outstreams().fid() << "wall time " << time() << " seconds" << std::endl;
    if(numEvents>0){
        // print table headers
        if(numThreads>1){
            // only print per-thread values if there is more than one thread
            for(int tid=0; tid<numThreads; tid++){
                outstreams().fid() << "   THREAD" << std::setw(2) << tid;
            }
        }
        outstreams().fid() << " [      TOTAL ]" << std::endl;

        for(int i=0; i<numEvents; i++){
            // if we have more than one hardare thread first print individual
            // totals for each thread
            if(numThreads>1){
                for(int tid=0; tid<numThreads; tid++){
                    outstreams().fid() << " " << std::setw(10) << count(tid, i);
                }
            }
            // print the cumulative total over all threads
            outstreams().fid() << " [ " << std::setw(10) << count(i) << " ]"
                                    << "\t" << eventName(i)  << std::endl;
        }
    }else{
        outstreams().fid()  << "PAPI-WRAP :: no hardware events to print" << std::endl;
    }
}
