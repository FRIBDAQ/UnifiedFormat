

add_library(
    V10Format SHARED CRingItem.cpp CPhysicsEventItem.cpp CRingFragmentItem.cpp
    CRingPhysicsEventCountItem.cpp CRingScalerItem.cpp CRingStateChangeItem.cpp
    CRingTextItem.cpp RingItemFactory.cpp
)
target_sources(
    V10Format PRIVATE CRingItem.h DataFormat.h CPhysicsEventItem.h
    CRingFragmentItem.h CRingPhysicsEventCountItem.h CRingScalerItem.h
    CRingStateChangeItem.h CRingTextItem.h RingItemFactory.h
)

target_include_directories(V10Format PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../abstract ${NSCLDAQ_INC}
  )
if(${NSCLDAQ_ROOT} STREQUAL _)
else ()
target_link_libraries(V10Format
  ${NSCLDAQ_LIB}/libdaqshm.so
  ${NSCLDAQ_LIB}/libDataFlow.so
  ${NSCLDAQ_LIB}/libException.so
  )
target_link_options(V10Format
  PUBLIC -Wl,-rpath=${NSCLDAQ_LIB}
  PRIVATE -g
)
endif()

target_compile_options(V10Format PRIVATE -g -DNSCLDAQ_ROOT='${NSCLDAQ_ROOT}')


install(TARGETS
  V10Format LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include/v10
)

install(FILES DataFormat.h CRingItem.h CPhysicsEventItem.h
  CRingFragmentItem.h CRingPhysicsEventCountItem.h CRingScalerItem.h
  CRingStateChangeItem.h CRingTextItem.h RingItemFactory.h
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include/v10 )

add_executable(v10unittests
  TestRunner.cpp v10rbuftests.cpp v10phystests.cpp v10fragtests.cpp
  v10counttests.cpp v10scalertests.cpp v10statetests.cpp v10txttests.cpp
  v10factorytests.cpp
)
target_link_libraries(v10unittests AbstractFormat V10Format cppunit)
target_include_directories(v10unittests PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}/../abstract
${NSCLDAQ_INC}
)
target_compile_options(v10unittests PRIVATE -g -DNSCLDAQ_ROOT='${NSCLDAQ_ROOT}')
target_link_options(v10unittests PRIVATE -g)

add_test(NAME v10Format COMMAND v10unittests)
