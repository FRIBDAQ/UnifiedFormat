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

/** @file:  sabchangetests.cpp
 *  @brief: 
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CRingStateChangeItem.h"
#include <stdexcept>


class abschangetest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(abschangetest);
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

CPPUNIT_TEST_SUITE_REGISTRATION(abschangetest);

// Construct only with reason:
void abschangetest::construct_1()
{
    CPPUNIT_ASSERT_NO_THROW(CRingStateChangeItem i1(BEGIN_RUN));
    CPPUNIT_ASSERT_NO_THROW(CRingStateChangeItem i2(END_RUN));
    CPPUNIT_ASSERT_NO_THROW(CRingStateChangeItem i3(PAUSE_RUN));
    CPPUNIT_ASSERT_NO_THROW(CRingStateChangeItem i4(RESUME_RUN));
    
    CPPUNIT_ASSERT_THROW(
        CRingStateChangeItem item(PHYSICS_EVENT),
        std::logic_error
    );
}