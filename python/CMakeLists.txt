find_package(Python3 COMPONENTS Development REQUIRED)
add_library(daqformat MODULE format_abnormalend.cpp  format_factory.cpp     format_ringfragment.cpp  format_version.cpp
format_eventcount.cpp   format_glomparams.cpp  format_ringitem.cpp      format_statechange.cpp
format_event.cpp        format_Module.cpp      format_scaler.cpp        format_textlist.cpp
format_abnormalend.h  format_factory.h       format_ringitem.h     format_textlist.h
format_eventcount.h   format_glomparams.h    format_scaler.h       format_version.h
format_event.h        format_ringfragment.h  format_statechange.h)

target_link_libraries(daqformat PRIVATE  NSCLDAQFormat V10Format V11Format V12Format
		AbstractFormat Python3::Module)
set_target_properties(daqformat PROPERTIES PREFIX "${Python_MODULE_PREFIX}" )

target_include_directories(daqformat PRIVATE 
    ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/abstract  ${CMAKE_BINARY_DIR}
    )

install(TARGETS daqformat LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/python)
target_link_options(daqformat PUBLIC -Wl,-rpath=${CMAKE_INSTALL_PREFIX}/lib)