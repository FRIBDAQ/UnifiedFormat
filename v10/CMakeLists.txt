add_library(
    V10Format SHARED DataFormat.h
)
install(FILES DataFormat.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/v10 )