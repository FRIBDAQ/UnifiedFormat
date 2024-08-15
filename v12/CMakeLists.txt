set (v12headers
  DataFormat.h
  CRingItem.h
  CAbnormalEndItem.h
  CDataFormatItem.h
  CGlomParameters.h
  CPhysicsEventItem.h
  CRingPhysicsEventCountItem.h
  CRingScalerItem.h
  CRingStateChangeItem.h
  CRingTextItem.h
  CRingFragmentItem.h
  CUnknownFragment.h
  RingItemFactory.h
)
set (v12sources
  CRingItem.cpp
  CAbnormalEndItem.cpp
  CDataFormatItem.cpp
  CGlomParameters.cpp
  CPhysicsEventItem.cpp
  CRingPhysicsEventCountItem.cpp
  CRingScalerItem.cpp
  CRingStateChangeItem.cpp
  CRingTextItem.cpp
  CRingFragmentItem.cpp
  CUnknownFragment.cpp
  RingItemFactory.cpp
)
add_library(
    V12Format STATIC ${v12sources}
)

install(TARGETS V12Format LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include/v12
)

target_sources(
    V12Format PRIVATE
    ${v12headers}
)
target_include_directories(
    V12Format PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../abstract ${NSCLDAQ_INC}
    ${CMAKE_BINARY_DIR}
)
if (${NSCLDAQ_ROOT} STREQUAL _)
else ()
target_link_libraries(
    V12Format
    ${NSCLDAQ_LIB}/libdaqshm.so
    ${NSCLDAQ_LIB}/libDataFlow.so
    ${NSCLDAQ_LIB}/libException.so
   
)
target_link_options(V12Format PRIVATE -g PUBLIC -Wl,-rpath=${NSCLDAQ_LIB})
endif()

target_compile_options(V12Format PRIVATE -g -O2 -DNSCLDAQ_ROOT='${NSCLDAQ_ROOT}')
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
  v12scltests.cpp
  v12statetests.cpp
  v12texttests.cpp
  v12fragtests.cpp
  v12factorytests.cpp
)

target_link_libraries(v12unittests V12Format  AbstractFormat cppunit)
target_include_directories(v12unittests PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}/../abstract
${NSCLDAQ_INC} ${CMAKE_BINARY_DIR}
)

target_compile_options(v12unittests PRIVATE -g -O2  -DNSCLDAQ_ROOT='${NSCLDAQ_ROOT}')
target_link_options(v12unittests PRIVATE -g)
add_test(NAME v12unittests COMMAND v12unittests)
