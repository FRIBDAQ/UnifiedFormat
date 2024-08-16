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

using namespace ufmt;

class v12counttest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v12counttest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    CPPUNIT_TEST(construct_3);
    CPPUNIT_TEST(construct_4);
    
    CPPUNIT_TEST(getOffset_1);
    CPPUNIT_TEST(getOffset_2);
    
    CPPUNIT_TEST(setOffset_1);
    CPPUNIT_TEST(setOffset_2);
    
    CPPUNIT_TEST(elapsed_1);
    CPPUNIT_TEST(elapsed_2);
    
    CPPUNIT_TEST(divisor_1);
    CPPUNIT_TEST(divisor_2);
    
    CPPUNIT_TEST(gettime_1);
    CPPUNIT_TEST(gettime_2);
    
    CPPUNIT_TEST(settime_1);
    CPPUNIT_TEST(settime_2);
    
    CPPUNIT_TEST(getcount_1);
    CPPUNIT_TEST(getcount_2);
    
    CPPUNIT_TEST(originalsid_1);
    CPPUNIT_TEST(originalsid_2);
    
    CPPUNIT_TEST(bsize_1);
    CPPUNIT_TEST(bsize_2);
    
    CPPUNIT_TEST(bptr_1);
    CPPUNIT_TEST(bptr_2);
    CPPUNIT_TEST(bptr_3);
    CPPUNIT_TEST(bptr_4);
    
    CPPUNIT_TEST(hasbhdr_1);
    CPPUNIT_TEST(hasbhdr_2);
    
    CPPUNIT_TEST(ets_1);
    CPPUNIT_TEST(ets_2);
    
    CPPUNIT_TEST(sid_1);
    CPPUNIT_TEST(sid_2);
    
    CPPUNIT_TEST(bar_1);
    CPPUNIT_TEST(bar_2);
    
    CPPUNIT_TEST(setbhdr_1);
    CPPUNIT_TEST(setbhdr_2);
    
    CPPUNIT_TEST(getbhdr_1);
    CPPUNIT_TEST(getbhdr_2);
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
    
    void getOffset_1();
    void getOffset_2();
    
    void setOffset_1();
    void setOffset_2();
    
    void elapsed_1();
    void elapsed_2();
    
    void divisor_1();
    void divisor_2();
    
    void gettime_1();
    void gettime_2();
    
    void settime_1();
    void settime_2();
    
    void getcount_1();
    void getcount_2();
    
    void originalsid_1();
    void originalsid_2();
    
    void bsize_1();
    void bsize_2();
    
    void bptr_1();
    void bptr_2();
    void bptr_3();
    void bptr_4();
    
    void hasbhdr_1();
    void hasbhdr_2();
    
    void ets_1();
    void ets_2();
    
    void sid_1();
    void sid_2();
    
    void bar_1();
    void bar_2();
    
    void setbhdr_1();
    void setbhdr_2();
    
    void getbhdr_1();
    void getbhdr_2();
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
    EQ(uint64_t(0xffffffffffffffff), pBh->s_timestamp);
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
// retrieve time offset with no body header.
void v12counttest::getOffset_1()
{
    v12::CRingPhysicsEventCountItem item(12345, 10);
    EQ(uint32_t(10), item.getTimeOffset());
    
}

// retrieve time offset with body header
void v12counttest::getOffset_2()
{
    v12::CRingPhysicsEventCountItem item(1234, 10, time(nullptr), 2);
    
    EQ(uint32_t(10), item.getTimeOffset());
}
// set time offset with no body header.
void v12counttest::setOffset_1()
{
    v12::CRingPhysicsEventCountItem item(12345, 10);
    item.setTimeOffset(20);
    EQ(uint32_t(20), item.getTimeOffset());
}
// set time offset with body header
void v12counttest::setOffset_2()
{
    v12::CRingPhysicsEventCountItem item(1234, 10, time(nullptr), 2);
    item.setTimeOffset(35);
    EQ(uint32_t(35), item.getTimeOffset());
}
// compute elapsed with no body header.

void v12counttest::elapsed_1()
{
    v12::CRingPhysicsEventCountItem item(12345, 10, 2);
    EQ(float(5), item.computeElapsedTime());
}
// compute elapsed time with body header.
void v12counttest::elapsed_2()
{
    v12::CRingPhysicsEventCountItem item(1234, 10, time(nullptr), 2, 2);
    EQ(float(5), item.computeElapsedTime());
}

