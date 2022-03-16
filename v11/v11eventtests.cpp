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

#include <stdexcept>

class v11eventtest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v11eventtest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    CPPUNIT_TEST(construct_3);
    CPPUNIT_TEST(construct_4);
    
    CPPUNIT_TEST(bodysize_1);
    CPPUNIT_TEST(bodysize_2);
    
    CPPUNIT_TEST(bodypointer_1);
    CPPUNIT_TEST(bodypointer_2);
    CPPUNIT_TEST(bodypointer_3);
    CPPUNIT_TEST(bodypointer_4);
    
    CPPUNIT_TEST(hasbodyhdr_1);
    CPPUNIT_TEST(hasbodyhdr_2);
    
    CPPUNIT_TEST(tstamp_1);
    CPPUNIT_TEST(tstamp_2);
    
    CPPUNIT_TEST(sid_1);
    CPPUNIT_TEST(sid_2);
    
    CPPUNIT_TEST(btype_1);
    CPPUNIT_TEST(btype_2);
    
    CPPUNIT_TEST(bheader_1);
    CPPUNIT_TEST(bheader_2);
    CPPUNIT_TEST(bheader_3);
    CPPUNIT_TEST(bheader_4);
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
    
    void bodysize_1();
    void bodysize_2();
    
    void bodypointer_1();
    void bodypointer_2();
    void bodypointer_3();
    void bodypointer_4();
    
    void hasbodyhdr_1();
    void hasbodyhdr_2();
    
    void tstamp_1();
    void tstamp_2();
    
    void sid_1();
    void sid_2();
    
    void btype_1();
    void btype_2();
    
    void bheader_1();
    void bheader_2();
    void bheader_3();
    void bheader_4();
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
void v11eventtest::construct_2()
{
    v11::CPhysicsEventItem item(123456789, 1, 0, 100);
    const v11::PhysicsEventItem* pItem =
        reinterpret_cast<const v11::PhysicsEventItem*>(item.getItemPointer());
    EQ(v11::PHYSICS_EVENT, item.type());
    EQ(sizeof(v11::RingItemHeader) + sizeof(v11::BodyHeader), size_t(item.size()));
    
    const v11::BodyHeader* pH = &pItem->s_body.u_hasBodyHeader.s_bodyHeader;
    EQ(sizeof(v11::BodyHeader), size_t(pH->s_size));
    EQ(uint64_t(123456789), pH->s_timestamp);
    EQ(uint32_t(1), pH->s_sourceId);
    EQ(uint32_t(0), pH->s_barrier);
    
}
// Construct non-body header and fill with data.

void v11eventtest::construct_3()
{
    v11::CPhysicsEventItem item(1000);
    uint16_t* p = reinterpret_cast<uint16_t*>(item.getBodyCursor());
    for (int i =0; i < 100; i++) {
        *p++ = i;
    }
    item.setBodyCursor(p);
    item.updateSize();
    
    const v11::PhysicsEventItem* pItem =
        reinterpret_cast<const v11::PhysicsEventItem*>(item.getItemPointer());
    EQ(v11::PHYSICS_EVENT, pItem->s_header.s_type);
    EQ(sizeof(v11::RingItemHeader) + sizeof(uint32_t) + sizeof(uint16_t)*100,
       size_t(pItem->s_header.s_size)
    );
    EQ(uint32_t(0), pItem->s_body.u_noBodyHeader.s_mbz);
    p = const_cast<uint16_t*>(pItem->s_body.u_noBodyHeader.s_body);
    for (uint16_t i =0; i < 100; i++) {
        EQ(p[i], i);
    }
}
// Construct body header event with data:

void v11eventtest::construct_4()
{
    v11::CPhysicsEventItem item(123456789, 1, 0, 100);
    uint16_t* p = reinterpret_cast<uint16_t*>(item.getBodyCursor());
    for (int i =0; i < 100; i++) {
        *p++ = i;
    }
    item.setBodyCursor(p);
    item.updateSize();
    
    const v11::PhysicsEventItem* pItem =
        reinterpret_cast<const v11::PhysicsEventItem*>(item.getItemPointer());
    EQ(v11::PHYSICS_EVENT, pItem->s_header.s_type);
    EQ(sizeof(v11::RingItemHeader) + sizeof(v11::BodyHeader) + sizeof(uint16_t)*100,
       size_t(pItem->s_header.s_size)
    );
    EQ(sizeof(v11::BodyHeader), size_t(pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_size));
    EQ(uint64_t(123456789), pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_timestamp);
    EQ(uint32_t(1), pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_sourceId);
    EQ(uint32_t(0), pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_barrier);
    p = const_cast<uint16_t*>(pItem->s_body.u_hasBodyHeader.s_body);
    for (uint16_t i = 0; i < 100; i++) {
        EQ(p[i], i);
    }
}
// bodysize of non-body header item:

