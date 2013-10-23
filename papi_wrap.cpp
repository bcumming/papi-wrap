#include <string>
#include <PapiCollectors.h>

extern "C"
int pw_new_collector(char *name){
    return PapiCollectors::instance()->addCollector(name);
}

extern "C"
void pw_start_collector(int handle){
    PapiCollectors::instance()->start(handle);
}

extern "C"
void pw_stop_collector(int handle){
    PapiCollectors::instance()->stop(handle);
}

extern "C"
void pw_print(){
    PapiCollectors::instance()->print();
}

int pw_new_collector(char *name);
void pw_start_collector(int handle);
void pw_stop_collector(int handle);
void pw_print();

