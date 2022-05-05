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

/** @file:  v12counttests.cpp
 *  @brief:  Test v12::CRingPhysicsEventCountItem
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CRingPhysicsEventCountItem.h"
#include "DataFormat.h"

#include <time.h>

class v12counttest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v12counttest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    CPPUNIT_TEST(construct_3);
    CPPUNIT_TEST(construct_4);
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
    void construct_3();
    void construct_4();
};

CPPUNIT_TEST_SUITE_REGISTRATION(v12counttest);

// Default constructor ...no body header.  Body zero but for divisor
void v12counttest::construct_1()
{
    time_t now = time(nullptr);
    v12::CRingPhysicsEventCountItem item;
    const v12::PhysicsEventCountItem* pItem =
        reinterpret_cast<const v12::PhysicsEventCountItem*>(item.getItemPointer());
    EQ(v12::PHYSICS_EVENT_COUNT, pItem->s_header.s_type);
    EQ(
        sizeof(v12::RingItemHeader) + sizeof(uint32_t)
         + sizeof(v12::PhysicsEventCountItemBody),
        size_t(pItem->s_header.s_size)
    );
    
    EQ(sizeof(uint32_t), size_t(pItem->s_body.u_noBodyHeader.s_empty));
    const v12::PhysicsEventCountItemBody* pBody =
        &(pItem->s_body.u_noBodyHeader.s_body);
    EQ(uint32_t(0), pBody->s_timeOffset);
    EQ(uint32_t(1), pBody->s_offsetDivisor);
    ASSERT(pBody->s_timestamp - now <= 1);
    EQ(uint32_t(0), pBody->s_originalSid);
    EQ(uint64_t(0), pBody->s_eventCount);
    
}
/// Constructor with count and offset:

void v12counttest::construct_2()
{
    time_t now = time(nullptr);
    v12::CRingPhysicsEventCountItem item(1234, 10);
    
    const v12::PhysicsEventCountItem* pItem =
        reinterpret_cast<const v12::PhysicsEventCountItem*>(item.getItemPointer());
    EQ(v12::PHYSICS_EVENT_COUNT, pItem->s_header.s_type);
    EQ(
        sizeof(v12::RingItemHeader) + sizeof(uint32_t)
         + sizeof(v12::PhysicsEventCountItemBody),     // No body header.
        size_t(pItem->s_header.s_size)
    );
    
    EQ(sizeof(uint32_t), size_t(pItem->s_body.u_noBodyHeader.s_empty));
    const v12::PhysicsEventCountItemBody* pBody =
        &(pItem->s_body.u_noBodyHeader.s_body);
    EQ(uint32_t(10), pBody->s_timeOffset);
    EQ(uint32_t(1), pBody->s_offsetDivisor);
    EQ(now, time_t(pBody->s_timestamp));
    EQ(uint32_t(0), pBody->s_originalSid);
    EQ(uint64_t(1234), pBody->s_eventCount);
}
// Initial SID

void v12counttest::construct_3()
{
    time_t now = time(nullptr);
    v12::CRingPhysicsEventCountItem item(12345, 20, now, 2);
    
    const v12::PhysicsEventCountItem* pItem =
        reinterpret_cast<const v12::PhysicsEventCountItem*>(item.getItemPointer());
    
    EQ(v12::PHYSICS_EVENT_COUNT, pItem->s_header.s_type);
    EQ(
        sizeof(v12::RingItemHeader)
         + sizeof(v12::BodyHeader)
         + sizeof(v12::PhysicsEventCountItemBody),    
        size_t(pItem->s_header.s_size)
    );
    const v12::BodyHeader* pBh = &(pItem->s_body.u_hasBodyHeader.s_bodyHeader);
    EQ(sizeof(v12::BodyHeader), size_t(pBh->s_size));
    EQ(uint64_t(0), pBh->s_timestamp);
    EQ(uint32_t(2), pBh->s_sourceId);
    EQ(uint32_t(0), pBh->s_barrier);
    
    const v12::PhysicsEventCountItemBody* pBody =
        &(pItem->s_body.u_hasBodyHeader.s_body);
    EQ(uint32_t(20), pBody->s_timeOffset);
    EQ(uint32_t(1), pBody->s_offsetDivisor);
    EQ(now, time_t(pBody->s_timestamp));
    EQ(uint32_t(2), pBody->s_originalSid);
    EQ(uint64_t(12345), pBody->s_eventCount);
}
// fully specfied construction

void v12counttest::construct_4()
{
    time_t now = time(nullptr);
    v12::CRingPhysicsEventCountItem item(0x1234567890, 2, 1, 123456, 20, now);
    
    const v12::PhysicsEventCountItem* pItem =
        reinterpret_cast<const v12::PhysicsEventCountItem*>(item.getItemPointer());
    
    EQ(v12::PHYSICS_EVENT_COUNT, pItem->s_header.s_type);
    EQ(
        sizeof(v12::RingItemHeader)
         + sizeof(v12::BodyHeader)
         + sizeof(v12::PhysicsEventCountItemBody),    
        size_t(pItem->s_header.s_size)
    );
    const v12::BodyHeader* pBh = &(pItem->s_body.u_hasBodyHeader.s_bodyHeader);
    EQ(sizeof(v12::BodyHeader), size_t(pBh->s_size));
    EQ(uint64_t(0x1234567890), pBh->s_timestamp);
    EQ(uint32_t(2), pBh->s_sourceId);
    EQ(uint32_t(1), pBh->s_barrier);
    
    const v12::PhysicsEventCountItemBody* pBody =
        &(pItem->s_body.u_hasBodyHeader.s_body);
    EQ(uint32_t(20), pBody->s_timeOffset);
    EQ(uint32_t(1), pBody->s_offsetDivisor);
    EQ(now, time_t(pBody->s_timestamp));
    EQ(uint32_t(2), pBody->s_originalSid);
    EQ(uint64_t(123456), pBody->s_eventCount);
}