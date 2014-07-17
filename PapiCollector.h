#pragma once

#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cmath>

#include <vector>
#include <string>

#ifndef NOPAPI
#include <papi.h>
#endif
#include <omp.h>

class PapiCollector {
public:
    PapiCollector();

    void start();
    void stop();

    std::string eventName(int eventIdx);
    int eventNumber(int eventIdx);

    int numEvents();
    int numThreads();

    double time(int tid);
    double time();

    long long count(int tid, int eventIdx);
    long long count(int eventIdx);

    void print();

private:
    std::vector<int> numbers_;
    std::vector<double> times_;
    std::vector<std::vector<long long> > counts_;
#ifdef NOPAPI
    std::vector<double> timeVec_;
#endif
};

