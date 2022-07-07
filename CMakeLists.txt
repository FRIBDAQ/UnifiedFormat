cmake_minimum_required(VERSION 3.13)
project(UnifiedFormat VERSION 0.1)

set(NSCLDAQ_ROOT "_"  CACHE STRING "_")
set(NSCLDAQ_INC ${NSCLDAQ_ROOT}/include CACHE STRING "${NSCLDAQ_ROOT}/include")
set(NSCLDAQ_LIB ${NSCLDAQ_ROOT}/lib CACHE STRING "${NSCLDAQ_ROOT}/lib")

# Make the fmtconfig.h header:

file(WRITE ${CMAKE_BINARY_DIR}/fmtconfig.h "#ifndef FMTCONFIG_H\n")
file(APPEND ${CMAKE_BINARY_DIR}/fmtconfig.h "#define FMTCONFIG_H\n")
if(${NSCLDAQ_ROOT} STREQUAL _)
else()
 file(APPEND ${CMAKE_BINARY_DIR}/fmtconfig.h "#define HAVE_NSCLDAQ\n")
endif()
file(APPEND ${CMAKE_BINARY_DIR}/fmtconfig.h "#endif\n")

install(FILES ${CMAKE_BINARY_DIR}/fmtconfig.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
enable_testing()



# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#  The subdirectories that have libraries:

add_subdirectory(abstract)
add_subdirectory(v10)
add_subdirectory(v11)
add_subdirectory(v12)
add_subdirectory(examples)
add_subdirectory(docs)

#  Now the top level library target:

set (toplevel_sources
    NSCLDAQFormatFactorySelector.cpp
)

set (toplevel_headers
    NSCLDAQFormatFactorySelector.h
    ${CMAKE_BINARY_DIR}/fmtconfig.h
)

add_library(
    NSCLDAQFormat SHARED ${toplevel_sources}
)

install(FILES
    ${toplevel_headers}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include
)

target_compile_options(NSCLDAQFormat PRIVATE -g)
target_link_options(NSCLDAQFormat PRIVATE -g)

install(TARGETS NSCLDAQFormat LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib )
target_include_directories(
    NSCLDAQFormat PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/v10
    ${CMAKE_CURRENT_SOURCE_DIR}/v11
    ${CMAKE_CURRENT_SOURCE_DIR}/v12
    ${CMAKE_CURRENT_SOURCE_DIR}/abstract
    ${CMAKE_BINARY_DIR}
)

## Unit tests

add_executable(selectortests
TestRunner.cpp
selectortests.cpp
)
target_include_directories(selectortests PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}/abstract
${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_BINARY_DIR}
)

target_compile_options(selectortests PRIVATE -g)
target_link_options(selectortests PRIVATE -g)

target_link_libraries(selectortests
    NSCLDAQFormat AbstractFormat V10Format V11Format V12Format
    cppunit
)
add_test(NAME selector COMMAND selectortests)
