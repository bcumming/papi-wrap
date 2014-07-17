#pragma once
#include <fstream>
#include <iostream>

class outstreams {
public:
    outstreams();

#ifdef PW_MPI
    std::ofstream& fid() const {return fid_;}
#else
    std::ostream& fid() const {return std::cerr;}
#endif

private:
#ifdef PW_MPI
    std::ofstream fid_;
#endif

};