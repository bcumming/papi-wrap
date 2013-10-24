#pragma once

#ifdef __cplusplus
extern "C" {
#endif
int  pw_new_collector(const char *name);
void pw_new_collector_fortran(const char *name, int name_length, int *handle);
void pw_start_collector(int handle);
void pw_stop_collector(int handle);
void pw_print();
#ifdef __cplusplus
}
#endif

