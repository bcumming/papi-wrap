cmake_minimum_required( VERSION 3.12)

project( PAPIWRAP )
enable_language( CXX )

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
find_package(PAPI)

# Optionally include CUDA
set( PAPI_WRAP_USE_GPU "OFF" CACHE BOOL "Compile with GPU support (CUDA)" )
if( "${PAPI_WRAP_USE_GPU}" STREQUAL "ON" )
  find_package(CUDA REQUIRED)
  include_directories(SYSTEM ${CUDA_INCLUDE_DIRS})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_USE_GPU_")
  set(CUDA_LIBRARY_NAMES cuda cupti cudart)
  message("${CUDA_LIBRARY_NAMES}")
  set(CUDA_CUDART_DIR "" CACHE STRING "Path to guess what")
  set(CUDA_CUPTI_DIR "" CACHE STRING "Path to guess what")
  link_directories(${CUDA_SDK_ROOT_DIR}/lib64 ${CUDA_CUDART_DIR} ${CUDA_CUPTI_LIBRARY})
else()
  set (CUDA_LIBRARIES "")
  set (CUDA_LIBRARY_NAMES "")
endif()

set( PAPI_WRAP_USE_MPI "OFF" CACHE BOOL "Compile with MPI support" )
if( "${PAPI_WRAP_USE_MPI}" STREQUAL "ON" )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPW_MPI")
endif()

# pass PAPI_WRAP_USE_OPENMP = 0 to disable it
if (NOT PAPI_WRAP_DO_NOT_USE_OPENMP)
  include(FindOpenMP)
endif()

if (${CMAKE_CXX_COMPILER_ID} MATCHES "XL")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -qsuppress=1540-1088:1540-0700:1540-1090:1540-1101 -qnoeh")
endif()

include_directories( ${PAPI_INCLUDE_DIRS} . )

set( LIBRARY_OUTPUT_PATH "lib")
set( CMAKE_CXX_ARCHIVE_APPEND "${PAPI_LIB_PATH}/libpapi.a" )

set( PAPI_WRAP_DO_NOT_USE_PAPI "ON" CACHE BOOL "Set to on to not use PAPI and just measure times")

if (${PAPI_WRAP_DO_NOT_USE_PAPI})
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOPAPI" )
  add_library( papi_wrap PapiCollectors.cpp PapiCollector_nopapi.cpp papi_wrap.cpp OutStreams.cpp)
else()
  add_library( papi_wrap Papi.cpp PapiEventSet.cpp PapiCollectors.cpp PapiCollector.cpp papi_wrap.cpp OutStreams.cpp)
endif()
target_link_libraries(papi_wrap PRIVATE ${PAPI_LIBRARIES})
if(OPENMP_FOUND)
  message("Linking openmp")
  target_link_libraries(papi_wrap PRIVATE OpenMP::OpenMP_CXX)
endif()

set ( PAPI_WRAP_COMPILE_TESTS "OFF" CACHE BOOL "Set to on if you want to compile the tests" )

if ( ${PAPI_WRAP_COMPILE_TESTS} )
    add_executable( matmul test/matmul.cpp )
    add_executable( static test/static.c )

    if (${PAPI_WRAP_DO_NOT_USE_PAPI})
        target_link_libraries( matmul papi_wrap ${CUDA_LIBRARY_NAMES} )
        target_link_libraries( static papi_wrap ${CUDA_LIBRARY_NAMES} )
    else()
      target_link_libraries( matmul papi_wrap ${CUDA_LIBRARY_NAMES} ${PAPI_LIBRARIES} )
      target_link_libraries( static papi_wrap ${CUDA_LIBRARY_NAMES} ${PAPI_LIBRARIES} )
    endif()
endif()

message("Papi-wrap usage : Remember to set (for example) \nexport CSCSPERF_EVENTS=\"PAPI_L1_DCM|PAPI_L2_DCM\"")
message("Check CSCSPERF_FILENAME, CSCSPERF_PRINTOPTS, CSCSPERF_DEBUG, CSCSPERF_EVENTS")
