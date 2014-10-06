#include "PapiCollectors.h"

#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib>

#ifndef NOPAPI
#include "Papi.h"
#endif
#include "PapiCollectors.h"
#include "util.h"
#include "OutStreams.h"

PapiCollectors *PapiCollectors::instance_=0;
PapiCollectors *PapiCollectors::instance()
{
    if(!instance_)
        instance_ = new PapiCollectors;
    return instance_;
}

int PapiCollectors::addCollector(const char *name=0)
{
    std::stringstream ss;
    // use a default name for the collector if none is provided
    if (name == 0)
        ss << "collector_" << collectors_.size();
    else
        ss << name;

    // create a new collector, and generate new handle
    collectors_.push_back(PapiCollector());

    // store name of collector
    collector_names_.push_back(ss.str());

    // return handle
    return collectors_.size() - 1;
}

void PapiCollectors::start(int handle)
{
    assert(handle<collectors_.size());
    collectors_[handle].start();
}

void PapiCollectors::stop(int handle)
{
    assert(handle<collectors_.size());
    collectors_[handle].stop();
}

PapiCollector& PapiCollectors::collector(int handle)
{
    assert(handle<collectors_.size());
    return collectors_[handle];
}

void PapiCollectors::print()
{
    char *filename_cstr = getenv("CSCSPERF_FILENAME");
    char *printopts_cstr = getenv("CSCSPERF_PRINTOPTS");

    outstreams os;
    os.fid() << "--------------------------------------------------------------------------------" << std::endl;
    for(int i=0; i<collectors_.size(); i++){
        os.fid() << "   Collector " << collector_names_[i] << " :: ";
        collectors_[i].print();
        os.fid() << "--------------------------------------------------------------------------------" << std::endl;
    }
}

namespace util {
    template <typename T>
    struct perm_compare
    {
        perm_compare(std::vector<T> &v) : v_(v) {};
        bool operator()(size_t left, size_t right)
        {
            return v_[left] > v_[right];
        }
        private:
        std::vector<T>& v_;
    };
}

void PapiCollectors::print_table()
{
    char *filename_cstr = getenv("CSCSPERF_FILENAME");
    char *printopts_cstr = getenv("CSCSPERF_PRINTOPTS");

    outstreams os;

    // find
    double total_time = 0.;
    std::vector<double> times(collectors_.size());
    std::vector<size_t> indexes(collectors_.size());
    for(int i=0; i<collectors_.size(); ++i)
        indexes[i] = i;
    for(int i=0; i<collectors_.size(); ++i) {
        times[i] = collectors_[i].time();
        total_time += times[i];
    }
    // sort the times in descending order, keeping indexes in indexes
    util::perm_compare<double> f(times);
    std::sort(indexes.begin(), indexes.end(), f);

    const int maxline = 512;
    char line[maxline];
    snprintf(line, maxline,
             "%-25s%13s%9s",
             "collector",
             "time",
             "time (\%)");
    char valstr[32];
    int num_counters = collectors_.size() ? collectors_[0].numEvents() : 0;
    for(int k=0; k<num_counters; ++k) {
        sprintf(valstr, "%16s", collectors_[0].eventName(k).c_str());
        strcat(line, valstr);
    }
    os.fid() << "--------------------------------------------------------------------------------" << std::endl;
    os.fid() << line << std::endl;
    os.fid() << "--------------------------------------------------------------------------------" << std::endl;
    for(int j=0; j<collectors_.size(); ++j) {
        int i = indexes[j];
        double t = collectors_[i].time();
        snprintf(line, maxline,
                 "%-25s%13.4f%9.2f",
                 collector_names_[i].c_str(),
                 float(t),
                 float(t/total_time*100.));
        for(int k=0; k<num_counters; ++k) {
            sprintf(valstr, "%16lld", collectors_[i].count(k));
            strcat(line, valstr);
        }
        os.fid() << line << std::endl;
    }
    snprintf(line, maxline, "%-25s%13.4f", "TOTAL ", total_time);
    os.fid() << line << std::endl;

    os.fid() << "--------------------------------------------------------------------------------" << std::endl;
}
