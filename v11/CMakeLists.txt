add_library(
    V11Format SHARED CRingItem.cpp CAbnormalEndItem.cpp
)

target_sources(
     V11Format PRIVATE
     CRingItem.h DataFormat.h CAbnormalEndItem.h
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
    CAbnormalEndItem.h
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/v11
)

add_executable(v11unittests TestRunner.cpp v11ringtests.cpp
)

target_link_libraries(v11unittests AbstractFormat V11Format cppunit)
target_include_directories(v11unittests PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}/../abstract
${NSCLDAQ_INC}
)


add_test(NAME v11unittests COMMAND v11unittests)