// get time divisor from no body header.
void v12counttest::divisor_1()
{
    v12::CRingPhysicsEventCountItem item(12345, 10, 2);
    EQ(uint32_t(2), item.getTimeDivisor());
}
// Get time divisor from body header:

void v12counttest::divisor_2()
{
    v12::CRingPhysicsEventCountItem item(1234, 10, time(nullptr), 2, 2);
    EQ(uint32_t(2), item.getTimeDivisor());
}
// Get tod stamp from non body header item.

void v12counttest::gettime_1()
{
    time_t now = time(nullptr);
    v12::CRingPhysicsEventCountItem item(12345, 10, 2);
    ASSERT(item.getTimestamp() - now <= 1);    // Due to skew in time.
}
// Get tod stamp from body header item.
void v12counttest::gettime_2()
{
    time_t now = time(nullptr);
    v12::CRingPhysicsEventCountItem item(1234, 10, now, 2, 2);
    EQ(now, item.getTimestamp());
}

void v12counttest::settime_1()
{
    time_t now = time(nullptr);
    v12::CRingPhysicsEventCountItem item(12345, 10, 2);
    item.setTimestamp(now+10);
    EQ(now+10, item.getTimestamp());
}
void v12counttest::settime_2()
{
    time_t now = time(nullptr);
    v12::CRingPhysicsEventCountItem item(1234, 10, now, 2, 2);
    item.setTimestamp(now+20);
    EQ(now+20, item.getTimestamp());
}

// get count from non body header:

void v12counttest::getcount_1()
{
    v12::CRingPhysicsEventCountItem item(12345, 10, 2);
    EQ(uint64_t(12345), item.getEventCount());
}
void v12counttest::getcount_2()
{
    time_t now = time(nullptr);
    v12::CRingPhysicsEventCountItem item(1234, 10, now, 2, 2);
    EQ(uint64_t(1234), item.getEventCount());
}
void v12counttest::originalsid_1()
{
    v12::CRingPhysicsEventCountItem item(12345, 10, 2);
    EQ(uint32_t(0), item.getOriginalSourceId());
}
void v12counttest::originalsid_2()
{
    time_t now = time(nullptr);
    v12::CRingPhysicsEventCountItem item(1234, 10, now, 2, 2);
    EQ(uint32_t(2), item.getOriginalSourceId());
}
void v12counttest::bsize_1()
{
    v12::CRingPhysicsEventCountItem item(12345, 10, 2);
    EQ(sizeof(v12::PhysicsEventCountItemBody), item.getBodySize());
}
void v12counttest::bsize_2()
{
    time_t now = time(nullptr);
    v12::CRingPhysicsEventCountItem item(1234, 10, now, 2, 2);
    EQ(sizeof(v12::PhysicsEventCountItemBody), item.getBodySize());
}
void v12counttest::bptr_1()
{
    v12::CRingPhysicsEventCountItem item(12345, 10, 2);
    const v12::PhysicsEventCountItemBody* pBody =
        reinterpret_cast<const v12::PhysicsEventCountItemBody*>(item.getBodyPointer());
    const v12::PhysicsEventCountItem* pItem =
        reinterpret_cast<const v12::PhysicsEventCountItem*>(item.getItemPointer());
    EQ(&(pItem->s_body.u_noBodyHeader.s_body), pBody);
}
void v12counttest::bptr_2()
{
    time_t now = time(nullptr);
    v12::CRingPhysicsEventCountItem item(1234, 10, now, 2, 2);
    const v12::PhysicsEventCountItemBody* pBody =
        reinterpret_cast<const v12::PhysicsEventCountItemBody*>(item.getBodyPointer());
    const v12::PhysicsEventCountItem* pItem =
        reinterpret_cast<const v12::PhysicsEventCountItem*>(item.getItemPointer());
    EQ(&(pItem->s_body.u_hasBodyHeader.s_body), pBody);
}
void v12counttest::bptr_3()
{
    v12::CRingPhysicsEventCountItem item(12345, 10, 2);
    v12::PhysicsEventCountItemBody* pBody =
        reinterpret_cast<v12::PhysicsEventCountItemBody*>(item.getBodyPointer());
    v12::PhysicsEventCountItem* pItem =
        reinterpret_cast<v12::PhysicsEventCountItem*>(item.getItemPointer());
    EQ(&(pItem->s_body.u_noBodyHeader.s_body), pBody);
}
void v12counttest::bptr_4()
{
    time_t now = time(nullptr);
    v12::CRingPhysicsEventCountItem item(1234, 10, now, 2, 2);
    v12::PhysicsEventCountItemBody* pBody =
        reinterpret_cast<v12::PhysicsEventCountItemBody*>(item.getBodyPointer());
    v12::PhysicsEventCountItem* pItem =
        reinterpret_cast<v12::PhysicsEventCountItem*>(item.getItemPointer());
    EQ(&(pItem->s_body.u_hasBodyHeader.s_body), pBody);
}
void v12counttest::hasbhdr_1()
{
    v12::CRingPhysicsEventCountItem item(12345, 10, 2);
    ASSERT(!item.hasBodyHeader());
}
void v12counttest::hasbhdr_2()
{
    time_t now = time(nullptr);
    v12::CRingPhysicsEventCountItem item(1234, 10, now, 2, 2);
    ASSERT(item.hasBodyHeader());
}
void v12counttest::ets_1()
{
    v12::CRingPhysicsEventCountItem item(12345, 10, 2);
    CPPUNIT_ASSERT_THROW(
        item.getEventTimestamp(),
        std::logic_error
    );
}
void v12counttest::ets_2()
{
    time_t now = time(nullptr);
    v12::CRingPhysicsEventCountItem item(1234, 10, now, 2, 2);
    uint64_t ts;
    CPPUNIT_ASSERT_NO_THROW(
        ts = item.getEventTimestamp()
    );
    EQ(uint64_t(0xffffffffffffffff), ts);
}
void v12counttest::sid_1()
{
    v12::CRingPhysicsEventCountItem item(12345, 10, 2);
    CPPUNIT_ASSERT_THROW(
        item.getSourceId(),
        std::logic_error
    );
}
void v12counttest::sid_2()
{
    time_t now = time(nullptr);
    v12::CRingPhysicsEventCountItem item(1234, 10, now, 2, 3);
    uint32_t id;
    CPPUNIT_ASSERT_NO_THROW(
        id = item.getSourceId()
    );
    EQ(uint32_t(2), id);
}

