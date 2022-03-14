cmake_minimum_required(VERSION 3.13)
project(UnifiedFormat VERSION 0.1)

set(NSCLDAQ_ROOT /usr/opt/daq/current CACHE STRING "/usr/opt/daq/current")
set(NSCLDAQ_INC ${NSCLDAQ_ROOT}/include CACHE STRING "${NSCLDAQ_ROOT}/include")
set(NSCLDAQ_LIB ${NSCLDAQ_ROOT}/lib CACHE STRING "${NSCLDAQ_ROOT}/lib")


enable_testing()


# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#  The subdirectories that have libraries:

add_subdirectory(abstract)
add_subdirectory(v10)
add_subdirectory(v11)


