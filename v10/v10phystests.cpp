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

/** @file:  v10phystests.cpp
 *  @brief: 
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CPhysicsEventItem.h"
#include "DataFormat.h"


#include <stdint.h>

class v10phytest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v10phytest);
    CPPUNIT_TEST(construct_1);
    
    CPPUNIT_TEST(bodysize_1);
    CPPUNIT_TEST(bodysize_2);
    CPPUNIT_TEST(bodyptr_1);
    CPPUNIT_TEST(bodyptr_2);
    CPPUNIT_TEST(bodyhdr_1);
    
    CPPUNIT_TEST(itemtype);
    CPPUNIT_TEST_SUITE_END();
    
private:

public:
    void setUp() {
        
    }
    void tearDown() {
        
    }
protected:
    void construct_1();
    
    void bodysize_1();
    void bodysize_2();
    
    void bodyptr_1();
    void bodyptr_2();
    void bodyhdr_1();
    
    void itemtype();
};

CPPUNIT_TEST_SUITE_REGISTRATION(v10phytest);

void v10phytest::construct_1()
{
    v10::CPhysicsEventItem item;
    EQ(v10::PHYSICS_EVENT, item.type());
}

// bodysize initially 0:

void v10phytest::bodysize_1()
{
    v10::CPhysicsEventItem item;
    EQ(size_t(0), item.getBodySize());
}
// Add 'stuff' and all should be counted.

void v10phytest::bodysize_2()
{
    v10::CPhysicsEventItem item;
    uint8_t* p = reinterpret_cast<uint8_t*>(item.getBodyCursor());
    p += 10;
    item.setBodyCursor(p);
    item.updateSize();
    EQ(size_t(10), item.getBodySize());
            
}

void v10phytest::bodyptr_1()
{
    v10::CPhysicsEventItem item;
    
    v10::RingItemHeader* pH = reinterpret_cast<v10::RingItemHeader*>(item.getItemPointer());
    uint8_t* pb = reinterpret_cast<uint8_t*>(item.getBodyPointer());
    
    EQ(reinterpret_cast<uint8_t*>(pH+1), pb);
}

void v10phytest::bodyptr_2()
{
    v10::CPhysicsEventItem item;
    
    const v10::RingItemHeader* pH = reinterpret_cast<const v10::RingItemHeader*>(item.getItemPointer());
    const uint8_t* pb = reinterpret_cast<const uint8_t*>(item.getBodyPointer());
    EQ(reinterpret_cast<const uint8_t*>(pH+1), pb);
}
void v10phytest::bodyhdr_1()
{
    v10::CPhysicsEventItem item;
    ASSERT(item.getBodyHeader() == nullptr);
}
void v10phytest::itemtype()
{
    v10::CPhysicsEventItem item;
    EQ(std::string("Event (V10)"), item.typeName());
}