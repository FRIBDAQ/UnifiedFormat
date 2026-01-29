add_library(
    DataSources SHARED
    DataSource.cpp
    FdDataSource.cpp
    RingDataSource.cpp
    StreamDataSource.cpp
    URL.cpp
    SourceSelector.cpp
)

target_sources(
    DataSources PRIVATE
    DataSource.h
    FdDataSource.h
    RingDataSource.h
    StreamDataSource.h
    URL.h
    SourceSelector.h
)

target_include_directories(
    DataSources PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/abstract
    ${CMAKE_BINARY_DIR}
)

    
target_compile_options(DataSources PRIVATE -g -O2 -DNSCLDAQROOT='${NSCLDAQ_ROOT}')


# If NSCLDAQ is enabled add appropriate directories to the -I
# and link options:

if(NOT ${NSCLDAQ_ROOT} STREQUAL _)
    target_include_directories(
        DataSources PRIVATE
        ${NSCLDAQ_INC}
    )
    target_link_libraries(
        DataSources 
        ${NSCLDAQ_LIB}/libdaqshm.so
        ${NSCLDAQ_LIB}/libDataFlow.so
        ${NSCLDAQ_LIB}/libException.so
        ${NSCLDAQ_LIB}/liburl.so
    )
    target_link_Options(
        DataSources PUBLIC
        -L${NSCLDAQ_LIB} -Wl,-rpath=${NSCLDAQ_LIB}
    )
endif()
install(TARGETS DataSources
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include
)
install(FILES 
    DataSource.h FdDataSource.h RingDataSource.h StreamDataSource.h
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include
)
