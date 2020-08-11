#pragma once

/******************************************************************************************
    NOTE: this code is not thread safe if, for some reason, the number of events in event
    sets on different threads are different. This would occurr if on thread successfully adds
    an event that another fails to add, which is not likely.
 ******************************************************************************************/

#include <papi.h>
#include <assert.h>

#include "definitions.h"
#include "util.h"

class PapiEventSet{
    public:
        PapiEventSet();
        ~PapiEventSet();
        bool initialize();
        bool isInitialized();

        // add an event by passing event code
        PapiEventSetReturn addEvent(int eid);

        // add all events in a list to the event set
        // returns a vector of corresponding PapiEventSetReturn values for each event
        std::vector<PapiEventSetReturn> addEvent(std::vector<int> const &evec);

        // add an event by passing event string
        PapiEventSetReturn addEvent(std::string estr);

        // return number of events in set
        int events() {return events_.size();};

        // return events in an event set
        int getEvent( int i );

        // return the handle for the event set
        int getEventSet(int tid);

        // return the index of an event (event is a PAPI event code)
        // returns -1 if event is not being counted
        int contains(int event);
    private:
        // dissable copying, because an event set is a handle to internal work
        // space maintained by papi
        PapiEventSet(const PapiEventSet&);
        PapiEventSet& operator=(const PapiEventSet&);

        std::vector<int> events_;
        std::vector<int> eventSet_;

        int numThreads_;
        bool initialized_;
};