void v11eventtest::bodysize_1()
{
    v11::CPhysicsEventItem item(1000);
    uint16_t* p = reinterpret_cast<uint16_t*>(item.getBodyCursor());
    for (int i =0; i < 100; i++) {
        *p++ = i;
    }
    item.setBodyCursor(p);
    item.updateSize();
    EQ(100*sizeof(uint16_t), size_t(item.getBodySize()));
}
// Body size of body header item.

void v11eventtest::bodysize_2()
{
    v11::CPhysicsEventItem item(123456789, 1, 0, 100);
    uint16_t* p = reinterpret_cast<uint16_t*>(item.getBodyCursor());
    for (int i =0; i < 100; i++) {
        *p++ = i;
    }
    item.setBodyCursor(p);
    item.updateSize();
    EQ(100*sizeof(uint16_t), size_t(item.getBodySize()));
}
// body pointer for non body header items (not const).

void v11eventtest::bodypointer_1()
{
    v11::CPhysicsEventItem item(1000);
    
    EQ(item.getBodyCursor(), item.getBodyPointer());
    uint16_t* p = reinterpret_cast<uint16_t*>(item.getBodyCursor());
    for (int i =0; i < 100; i++) {
        *p++ = i;
    }
    item.setBodyCursor(p);
    item.updateSize();
    
    p = reinterpret_cast<uint16_t*>(item.getBodyPointer());
    for (uint16_t i = 0; i < 100; i++) {
        EQ(p[i], i);
    }
}
// body pointer for body header items (non const)

void v11eventtest::bodypointer_2()
{
    v11::CPhysicsEventItem item(123456789, 1, 0, 100);
    EQ(item.getBodyCursor(), item.getBodyPointer());
    uint16_t* p = reinterpret_cast<uint16_t*>(item.getBodyCursor());
    for (int i =0; i < 100; i++) {
        *p++ = i;
    }
    item.setBodyCursor(p);
    item.updateSize();
    
    p = reinterpret_cast<uint16_t*>(item.getBodyPointer());
    for (uint16_t i = 0; i < 100; i++) {
        EQ(p[i], i);
    }
}
// same as bodypointyer_1 but const.

void v11eventtest::bodypointer_3()
{
    v11::CPhysicsEventItem item(1000);
    
    
    uint16_t* p = reinterpret_cast<uint16_t*>(item.getBodyCursor());
    const uint16_t* pb = reinterpret_cast<const uint16_t*>(item.getBodyPointer());
    EQ(const_cast<const uint16_t*>(p), pb);
    for (int i =0; i < 100; i++) {
        *p++ = i;
    }
    item.setBodyCursor(p);
    item.updateSize();
    
    pb = reinterpret_cast<const uint16_t*>(item.getBodyPointer());
    for (uint16_t i = 0; i < 100; i++) {
        EQ(pb[i], i);
    }
}
void v11eventtest::bodypointer_4()
{
    v11::CPhysicsEventItem item(123456789, 1, 0, 100);
    
    
    uint16_t* p = reinterpret_cast<uint16_t*>(item.getBodyCursor());
    const uint16_t* pb = reinterpret_cast<const uint16_t*>(item.getBodyPointer());
    EQ(const_cast<const uint16_t*>(p), pb);
    for (int i =0; i < 100; i++) {
        *p++ = i;
    }
    item.setBodyCursor(p);
    item.updateSize();
    
    pb = reinterpret_cast<const uint16_t*>(item.getBodyPointer());
    for (uint16_t i = 0; i < 100; i++) {
        EQ(pb[i], i);
    }
}
// Construction w/o body header gives body header:

void v11eventtest::hasbodyhdr_1()
{
    v11::CPhysicsEventItem item(1000);
    ASSERT(!item.hasBodyHeader());
}
// construction with body header has body header.

void v11eventtest::hasbodyhdr_2()
{
    v11::CPhysicsEventItem item(123456789, 1, 0, 100);
    ASSERT(item.hasBodyHeader());
}
// get timetamp for non body header is a logic error.

