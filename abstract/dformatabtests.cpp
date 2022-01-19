/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  dformatabtests.cpp
 *  @brief: Tests for the abstract data format item.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <DataFormat.h>
#include "CDataFormatItem.h"
#include <stdexcept>
#include <string>

class dformatabtest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(dformatabtest);
    CPPUNIT_TEST(major);
    CPPUNIT_TEST(minor);
    CPPUNIT_TEST(type_name);
    CPPUNIT_TEST(getbodyheader);
    CPPUNIT_TEST(setbodyheader);
    CPPUNIT_TEST_SUITE_END();
    
private:

public:
    void setUp() {
        
    }
    void tearDown() {
        
    }
protected:
    void major();
    void minor();
    void type_name();
    void getbodyheader();
    void setbodyheader();
};

CPPUNIT_TEST_SUITE_REGISTRATION(dformatabtest);

void dformatabtest::major()
{
    CDataFormatItem item;
    EQ(FORMAT_MAJOR, item.getMajor());
}
void dformatabtest::minor()
{
    CDataFormatItem item;
    EQ(FORMAT_MINOR, item.getMinor());
    
}
void dformatabtest::type_name()
{
    CDataFormatItem item;
    EQ(std::string("Ring Item format version"), item.typeName());
}
void dformatabtest::getbodyheader()
{
    CDataFormatItem item;
    CPPUNIT_ASSERT_THROW(item.getBodyHeader(), std::logic_error);
}
void dformatabtest::setbodyheader()
{
    CDataFormatItem item;
    CPPUNIT_ASSERT_THROW(item.setBodyHeader(12345, 1, 0), std::logic_error);
}