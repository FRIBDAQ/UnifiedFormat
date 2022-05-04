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

/** @file:  v12phystests.cpp    
 *  @brief: Test v12::CPhysicsEventItem.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CPhysicsEventItem.h"
#include "DataFormat.h"
#include <stdexcept>

class v12phystest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v12phystest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    
    CPPUNIT_TEST(hasbodyhdr_1);
    CPPUNIT_TEST(hasbodyhdr_2);
    
    CPPUNIT_TEST(getbodyhdr_1);
    CPPUNIT_TEST(getbodyhdr_2);
    
    CPPUNIT_TEST(setbodyhdr_1);
    CPPUNIT_TEST(setbodyhdr_2);
    CPPUNIT_TEST(setbodyhdr_3);
    
    CPPUNIT_TEST(bsize_1);
    CPPUNIT_TEST(bsize_2);
    
    CPPUNIT_TEST(ts_1);
    CPPUNIT_TEST(ts_2);
    
    CPPUNIT_TEST(sid_1);
    CPPUNIT_TEST(sid_2);
    
    CPPUNIT_TEST(bt_1);
    CPPUNIT_TEST(bt_2);
    
    
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
    
    void hasbodyhdr_1();
    void hasbodyhdr_2();
    
    void getbodyhdr_1();
    void getbodyhdr_2();
    
    void setbodyhdr_1();
    void setbodyhdr_2();
    void setbodyhdr_3();
    
    void bsize_1();
    void bsize_2();
    
    void ts_1();
    void ts_2();
    
    void sid_1();
    void sid_2();
    
    void bt_1();
    void bt_2();
};

CPPUNIT_TEST_SUITE_REGISTRATION(v12phystest);

// no body  header

void v12phystest::construct_1()
{
    v12::CPhysicsEventItem item;
    const v12::PhysicsEventItem* pItem =
        reinterpret_cast<const v12::PhysicsEventItem*>(item.getItemPointer());
        
    EQ(v12::PHYSICS_EVENT, pItem->s_header.s_type);
    EQ(sizeof(v12::RingItemHeader) + sizeof(uint32_t), size_t(pItem->s_header.s_size));
    EQ(sizeof(uint32_t), size_t(pItem->s_body.u_noBodyHeader.s_empty));
}

// body header:
void v12phystest::construct_2()
{
    
    v12::CPhysicsEventItem item(0x1234567890, 2, 3);
    
    const v12::PhysicsEventItem* pItem =
        reinterpret_cast<const v12::PhysicsEventItem*>(item.getItemPointer());
        
    EQ(v12::PHYSICS_EVENT, pItem->s_header.s_type);
    EQ(sizeof(v12::RingItemHeader) + sizeof(v12::BodyHeader), size_t(pItem->s_header.s_size));
    
    const v12::BodyHeader* pBh = &(pItem->s_body.u_hasBodyHeader.s_bodyHeader);
    EQ(sizeof(v12::BodyHeader), size_t(pBh->s_size));
    EQ(uint64_t(0x1234567890), pBh->s_timestamp);
    EQ(uint32_t(2), pBh->s_sourceId);
    EQ(uint32_t(3), pBh->s_barrier);
    
}

// no body header:

void v12phystest::hasbodyhdr_1()
{
    v12::CPhysicsEventItem item;
    ASSERT(!item.hasBodyHeader());
}
// has body header:
void v12phystest::hasbodyhdr_2()
{
     v12::CPhysicsEventItem item(0x1234567890, 2, 3);
     ASSERT(item.hasBodyHeader());
}

// no bodyhdr -> nullptr.

void v12phystest::getbodyhdr_1()
{
    v12::CPhysicsEventItem item;
    ASSERT(nullptr == item.getBodyHeader());
}
// body header pointer  -> s_body.u_hasBodyHeader.s_bodyHeader:

void v12phystest::getbodyhdr_2()
{
    v12::CPhysicsEventItem item(0x1234567890, 2, 3);
    
    const void* p = item.getBodyHeader();
    ASSERT(p);
    const v12::BodyHeader* pB = reinterpret_cast<const v12::BodyHeader*>(p);
    const v12::PhysicsEventItem* pItem =
        reinterpret_cast<const v12::PhysicsEventItem*>(item.getItemPointer());
    EQ(&(pItem->s_body.u_hasBodyHeader.s_bodyHeader), pB);
}
// no body header inserts one:

void v12phystest::setbodyhdr_1()
{
    v12::CPhysicsEventItem item;
    size_t prior = item.size();
    item.setBodyHeader(0x9876543210, 1, 2);
    size_t now = item.size();
    EQ(sizeof(v12::BodyHeader) - sizeof(uint32_t), now - prior);
    
    const v12::BodyHeader* pBh =
        reinterpret_cast<const v12::BodyHeader*>(item.getBodyHeader());
    ASSERT(pBh);
    EQ(sizeof(v12::BodyHeader), size_t(pBh->s_size));
    EQ(uint64_t(0x9876543210), pBh->s_timestamp);
    EQ(uint32_t(1), pBh->s_sourceId);
    EQ(uint32_t(2), pBh->s_barrier);
}

// no body header insertio8n preserves body contents.

void v12phystest::setbodyhdr_2()
{
    v12::CPhysicsEventItem item;
    
    // add a body:
    
    uint16_t* pBody = reinterpret_cast<uint16_t*>(item.getBodyCursor());
    for (int i =0;  i < 100; i++) {
        *pBody++ = i;
    }
    item.setBodyCursor(pBody);
    item.updateSize();
    
    item.setBodyHeader(0x9876543210, 1, 2);
    const v12::BodyHeader* pBh =
        reinterpret_cast<const v12::BodyHeader*>(item.getBodyHeader());
    ASSERT(pBh);
    EQ(sizeof(v12::BodyHeader), size_t(pBh->s_size));
    EQ(uint64_t(0x9876543210), pBh->s_timestamp);
    EQ(uint32_t(1), pBh->s_sourceId);
    EQ(uint32_t(2), pBh->s_barrier);
    
    const v12::PhysicsEventItem* pItem =
        reinterpret_cast<const v12::PhysicsEventItem*>(item.getItemPointer());
    pBody = reinterpret_cast<uint16_t*>(item.getBodyPointer());
    EQ((void*)(&pItem->s_body.u_hasBodyHeader.s_body[0]), (void*)pBody);
    for (uint16_t i =0; i < 100; i++) {
        EQ(*pBody, i);
        pBody++;
    }
}

// has body header just replaces.
void v12phystest::setbodyhdr_3()
{
    v12::CPhysicsEventItem item(0x1234567890, 1, 2);
    size_t before = item.size();
    item.setBodyHeader(0x9876543210, 2, 1);
    size_t after = item.size();
    EQ(before, after);
    const v12::BodyHeader* pBh =
        reinterpret_cast<const v12::BodyHeader*>(item.getBodyHeader());
    ASSERT(pBh);
    EQ(sizeof(v12::BodyHeader), size_t(pBh->s_size));
    EQ(uint64_t(0x9876543210), pBh->s_timestamp);
    EQ(uint32_t(2), pBh->s_sourceId);
    EQ(uint32_t(1), pBh->s_barrier);
    
}
// Bbody size when there's no body header:
void v12phystest::bsize_1()
{
    v12::CPhysicsEventItem item;
    uint8_t* p = reinterpret_cast<uint8_t*>(item.getBodyCursor());
    p += 100;
    item.setBodyCursor(p);
    item.updateSize();
    EQ(size_t(100), item.getBodySize());
}
// body size when there is a body header:

void v12phystest::bsize_2()
{
    v12::CPhysicsEventItem item(0x124, 1, 2);
    
    uint8_t* p = reinterpret_cast<uint8_t*>(item.getBodyCursor());
    p += 100;
    
    item.setBodyCursor(p);
    item.updateSize();
    EQ(size_t(100), item.getBodySize());
}
// ts - fails with no body header:

void v12phystest::ts_1()
{
    v12::CPhysicsEventItem item;
    CPPUNIT_ASSERT_THROW(
        item.getEventTimestamp(),
        std::logic_error
    );
}
// get ts works with body header.
void v12phystest::ts_2()
{
    v12::CPhysicsEventItem item(0x1234567890, 1, 2);
    uint64_t ts;
    CPPUNIT_ASSERT_NO_THROW(
        ts = item.getEventTimestamp()
    );
    EQ(uint64_t(0x1234567890), ts);
    
}
// no body header getSourceId throws/

void v12phystest::sid_1()
{
    v12::CPhysicsEventItem item;
    CPPUNIT_ASSERT_THROW(
        item.getSourceId(),
        std::logic_error
    );
}
// Body header sid is returned:
void v12phystest::sid_2()
{
    v12::CPhysicsEventItem item(0x1234567890, 1, 2);
    uint32_t sid;
    CPPUNIT_ASSERT_NO_THROW(
        sid = item.getSourceId()
    );
    EQ(uint32_t(1), sid);
    
}
// getBarriertype for no body header throws:

void v12phystest::bt_1()
{
    v12::CPhysicsEventItem item;
    CPPUNIT_ASSERT_THROW(
        item.getBarrierType(),
        std::logic_error
    );
}
// getBarrierType with body header returns it:

void v12phystest::bt_2()
{
    v12::CPhysicsEventItem item(0x1234567890, 1, 2);
    uint32_t bt;
    CPPUNIT_ASSERT_NO_THROW(
        bt = item.getBarrierType()
    );
    EQ(uint32_t(2), bt);
}

