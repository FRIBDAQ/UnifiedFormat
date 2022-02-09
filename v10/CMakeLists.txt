add_library(
    V10Format SHARED CRingItem.cpp
)
target_sources(
    V10Format PRIVATE CRingItem.h DataFormat.h
)

target_include_directories(V10Format PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../abstract)

install(FILES DataFormat.h CRingItem.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/v10 )