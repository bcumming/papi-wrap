#include "PapiCollectors.h"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib>

#include "Papi.h"
#include "PapiCollector.h"
#include "util.h"

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

    std::cout << "--------------------------------------------------------------------------------" << std::endl;
    for(int i=0; i<collectors_.size(); i++){
        std::cout << "   Collector " << collector_names_[i] << " :: ";
        collectors_[i].print();
        std::cout << "--------------------------------------------------------------------------------" << std::endl;
    }
}

