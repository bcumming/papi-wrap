#pragma once
#include <fstream>
#include <iostream>

class outstreams {
public:
    outstreams();

#ifdef PW_MPI
    std::ofstream& fid() {return fid_;}
#else
    std::ostream& fid() {return std::cerr;}
#endif

private:
#ifdef PW_MPI
    std::ofstream fid_;
#endif

};
