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
add_subdirectory(v12)

#  Now the top level library target:

set (toplevel_sources
    NSCLDAQFormatFactorySelector.cpp
)

set (toplevel_headers
    NSCLDAQFormatFactorySelector.h
)

add_library(
    NSCLDAQFormat SHARED ${toplevel_sources}
)

install(FILES
    ${toplevel_headers}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include
)
target_include_directories(
    NSCLDAQFormat PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/v10
    ${CMAKE_CURRENT_SOURCE_DIR}/v11
    ${CMAKE_CURRENT_SOURCE_DIR}/v12
    ${CMAKE_CURRENT_SOURCE_DIR}/abstract
)