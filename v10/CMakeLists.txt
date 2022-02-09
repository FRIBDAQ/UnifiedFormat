add_library(
    V10Format SHARED CRingItem.cpp
)
target_sources(
    V10Format PRIVATE CRingItem.h DataFormat.h
)

target_include_directories(V10Format PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../abstract)

install(FILES DataFormat.h CRingItem.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/v10 )

add_executable(v10unittests
  TestRunner.cpp
)
target_link_libraries(v10unittests AbstractFormat V10Format cppunit)
add_test(NAME v10Format COMMAND v10unittests)