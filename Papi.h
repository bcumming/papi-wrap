#pragma once

#include <vector>

#include "PapiEventSet.h"

// singleton that handles papi intialisation and access to papi calls
class Papi{
public:
    static Papi* instance();

    void init();

    int numEvents(){
        return events_.size();
    }

    std::string eventName(int i){
        assert(i<numEvents());
        char tmp[256];
        PAPI_event_code_to_name(events_[i], tmp);
        return std::string(tmp);
    }

    int eventNumber(int i){
        assert(i<numEvents());
        return events_[i];
    }

    long long counter(int tid, int i){
        assert(i<numEvents());
        assert(tid<numThreads());
        return counters_[tid][i];
    }

    double time(int tid){
        assert(tid<numThreads());
        return timeVec_[tid];
    }

    PapiEventSet& eventSet(){
        return eventSet_;
    }

    void startCounters();
    void stopCounters();
    int numThreads() {return numThreads_;}
    bool counting() { return counting_; }

private:
    Papi(Papi const &) {}
    Papi() {setup_=false; debug_=false; counting_=false;}

    void papi_print_error(int);

    bool setup_;
    bool debug_;
    bool counting_;
    PapiEventSet eventSet_;
    int numThreads_;
    std::vector<int> events_;
    std::vector<double> timeVec_;
    std::vector<std::vector<long long> > counters_;

    static Papi* instance_;
};

