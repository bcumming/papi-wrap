#pragma once

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

#ifdef _OPENMP
#include <omp.h>
static int get_num_threads() {
    return omp_get_num_threads();
}
static int get_max_threads() {
    return omp_get_max_threads();
}
static int get_thread_num() {
    return omp_get_thread_num();
}
#else
static int get_num_threads() {
    return 1;
}
static int get_max_threads() {
    return 1;
}
static int get_thread_num() {
    return 0;
}
#endif

#define FILEPOS "=====CSCS::BENCH(" << __FILE__ << ":" << __LINE__ << ") - "

// find the sum of a vector of type T
template <typename T>
T vecSum(std::vector<T> const &v){
    T sum = T();
    for(int i=0; i<v.size(); i++)
        sum += v[i];
    return sum;
}

// find the mean of a vector of type T
template <typename T>
T vecMean(std::vector<T> const &v)
{
    return vecSum(v)/(T)v.size();
}

// write a vector of type TVec to stream in a format
// that matlabl can read
template <typename TVec>
void writeVecMatlab(std::ofstream &fid, std::string name, TVec const &v)
{
    fid << name << " = [";
    for(int i=0; i<v.size(); i++)
        fid << v[i] << (i<(v.size()-1) ? " " : "];");
    fid << std::endl;
}

template <typename T, typename TStream>
void printLine(int idx, std::vector<std::vector<T> > const &val, std::string const &name, TStream fid)
{
    int nThreads=val.size();
    T sum = T(0);
    std::cerr << "STARTED OUTPUT " << nThreads << std::endl;
    if(nThreads>1){
        std::cerr << "\tMore than one thread..." << std::endl;
        for(int tid=0; tid<nThreads; tid++){
            std::cerr << "\tval " << tid << " :: " << val[tid][idx] << std::endl;
            fid << " " << std::setw(10) << val[tid][idx];
            sum += val[tid][idx];
        }
    }
    std::cerr << "val has dimensions :: " << val.size() << ":" << val[0].size() << std::endl;
    std::cerr << "[ " << std::setw(10) << val[0][idx] << " ] \t" << "name" << std::endl;
    std::cerr << "FINISHED" << std::endl;
}


template <typename T, typename TStream>
void printLine(int nThreads, T const &val, std::string const &name, TStream fid)
{
    if(nThreads>1)
        for(int i=0; i<nThreads; i++)
            fid << "           ";
    fid << "[ " << std::setw(10) << val << " ] \t" << name << std::endl;
}

template <typename T, typename TStream>
void printLine(int nThreads, std::vector<T> const &val, std::string const &name, TStream fid)
{
    if(nThreads>1)
        for(int i=0; i<nThreads; i++)
            fid << " " << std::setw(10) << val[i];
    fid << "[ " << std::setw(10) << vecSum(val) << " ] \t" << name << std::endl;
}

static std::vector<std::string> splitString(const std::string &s, char delim, std::vector<std::string> &elems)
{
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


static std::vector<std::string> splitString(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    return splitString(s, delim, elems);
}

/*
int findString(std::vector<std::string> const& strVec, std::string str){
    std::vector<std::string>::const_iterator it;
    it = std::find(strVec.begin(), strVec.end(), str);
    if(it==strVec.end())
        return -1;
    return it - strVec.begin();
}
*/

// return time
static double getTime()
{
    return double(PAPI_get_real_usec())*1.e-6;
}

static void parseTokenString(const std::string &str, std::vector<int> &events)
{
    std::vector<std::string> tokens(splitString(str, '|'));
    for(int i=0; i<tokens.size(); i++) {
        int eid;
        int status = PAPI_event_name_to_code(const_cast<char*>(tokens[i].c_str()), &eid);
        if(status == PAPI_OK)
            events.push_back(eid);
    }
}

/*
    pretty print an integer with "," used to denote multiples of 1000
    e.g.:
    1234     -> 1,234
    123456   -> 123,456
    12345678 -> 12,345,678
*/
static std::string num_to_human(long long n)
{
    std::stringstream out;
    out << n;

    std::string tmp(out.str());
    std::string ret;
    int len;

    while (tmp.size() > 3) {
        len = tmp.size();
        for (int i=0; i<3; i++) {
            ret = tmp[len-i-1] + ret;
        }
        ret = ',' + ret;
        tmp.replace(len-3, 3, "");
    }
    ret = tmp + ret;
    return ret;
}

