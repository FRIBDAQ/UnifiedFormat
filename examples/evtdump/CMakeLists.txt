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
)

target_include_directories(evtdump PRIVATE 
  ${CMAKE_CURRENT_SOURCE_DIR}/../../
  ${CMAKE_BINARY_DIR}/examples/evtdump
)
target_compile_options(evtdump PUBLIC -g)
target_link_options(evtdump PUBLIC -g)
install(TARGETS evtdump DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
add_dependencies(evtdump cmdopts)