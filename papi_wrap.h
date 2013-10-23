#pragma once

#ifdef __cplusplus
extern "C" {
#endif
int  pw_new_collector(char *name);
void pw_start_collector(int handle);
void pw_stop_collector(int handle);
void pw_print();
#ifdef __cplusplus
}
#endif
