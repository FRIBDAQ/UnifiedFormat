add_library(
    V10Format SHARED CRingItem.cpp CPhysicsEventItem.cpp
)
target_sources(
    V10Format PRIVATE CRingItem.h DataFormat.h CPhysicsEventItem.h
)

target_include_directories(V10Format PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../abstract)

install(FILES DataFormat.h CRingItem.h CPhysicsEventItem.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/v10 )

add_executable(v10unittests
  TestRunner.cpp v10rbuftests.cpp
)
target_link_libraries(v10unittests AbstractFormat V10Format cppunit)
target_include_directories(v10unittests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../abstract)

add_test(NAME v10Format COMMAND v10unittests)