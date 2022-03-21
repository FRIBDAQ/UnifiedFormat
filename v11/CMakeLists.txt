set(v11sources CRingItem.cpp CAbnormalEndItem.cpp CDataFormatItem.cpp
CGlomParameters.cpp CPhysicsEventItem.cpp CRingFragmentItem.cpp
CRingPhysicsEventCountItem.cpp
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
)

target_link_libraries(V11Format
  ${NSCLDAQ_LIB}/libdaqshm.so
  ${NSCLDAQ_LIB}/libDataFlow.so
  ${NSCLDAQ_LIB}/libException.so
)

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
)

target_link_libraries(v11unittests AbstractFormat V11Format cppunit)
target_include_directories(v11unittests PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}/../abstract
${NSCLDAQ_INC}
)


add_test(NAME v11unittests COMMAND v11unittests)
