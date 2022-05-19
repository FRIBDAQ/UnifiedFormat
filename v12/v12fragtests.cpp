/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     FRIB
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  v12fragtests.cpp
 *  @brief: Tests of v12::CRingFragmentItem and v12::CUnknownFragment
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CRingFragmentItem.h"
#include "CUnknownFragment.h"
#include "DataFormat.h"

class v12fragtest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v12fragtest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST_SUITE_END();
    
private:

public:
    void setUp() {
        
    }
    void tearDown() {
        
    }
protected:
    void construct_1();
};

CPPUNIT_TEST_SUITE_REGISTRATION(v12fragtest);

void v12fragtest::construct_1()
{
}