set (v12headers
  DataFormat.h
  CRingItem.h
  CAbnormalEndItem.h
  CDataFormatItem.h
  CGlomParameters.h
  CPhysicsEventItem.h
  CRingPhysicsEventCountItem.h
)
set (v12sources
  CRingItem.cpp
  CAbnormalEndItem.cpp
  CDataFormatItem.cpp
  CGlomParameters.cpp
  CPhysicsEventItem.cpp
  CRingPhysicsEventCountItem.cpp
)
add_library(
    V12Format SHARED ${v12sources}
)

target_sources(
    V12Format PRIVATE
    ${v12headers}
)
target_include_directories(
    V12Format PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../abstract ${NSCLDAQ_INC}
)
target_link_libraries(
    V12Format
    ${NSCLDAQ_LIB}/libdaqshm.so
    ${NSCLDAQ_LIB}/libDataFlow.so
    ${NSCLDAQ_LIB}/libException.so
   
)

target_compile_options(V12Format PRIVATE -g)
target_link_options(V12Format PRIVATE -g)
install(FILES
    ${v12headers}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/v12
)

add_executable(v12unittests
  TestRunner.cpp
  v12ringtest.cpp
  v12abend.cpp
  v12fmttests.cpp
  v12glomtests.cpp
  v12phystests.cpp
  v12counttests.cpp
)

target_link_libraries(v12unittests AbstractFormat V12Format cppunit)
target_include_directories(v12unittests PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}/../abstract
${NSCLDAQ_INC}
)

target_compile_options(v12unittests PRIVATE -g)
target_link_options(v12unittests PRIVATE -g)
add_test(NAME v12unittests COMMAND v12unittests)
