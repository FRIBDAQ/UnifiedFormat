add_library(
    V10Format SHARED CRingItem.cpp CPhysicsEventItem.cpp CRingFragmentItem.cpp
    CRingPhysicsEventCountItem.cpp CRingScalerItem.cpp CRingStateChangeItem.cpp
)
target_sources(
    V10Format PRIVATE CRingItem.h DataFormat.h CPhysicsEventItem.h
    CRingFragmentItem.h CRingPhysicsEventCountItem.h CRingScalerItem.h
    CRingStateChangeItem.h
)

target_include_directories(V10Format PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../abstract)

install(FILES DataFormat.h CRingItem.h CPhysicsEventItem.h
  CRingFragmentItem.h CRingPhysicsEventCountItem.h CRingScalerItem.h
  CRingStateChangeItem.h
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include/v10 )

add_executable(v10unittests
  TestRunner.cpp v10rbuftests.cpp v10phystests.cpp v10fragtests.cpp
  v10counttests.cpp v10scalertests.cpp
)
target_link_libraries(v10unittests AbstractFormat V10Format cppunit)
target_include_directories(v10unittests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../abstract)

add_test(NAME v10Format COMMAND v10unittests)