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

/** @file:  v11eventtests.cpp
 *  @brief:  Tests for V11::CPhysicsEventItem.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CPhysicsEventItem.h"
#include "DataFormat.h"

class v11eventtest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v11eventtest);
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

CPPUNIT_TEST_SUITE_REGISTRATION(v11eventtest);

// Minimal - no payload.
void v11eventtest::construct_1()
{
    v11::CPhysicsEventItem item(100);
    
    EQ(v11::PHYSICS_EVENT, item.type());
    EQ(sizeof(v11::RingItemHeader) + sizeof(uint32_t), size_t(item.size()));
    
    const v11::RingItemHeader* pHdr =
        reinterpret_cast<const v11::RingItemHeader*>(item.getItemPointer());
    const uint32_t* pz = reinterpret_cast<const uint32_t*>(pHdr+1);
    EQ(uint32_t(0), *pz);
}