#include <algorithm>

#include "PapiEventSet.h"
#include "Papi.h"
#include "util.h"

PapiEventSet::PapiEventSet()
    : initialized_(false)
{
    numThreads_ = get_max_threads();
    // create NULL event set for each thread
    eventSet_.resize(numThreads_);
    for(int i=0; i<numThreads_; i++)
        eventSet_[i] = PAPI_NULL;
};

// destructor:
PapiEventSet::~PapiEventSet(){
    #pragma omp parallel
    {
        int tid = get_thread_num();
        if( eventSet_[tid]!=PAPI_NULL ){
            PAPI_cleanup_eventset(eventSet_[tid]);
            PAPI_destroy_eventset(&eventSet_[tid]);
        }
    }
}

bool PapiEventSet::initialize(){
    if( !initialized_ ){
        int success(0);

        // we have to use an addition reduction on the openmp clause below because
        // the Cray C++ compiler has a bug when using & or && for reductions
        #pragma omp parallel reduction( + : success )
        {
            int tid = get_thread_num();
            int status =  PAPI_create_eventset(&eventSet_[tid]);
            if( status!=PAPI_OK ){
                if(Papi::instance()->debug())
                    Papi::instance()->fid() << "PAPI:: unable to initialize event set :: "
                                            << PAPI_strerror(status) << std::endl;
                eventSet_[tid]=PAPI_NULL;
                success=0;
            }else{
                success=1;
            }
        }
        initialized_ = (success==get_max_threads()) ? true : false;
    }
    return initialized_;
}

bool PapiEventSet::isInitialized(){
    return initialized_;
};

// add an event by passing event code
PapiEventSetReturn PapiEventSet::addEvent(int eid){
    if( !isInitialized() ) {
        if( initialize()==false ) {
            Papi::instance()->fid() << "PAPI:: unable to initialize eventset "
                                    <<  std::endl;
            return PESuninitialized;
        }
    }

    int success(0);
    // we have to use an addition reduction on the openmp clause below because
    // the Cray C++ compiler has a bug when using & or && for reductions
    #pragma omp parallel reduction( + : success )
    {
        int tid = get_thread_num();

        int status = PAPI_add_event(eventSet_[tid], eid);
        success = (status==PAPI_OK) ? 1 : 0;

        if(!success &&  Papi::instance()->debug())
            Papi::instance()->fid() << "PAPI:: unable to add event to event set :: "
                                    << PAPI_strerror(status) << std::endl;
    }
    if(success == get_max_threads()){
        events_.push_back(eid);
        return PESsuccess;
    }
    return PESerror;
};

// add all events in a list to the event set
// returns a vector of corresponding PapiEventSetReturn values for each event
std::vector<PapiEventSetReturn> PapiEventSet::addEvent(std::vector<int> const &evec){
    std::vector<PapiEventSetReturn> rvec;
    for(int i=0; i<evec.size(); i++){
        rvec.push_back( addEvent(evec[i]) );
    }
    return rvec;
};

// add an event by passing event string
PapiEventSetReturn PapiEventSet::addEvent(std::string estr){
    int eid;
    int status = PAPI_event_name_to_code(const_cast<char *>(estr.c_str()), &eid);
    if(status!=PAPI_OK){
        if(Papi::instance()->debug())
            Papi::instance()->fid() << "PAPI:: unable to add event to event set :: "
                      << PAPI_strerror(status) << std::endl;
        return PESerror;
    }
    return addEvent(eid);
};

// return events in an event set
int PapiEventSet::getEvent( int i ){
    assert(i>=0 && i<events_.size() );
    return events_[i];
}

// return the handle for the event set
int PapiEventSet::getEventSet(int tid=0){
    assert(tid<get_max_threads() );
    return eventSet_[tid];
}

// returns the index of and event with id event
// returns -1 if the eventSet does not contain evetn
int PapiEventSet::contains(int event){
    std::vector<int>::iterator it = std::find(events_.begin(), events_.end(), event);
    return it==events_.end() ? -1 : it-events_.begin();
}