void v12counttest::bar_1()
{
    v12::CRingPhysicsEventCountItem item(12345, 10, 2);
    CPPUNIT_ASSERT_THROW(
        item.getBarrierType(),
        std::logic_error
    );
}
void v12counttest::bar_2()
{
    time_t now = time(nullptr);
    v12::CRingPhysicsEventCountItem item(1234, 10, now, 2, 3);
    uint32_t id;
    CPPUNIT_ASSERT_NO_THROW(
        id = item.getBarrierType()
    );
    EQ(uint32_t(0), id);
}
void v12counttest::setbhdr_1()
{
    v12::CRingPhysicsEventCountItem item(12345, 10, 2);
    size_t before = item.size();
    item.setBodyHeader(0x1234567890, 2, 0);
    size_t after = item.size();
    EQ(before + sizeof(v12::BodyHeader) - sizeof(uint32_t), after);
    EQ(uint64_t(12345), item.getEventCount());
    ASSERT(item.hasBodyHeader());
    EQ(uint64_t(0x1234567890), item.getEventTimestamp());
    EQ(uint32_t(2), item.getSourceId());
    EQ(uint32_t(0), item.getBarrierType());
}
void v12counttest::setbhdr_2()
{
    time_t now = time(nullptr);
    v12::CRingPhysicsEventCountItem item(1234, 10, now, 2, 3);
    size_t before = item.size();
    item.setBodyHeader(0x1234567890, 2, 1);
    size_t after = item.size();
    EQ(before, after);
    
    ASSERT(item.hasBodyHeader());
    EQ(uint64_t(0x1234567890), item.getEventTimestamp());
    EQ(uint32_t(2), item.getSourceId());
    EQ(uint32_t(1), item.getBarrierType());
}
void v12counttest::getbhdr_1()
{
    v12::CRingPhysicsEventCountItem item(12345, 10, 2);
    ASSERT(nullptr == item.getBodyHeader());
}
void v12counttest::getbhdr_2()
{
    time_t now = time(nullptr);
    v12::CRingPhysicsEventCountItem item(1234, 10, now, 2, 3);
    const v12::BodyHeader* pHdr =
        reinterpret_cast<v12::BodyHeader*>(item.getBodyHeader());
    ASSERT(pHdr);
    const v12::PhysicsEventCountItem* pItem =
        reinterpret_cast<v12::PhysicsEventCountItem*>(item.getItemPointer());
    EQ(&(pItem->s_body.u_hasBodyHeader.s_bodyHeader), pHdr);
}