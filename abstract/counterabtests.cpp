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

/** @file:  counterabtests.cpp
 *  @brief: Tests for the CRingPhysicsEventCountItem class
 *         in abstract.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CRingPhysicsEventCountItem.h"
#include <time.h>
#include <stdint.h>



class counterabtest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(counterabtest);
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

CPPUNIT_TEST_SUITE_REGISTRATION(counterabtest);

// Default constructor 
void counterabtest::construct_1()
{
    CRingPhysicsEventCountItem* pItem;
    CPPUNIT_ASSERT_NO_THROW(pItem = new CRingPhysicsEventCountItem);
    uint32_t* pBody =
        reinterpret_cast<uint32_t*>(pItem->getBodyPointer());
    EQ(uint32_t(0), *pBody);    // Time offset.
    pBody++;
    EQ(uint32_t(1), *pBody);    // Offset divisor.
    pBody++;
    
    time_t now = time(nullptr);
    ASSERT((now - *pBody) <= 1); // timestamp - might be on a sec. edge.
    pBody++;
    EQ(uint32_t(0), *pBody);     // original source id.
    pBody++;
    uint64_t* p64 = reinterpret_cast<uint64_t*>(pBody);
    EQ(uint64_t(0), *p64);
    
    
}