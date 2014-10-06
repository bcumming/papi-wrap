#include <string>
#include <PapiCollectors.h>

extern "C"
int pw_new_collector(const char *name){
    return PapiCollectors::instance()->addCollector(name);
}

extern "C"
void pw_new_collector_fortran(const char *name, int name_length, int *handle){
    std::string namestr(name, name_length);
    *handle = pw_new_collector(namestr.c_str());
}

extern "C"
void pw_get_time_fortran(int handle, double *time){
    *time = PapiCollectors::instance()->collector(handle).time();
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

extern "C"
void pw_print_table(){
    PapiCollectors::instance()->print_table();
}

int pw_new_collector(char *name);
void pw_start_collector(int handle);
void pw_stop_collector(int handle);
void pw_print();

