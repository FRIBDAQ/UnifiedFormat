add_library(
    V11Format SHARED CRingItem.cpp
)

target_sources(
     V11Format PRIVATE
     CRingItem.h DataFormat.h
)
target_include_directories(V11Format PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/../abstract ${NSCLDAQ_INC}
)

target_link_libraries(V11Format
  ${NSCLDAQ_LIB}/libdaqshm.so
  ${NSCLDAQ_LIB}/libDataFlow.so
  ${NSCLDAQ_LIB}/libException.so
)

install(FILES
    CRingItem.h
    DataFormat.h
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/v11
)