void v11eventtest::tstamp_1()
{
    v11::CPhysicsEventItem item(100);
    CPPUNIT_ASSERT_THROW(
        item.getEventTimestamp(),
        std::logic_error
    );
}
// get timestamp for body header is ok.
void v11eventtest::tstamp_2()
{
    v11::CPhysicsEventItem item(123456789, 1, 0, 100);
    uint64_t stamp;
    CPPUNIT_ASSERT_NO_THROW(stamp = item.getEventTimestamp());
    EQ(uint64_t(123456789), stamp);
}
// Non bodyheader item. getSourceId throws.

void v11eventtest::sid_1()
{
    v11::CPhysicsEventItem item(100);
    CPPUNIT_ASSERT_THROW(
        item.getSourceId(),
        std::logic_error
    );
}
// body header getSourceId is normal and correct:

void v11eventtest::sid_2()
{
    v11::CPhysicsEventItem item(123456789, 1, 0, 100);
    uint32_t stamp;
    CPPUNIT_ASSERT_NO_THROW(stamp = item.getSourceId());
    EQ(uint32_t(1), stamp);
}

// barrier type:

void v11eventtest::btype_1()
{
    v11::CPhysicsEventItem item(100);
    CPPUNIT_ASSERT_THROW(
        item.getBarrierType(),
        std::logic_error
    );
}
void v11eventtest::btype_2()
{
    v11::CPhysicsEventItem item(123456789, 1, 0, 100);
    uint32_t stamp;
    CPPUNIT_ASSERT_NO_THROW(stamp = item.getBarrierType());
    EQ(uint32_t(0), stamp);
}
// Get body header from headerless item.

void v11eventtest::bheader_1()
{
    v11::CPhysicsEventItem item( 100);
    ASSERT(item.getBodyHeader() == nullptr);
}
// Body header from header item gives correct pointer.
void v11eventtest::bheader_2()
{
    v11::CPhysicsEventItem item(123456789, 1, 0, 100);
    v11::pBodyHeader pB = reinterpret_cast<v11::pBodyHeader>(item.getBodyHeader());
    v11::pPhysicsEventItem pItem = reinterpret_cast<v11::pPhysicsEventItem>(item.getItemPointer());
    v11::pBodyHeader pExpected = &(pItem->s_body.u_hasBodyHeader.s_bodyHeader);
    EQ(pExpected, pB);
}
// insert body header into filled bodyheader-less item:

void v11eventtest::bheader_3()
{
    v11::CPhysicsEventItem item(100);
    
    
    uint16_t* p = reinterpret_cast<uint16_t*>(item.getBodyCursor());
    
    for (int i =0; i < 100; i++) {
        *p++ = i;
    }
    item.setBodyCursor(p);
    item.updateSize();
    size_t priorSize = item.size();
    
    item.setBodyHeader(100, 1, 0);
    EQ(priorSize + sizeof(v11::BodyHeader) - sizeof(uint32_t), size_t(item.size()));
    
    v11::pBodyHeader pB = reinterpret_cast<v11::pBodyHeader>(item.getBodyHeader());
    EQ(uint32_t(sizeof(v11::BodyHeader)), pB->s_size);
    EQ(uint64_t(100), pB->s_timestamp);
    EQ(uint32_t(1), pB->s_sourceId);
    EQ(uint32_t(0), pB->s_barrier);
    
    p = reinterpret_cast<uint16_t*>(item.getBodyPointer());
    for (uint16_t i =0; i < 100; i++) {
        EQ(p[i], i);
    }
}
// writes in place if there's already one:
void v11eventtest::bheader_4()
{
    v11::CPhysicsEventItem item(123456789, 2, 0, 100);
    
    
    uint16_t* p = reinterpret_cast<uint16_t*>(item.getBodyCursor());
    
    for (int i =0; i < 100; i++) {
        *p++ = i;
    }
    item.setBodyCursor(p);
    item.updateSize();
    size_t prior = item.size();
    
    item.setBodyHeader(100, 1, 0);
    EQ(prior, size_t(item.size()));   // Size stays the same.
    
    v11::pBodyHeader pB = reinterpret_cast<v11::pBodyHeader>(item.getBodyHeader());
    EQ(uint32_t(sizeof(v11::BodyHeader)), pB->s_size);
    EQ(uint64_t(100), pB->s_timestamp);
    EQ(uint32_t(1), pB->s_sourceId);
    EQ(uint32_t(0), pB->s_barrier);
    
    p = reinterpret_cast<uint16_t*>(item.getBodyPointer());
    for (uint16_t i =0; i < 100; i++) {
        EQ(p[i], i);
    }
}