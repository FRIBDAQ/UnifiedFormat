add_custom_target(Documentation ALL
  COMMAND docbook2html -o htmldocs ${CMAKE_CURRENT_SOURCE_DIR}/UserAndRef.xml
  COMMENT  "Building HTML documentation"
  )

install(DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/htmldocs
  DESTINATION ${CMAKE_INSTALL_PREFIX}/share/
  )
