add_custom_target(
  cmdopts ALL
  COMMAND gengetopt <${CMAKE_CURRENT_SOURCE_DIR}/dumperargs.ggo
  COMMAND $(CC) -c cmdline.c
  SOURCES dumperargs.ggo
  COMMENT "Building gengetopt args parser"
  BYPRODUCTS   cmdline.o cmdline.h
  )


add_executable(
  evtdump
  evtdump.cpp cmdline.o cmdline.h
  DataSource.cpp
  FdDataSource.cpp StreamDataSource.cpp
  URL.cpp
)

target_compile_options(evtdump PUBLIC -g -O2)
add_dependencies(evtdump cmdopts)


if(${NSCLDAQ_ROOT} STREQUAL _)
  target_include_directories(evtdump PRIVATE 
  ${CMAKE_CURRENT_SOURCE_DIR}/../../
  ${CMAKE_CURRENT_SOURCE_DIR}/../../abstract
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )
target_link_libraries(evtdump
  NSCLDAQFormat
  AbstractFormat
  V10Format
  V11Format
  V12Format
  )
  target_link_options(evtdump PUBLIC -g -Wl,-rpath=${CMAKE_INSTALL_PREFIX}/lib )
else ()
  target_link_libraries(evtdump
  NSCLDAQFormat
  AbstractFormat
  V10Format
  V11Format
  V12Format
  ${NSCLDAQ_LIB}/libdaqshm.so
  ${NSCLDAQ_LIB}/libDataFlow.so
  ${NSCLDAQ_LIB}/libException.so
  ${NSCLDAQ_LIB}/liburl.so
  )
  target_include_directories(evtdump PRIVATE 
  ${CMAKE_CURRENT_SOURCE_DIR}/../../
  ${CMAKE_CURRENT_SOURCE_DIR}/../../abstract
  ${NSCLDAQ_INC}
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )
  target_sources(evtdump PRIVATE RingDataSource.cpp)

  target_link_options(evtdump PUBLIC -g -Wl,-rpath=${CMAKE_INSTALL_PREFIX}/lib -Wl,-rpath=${NSCLDAQ_LIB})

endif()




install(TARGETS evtdump DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

install(FILES
  evtdump.cpp 
  DataSource.cpp
  FdDataSource.cpp StreamDataSource.cpp
  RingDataSource.cpp
  dumperargs.ggo
  
  DESTINATION ${CMAKE_INSTALL_PREFIX}/share/examples/evtdump
)
