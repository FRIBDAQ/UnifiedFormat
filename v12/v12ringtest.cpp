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

/** @file:  v12ringtest.cpp
 *  @brief: Test v12::CRingITem base class.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CRingItem.h"
#include "DataFormat.h"
#include <stdexcept>

class v12ringtest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v12ringtest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    
    CPPUNIT_TEST(bodysize_1);
    CPPUNIT_TEST(bodysize_2);
    CPPUNIT_TEST(bodysize_3);
    CPPUNIT_TEST(bodysize_4);
    
    CPPUNIT_TEST(bodyptr_1);
    CPPUNIT_TEST(bodyptr_2);
    CPPUNIT_TEST(bodyptr_3);
    CPPUNIT_TEST(bodyptr_4);
    
    CPPUNIT_TEST(swap);
    
    CPPUNIT_TEST(hasbodyhdr_1);
    CPPUNIT_TEST(hasbodyhdr_2);
    
    CPPUNIT_TEST(getbodyhdr_1);
    CPPUNIT_TEST(getbodyhdr_2);
    
    CPPUNIT_TEST(ts_1);
    CPPUNIT_TEST(ts_2);
    
    CPPUNIT_TEST(src_1);
    CPPUNIT_TEST(src_2);
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
    
    void bodysize_1();
    void bodysize_2();
    void bodysize_3();
    void bodysize_4();
    
    void bodyptr_1();
    void bodyptr_2();
    void bodyptr_3();
    void bodyptr_4();
    
    void swap();
    
    void hasbodyhdr_1();
    void hasbodyhdr_2();
    
    void getbodyhdr_1();
    void getbodyhdr_2();
    
    void ts_1();
    void ts_2();
    
    void src_1();
    void src_2();
};

CPPUNIT_TEST_SUITE_REGISTRATION(v12ringtest);

// non body header construction

void v12ringtest::construct_1()
{
    v12::CRingItem item(v12::PHYSICS_EVENT, 100);
    EQ(v12::PHYSICS_EVENT, item.type());
    EQ(sizeof(v12::RingItemHeader) + sizeof(uint32_t),
       size_t(item.size()));
    const v12::RingItem* pItem =
        reinterpret_cast<const v12::RingItem*>(item.getItemPointer());
    EQ(v12::PHYSICS_EVENT, pItem->s_header.s_type);
    EQ(sizeof(v12::RingItemHeader) + sizeof(uint32_t),
       size_t(pItem->s_header.s_size)
    );
    EQ(uint32_t(sizeof(uint32_t)), pItem->s_body.u_noBodyHeader.s_empty);
    const uint8_t* pbody =
        reinterpret_cast<const uint8_t*>(item.getBodyCursor());
    EQ(reinterpret_cast<const uint8_t*>(pItem->s_body.u_noBodyHeader.s_body), pbody);
}
// Body header construction.
void v12ringtest::construct_2()
{
    v12::CRingItem item(
        v12::PHYSICS_EVENT,
        0x1234567890, 1, 2, 100
    );
    EQ(v12::PHYSICS_EVENT, item.type());
    EQ(
        sizeof(v12::RingItemHeader) + sizeof(v12::BodyHeader),
        size_t(item.size())
    );
    const v12::RingItem* pItem =
        reinterpret_cast<const v12::RingItem*>(item.getItemPointer());
    EQ(v12::PHYSICS_EVENT, pItem->s_header.s_type);
    EQ(
        sizeof(v12::RingItemHeader) + sizeof(v12::BodyHeader),
        size_t(pItem->s_header.s_size)
    );
    EQ(
       sizeof(v12::BodyHeader),
       size_t(pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_size)
    );
    EQ(uint64_t(0x1234567890),
       pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_timestamp
    );
    EQ(uint32_t(1),
       pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_sourceId
    );
    EQ(uint32_t(2),
       pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_barrier
    );
    const uint8_t* pbody =
        reinterpret_cast<const uint8_t*>(item.getBodyCursor());
    EQ(reinterpret_cast<const uint8_t*>(pItem->s_body.u_hasBodyHeader.s_body), pbody);

}
// body size for zero length no body header:

void v12ringtest::bodysize_1()
{
    v12::CRingItem item(v12::PHYSICS_EVENT);
    EQ(size_t(0), item.getBodySize());
}
// non zero body length no body header.

void v12ringtest::bodysize_2()
{
    v12::CRingItem item(v12::PHYSICS_EVENT);
    uint8_t* p = reinterpret_cast<uint8_t*>(item.getBodyCursor());
    p += 100;                // 100 bytes of body.
    item.setBodyCursor(p);
    item.updateSize();
    EQ(size_t(100), item.getBodySize());
}
// body size zero length body header.
void v12ringtest::bodysize_3()
{
    v12::CRingItem item(
        v12::PHYSICS_EVENT,
        0x1234567890, 1, 2, 100
    );
    EQ(size_t(0), item.getBodySize());
}
// non zeor length body with body header.

void v12ringtest::bodysize_4()
{
    v12::CRingItem item(
        v12::PHYSICS_EVENT,
        0x1234567890, 1, 2, 100
    );
    uint8_t* p = reinterpret_cast<uint8_t*>(item.getBodyCursor());
    p += 100;                // 100 bytes of body.
    item.setBodyCursor(p);
    item.updateSize();
    EQ(size_t(100), item.getBodySize());
}
// body pointer with no body header non const

void v12ringtest::bodyptr_1()
{
    v12::CRingItem item(v12::PHYSICS_EVENT);
    void* pBody = item.getBodyPointer();
    v12::pRingItem pItem = reinterpret_cast<v12::pRingItem>(item.getItemPointer());
    EQ(
        reinterpret_cast<uint8_t*>(pItem->s_body.u_noBodyHeader.s_body),
        reinterpret_cast<uint8_t*>(pBody)
    );
        
}
// const body pointer with no body header.

void v12ringtest::bodyptr_2()
{
    v12::CRingItem item(v12::PHYSICS_EVENT);
    const void* pBody = item.getBodyPointer();
    const v12::RingItem* pItem =
        reinterpret_cast<const v12::RingItem*>(item.getItemPointer());
    EQ(
        reinterpret_cast<const uint8_t*>(pItem->s_body.u_noBodyHeader.s_body),
        reinterpret_cast<const uint8_t*>(pBody)
    );
                
}
// non const body pointer with body header:

void v12ringtest::bodyptr_3()
{
    v12::CRingItem item(
        v12::PHYSICS_EVENT, 0x1234567890, 1, 2    
    );
    void* pBody = item.getBodyPointer();
    v12::pRingItem pItem = reinterpret_cast<v12::pRingItem>(item.getItemPointer());
    EQ(
        reinterpret_cast<uint8_t*>(pItem->s_body.u_hasBodyHeader.s_body),
        reinterpret_cast<uint8_t*>(pBody)
    );
}
// const body pointer with body header:
void v12ringtest::bodyptr_4()
{
    v12::CRingItem item(
        v12::PHYSICS_EVENT, 0x1234567890, 1, 2    
    );
    const void* pBody = item.getBodyPointer();
    const v12::RingItem* pItem =
        reinterpret_cast<const v12::RingItem*>(item.getItemPointer());
    EQ(
        reinterpret_cast<const uint8_t*>(pItem->s_body.u_hasBodyHeader.s_body),
        reinterpret_cast<const uint8_t*>(pBody)
    );
}
// bit of white box we know it returns a const just make sure it's
// the right one:
void v12ringtest::swap()
{
    v12::CRingItem item(v12::PHYSICS_EVENT);
    ASSERT(!item.mustSwap());
}
// has no body header:
void v12ringtest::hasbodyhdr_1()
{
    v12::CRingItem item(v12::PHYSICS_EVENT);
    ASSERT(!item.hasBodyHeader());
}
// has body header
void v12ringtest::hasbodyhdr_2()
{
    v12::CRingItem item(v12::PHYSICS_EVENT,
        0x1234567890, 2, 1
    );
    ASSERT(item.hasBodyHeader());
}
// get body header from non body header:

void v12ringtest::getbodyhdr_1()
{
    v12::CRingItem item(v12::PHYSICS_EVENT);
    ASSERT(item.getBodyHeader() == nullptr);
}
void v12ringtest::getbodyhdr_2()
{
    v12::CRingItem item(
        v12::PHYSICS_EVENT<
        0x1234567890, 1, 2
    );
    void* pBH = item.getBodyHeader();
    ASSERT(pBH);
    v12::pRingItem pItem = reinterpret_cast<v12::pRingItem>(item.getItemPointer());
    EQ(
        reinterpret_cast<v12::pBodyHeader>(&pItem->s_body.u_hasBodyHeader.s_bodyHeader),
        reinterpret_cast<v12::pBodyHeader>(pBH)
    );
}
// timestamp no body header is logic error:

void v12ringtest::ts_1()
{
    v12::CRingItem item(v12::PHYSICS_EVENT);
    CPPUNIT_ASSERT_THROW(
        item.getEventTimestamp(),
        std::logic_error
    );
}
// Timestamp with body header:

void v12ringtest::ts_2()
{
    v12::CRingItem item(
        v12::PHYSICS_EVENT, 0x1234567890, 1, 2
    );
    uint64_t ts;
    CPPUNIT_ASSERT_NO_THROW(
        ts = item.getEventTimestamp();
    );
    EQ(uint64_t(0x1234567890), ts);
}
// sourceid with no body header is a logic error:

void v12ringtest::src_1()
{
    v12::CRingItem item(v12::PHYSICS_EVENT);
    CPPUNIT_ASSERT_THROW(
        item.getSourceId(),
        std::logic_error
    );
}
// sourceid with body header gets it:

void v12ringtest::src_2()
{
    v12::CRingItem item(
        v12::PHYSICS_EVENT, 0x1234567890, 1, 2
    );
    uint32_t src;
    CPPUNIT_ASSERT_NO_THROW(
        src = item.getSourceId()
    );
    EQ(uint32_t(1), src);
}