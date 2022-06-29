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
  RingDataSource.cpp
)

target_include_directories(evtdump PRIVATE 
  ${CMAKE_CURRENT_SOURCE_DIR}/../../
  ${CMAKE_CURRENT_SOURCE_DIR}/../../abstract
  ${CMAKE_BINARY_DIR}/examples/evtdump
  ${NSCLDAQ_INC}
)
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
target_compile_options(evtdump PUBLIC -g)
target_link_options(evtdump PUBLIC -g -Wl,-rpath=${CMAKE_INSTALL_PREFIX}/lib -Wl,-rpath=${NSCLDAQ_LIB})
install(TARGETS evtdump DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
add_dependencies(evtdump cmdopts)