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
target_compile_options(evtdump PUBLIC -I${CMAKE_BINARY_DIR}/examples/evtdump)
install(TARGETS evtdump DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
add_dependencies(evtdump cmdopts)