# cmake/FindCppUnit.cmake

find_path(CppUnit_INCLUDE_DIR NAMES cppunit/TestRunner.h)

find_library(CppUnit_LIBRARY NAMES cppunit)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CppUnit DEFAULT_MSG CppUnit_LIBRARY CppUnit_INCLUDE_DIR)

if(CppUnit_FOUND)
  set(CppUnit_LIBRARIES ${CppUnit_LIBRARY})
  set(CppUnit_INCLUDE_DIRS ${CppUnit_INCLUDE_DIR})
endif()

mark_as_advanced(CppUnit_INCLUDE_DIR CppUnit_LIBRARY)
