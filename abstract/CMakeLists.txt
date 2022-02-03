add_library(
    AbstractFormat SHARED
    CRingItem.cpp CAbnormalEndItem.cpp CDataFormatItem.cpp CGlomParameters.cpp
    CPhysicsEventItem.cpp CRingFragmentItem.cpp
    CRingPhysicsEventCountItem.cpp CRingScalerItem.cpp CRingTextItem.h
)
target_sources(
    AbstractFormat PUBLIC
    CRingItem.h CAbnormalEndItem.h CDataFormatItem.h CGlomParameters.h
    CPhysicsEventItem.h CRingFragmentItem.h CRingPhysicsEventCountItem.h
    CRingScalerItem.h CRingTextItem.cpp
)
get_target_property(ABSTRACT_FORMAT_PUBLIC_HEADERS AbstractFormat INTERFACE_SOURCES)
set_target_properties(
    AbstractFormat PROPERTIES PUBLIC_HEADER "${ABSTRACT_FORMAT_PUBLIC_HEADERS}"
)


target_include_directories(AbstractFormat SYSTEM
PUBLIC /usr/opt/daq/12.0-pre3/include
)
target_compile_options(AbstractFormat PRIVATE -g)
target_link_options(AbstractFormat PRIVATE -g)

install(TARGETS AbstractFormat LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_PREFIX}/include"
)

#  Unit tests:

add_executable(
    unittests
    TestRunner.cpp ringitemabtests.cpp abendabtests.cpp dformatabtests.cpp
    glomabtests.cpp physabtests.cpp fragabtests.cpp counterabtests.cpp
    scabtests.cpp textabtest.cpp
    CRingItem.h CAbnormalEndItem.h CGlomParameters.h CPhysicsEventItem.h
    CRingFragmentItem.h CRingPhysicsEventCountItem.h CRingScalerItem.h
    CRingTextItem.h
    Asserts.h
)

target_include_directories(
    unittests SYSTEM
    PUBLIC /usr/opt/daq/12.0-pre3/include
)
target_link_libraries(unittests AbstractFormat cppunit)
target_compile_options(unittests PRIVATE -g)
target_link_options(unittests PRIVATE -g)


add_test(NAME abstract COMMAND unittests)