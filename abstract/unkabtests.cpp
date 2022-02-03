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

/** @file:  unkabtests.cpp
 *  @brief: Test unknown fragment ring item.
 *    Note that this is a derived wrapper class for CRingFragmentItem that
 *    does little more than change the ring item type.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CUnknownFragment.h"
#include <DataFormat.h>

class unkabtest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(unkabtest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    CPPUNIT_TEST_SUITE_END();
    
private:

public:
    void setUp() {
        
    }
    void tearDown() {
        
    }
protected:
    void construct_1();
    void construct_2();
};

CPPUNIT_TEST_SUITE_REGISTRATION(unkabtest);

// Construction yields the right type.
void unkabtest::construct_1()
{
    CUnknownFragment item(12345, 3, 0, 0, nullptr);
    EQ(std::string("Fragment with unknown payload"), item.typeName());
}
void unkabtest::construct_2()
{
    CUnknownFragment item(12345, 3, 0, 0, nullptr);
    EQ(EVB_UNKNOWN_PAYLOAD, item.type());
}