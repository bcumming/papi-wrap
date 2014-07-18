cmake_minimum_required( VERSION 2.8 )

project( PAPIWRAP )
enable_language( CXX )


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

include(FindOpenMP)
if(OPENMP_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif(OPENMP_FOUND)

if (${CMAKE_CXX_COMPILER_ID} MATCHES "XL")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -qsuppress=1540-1088:1540-0700:1540-1090:1540-1101 -qnoeh")
endif()

set( PAPI_PATH "$ENV{PAPI_ROOT}" CACHE STRING "Path to papi library" )
set( PAPI_LIB_PATH "${PAPI_PATH}" )

include_directories( ${PAPI_PATH}/include . )
link_directories( ${PAPI_PATH}/lib )

set( LIBRARY_OUTPUT_PATH "lib")
set( CMAKE_CXX_ARCHIVE_APPEND "${PAPI_LIB_PATH}/libpapi.a" )

set( PAPI_WRAP_DO_NOT_USE_PAPI "ON" CACHE BOOL "Set to on to not use PAPI and just measure times")

if (${PAPI_WRAP_DO_NOT_USE_PAPI})
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOPAPI" )
  add_library( papi_wrap PapiCollectors.cpp PapiCollector_nopapi.cpp papi_wrap.cpp OutStreams.cpp)
else()
  add_library( papi_wrap Papi.cpp PapiEventSet.cpp PapiCollectors.cpp PapiCollector.cpp papi_wrap.cpp OutStreams.cpp)
endif()

set ( PAPI_WRAP_COMPILE_TESTS "OFF" CACHE BOOL "Set to on if you want to compile the tests" )

if ( ${PAPI_WRAP_COMPILE_TESTS} )
    add_executable( matmul test/matmul.cpp )

    if (${PAPI_WRAP_DO_NOT_USE_PAPI})
        target_link_libraries( matmul papi_wrap ${CUDA_LIBRARY_NAMES} )
    else()
        target_link_libraries( matmul papi_wrap ${CUDA_LIBRARY_NAMES} papi )
    endif()
endif()
