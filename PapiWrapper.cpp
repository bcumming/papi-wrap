#include <string>

#include <PapiCollectors.h>

int  pw_new_collector(char *name){
    return PapiCollectors::instance->addCollector(name);

}

void pw_start_collector(int handle){
    PapiCollectors::instance->start(handle);
}

void pw_stop_collector(int handle){
    PapiCollectors::instance->stop(handle);
}

void pw_print(){
    PapiCollectors::instance->print();
}

