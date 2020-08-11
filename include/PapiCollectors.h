#pragma once

#include <vector>
#include <string>
#include <ostream>

#include "PapiCollector.h"

// singleton class for accessing a list of Papi collectors
class PapiCollectors {
public:
    int addCollector(const char *name);
    void start(int collector_h);
    void stop(int collector_h);
    PapiCollector& collector(int handle);

    static PapiCollectors* instance();

    void print();
    void print_table();
private:
    // disbale default and copy constructors for singleton
    PapiCollectors() {};
    PapiCollectors(const PapiCollectors&);

    std::vector<PapiCollector> collectors_;
    std::vector<std::string> collector_names_;

    static PapiCollectors* instance_;
};

