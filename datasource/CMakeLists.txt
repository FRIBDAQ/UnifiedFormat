add_library(
    DataSources SHARED
    DataSource.cpp
    FdDataSource.cpp
    RingDataSource.cpp
    StreamDataSource.cpp
)

target_sources(
    DataSources PRIVATE
    DataSource.h
    FdDataSource.h
    RingDataSource.h
    StreamDataSource.h
)

target_include_directories(
    DataSources PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../abstract
    ${CMAKE_BINARY_DIR}
)

target_compile_options(DataSources PRIVATE -g -O2 -DNSCLDAQROOT='${NSCLDAQ_ROOT}')

install(TARGETS DataSources
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include
)
install(FILES 
    DataSource.h FdDataSource.h RingDataSource.h StreamDataSource.h
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include
)
