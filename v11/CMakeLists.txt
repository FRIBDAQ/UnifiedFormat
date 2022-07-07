set(v11sources CRingItem.cpp CAbnormalEndItem.cpp CDataFormatItem.cpp
CGlomParameters.cpp CPhysicsEventItem.cpp CRingFragmentItem.cpp
CRingPhysicsEventCountItem.cpp CRingScalerItem.cpp
CRingStateChangeItem.cpp CRingTextItem.cpp CUnknownFragment.cpp
RingItemFactory.cpp
)
set(v11headers
  CRingItem.h
    DataFormat.h
    CAbnormalEndItem.h
    CDataFormatItem.h
    CGlomParameters.h
    CPhysicsEventItem.h
    CRingFragmentItem.h
    CRingPhysicsEventCountItem.h
    CRingScalerItem.h
    CRingStateChangeItem.h
    CRingTextItem.h
    CUnknownFragment.h
    RingItemFactory.h
    )
add_library(
    V11Format SHARED ${v11sources}
)

target_sources(
     V11Format PRIVATE
     ${v11headers}
)
target_include_directories(V11Format PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/../abstract ${NSCLDAQ_INC}
      ${CMAKE_BINARY_DIR}
)

install(TARGETS V11Format LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include/v11
)

if (${NSCLDAQ_ROOT} STREQUAL _)
else ()
target_link_libraries(V11Format
  ${NSCLDAQ_LIB}/libdaqshm.so
  ${NSCLDAQ_LIB}/libDataFlow.so
  ${NSCLDAQ_LIB}/libException.so
)
target_link_options(V11Format PRIVATE -g PUBLIC -Wl,-rpath=${NSCLDAQ_LIB})
endif()
target_compile_options(V11Format PRIVATE -g -DNSCLDAQ_ROOT='${NSCLDAQ_ROOT}')
install(FILES
    ${v11headers}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/v11
)

add_executable(v11unittests
TestRunner.cpp
v11ringtests.cpp
v11abendtests.cpp 
v11dfmttests.cpp
v11glomtests.cpp
v11eventtests.cpp
v11fragtests.cpp
v11counttests.cpp
v11sctests.cpp
v11statetest.cpp
v11texttests.cpp
v11unktests.cpp
v11factoryTests.cpp
)

target_link_libraries(v11unittests AbstractFormat V11Format cppunit)
target_include_directories(v11unittests PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}/../abstract
${NSCLDAQ_INC}
${CMAKE_BINARY_DIR}
)

target_compile_options(v11unittests PRIVATE -g -DNSCLDAQ_ROOT='${NSCLDAQ_ROOT}')
target_link_options(v11unittests PRIVATE -g)
add_test(NAME v11unittests COMMAND v11unittests)
