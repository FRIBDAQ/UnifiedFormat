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

/** @file:  v12factorytests.cpp
 *  @brief: Tests for the V12 object factory.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <memory>
#include "DataFormat.h"
#include "RingItemFactory.h"
#include "CRingItem.h"


// In our tests, we use std::unique_ptr to ensure there's not
// any memory leaking.


class v12facttest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v12facttest);
    CPPUNIT_TEST(mkringitem_1);
    CPPUNIT_TEST(mkringitem_2);
    CPPUNIT_TEST(mkringitem_3);
    CPPUNIT_TEST(mkringitem_4);
    CPPUNIT_TEST_SUITE_END();
    
private:
    v12::RingItemFactory* m_pFactory;
public:
    void setUp() {
        m_pFactory = new v12::RingItemFactory;
    }
    void tearDown() {
        delete m_pFactory;
    }
protected:
    void mkringitem_1();
    void mkringitem_2();
    void mkringitem_3();
    void mkringitem_4();
};

CPPUNIT_TEST_SUITE_REGISTRATION(v12facttest);

// Empty ring item with capacity:

void v12facttest::mkringitem_1()
{
    std::unique_ptr<::CRingItem> p(m_pFactory->makeRingItem(v12::PHYSICS_EVENT, 100));
    EQ(uint32_t(v12::PHYSICS_EVENT), p->type());
    EQ(sizeof(v12::RingItemHeader) + sizeof(uint32_t), size_t(p->size()));
    
    const v12::RingItem* pItem =
        reinterpret_cast<const v12::RingItem*>(p->getItemPointer());
    EQ(
       sizeof(v12::RingItemHeader) + sizeof(uint32_t),
       size_t(pItem->s_header.s_size)
    );
    EQ(v12::PHYSICS_EVENT, pItem->s_header.s_type);
    EQ(sizeof(uint32_t), size_t(pItem->s_body.u_noBodyHeader.s_empty));
}
// Ring item with body header:

void v12facttest::mkringitem_2()
{
    std::unique_ptr<::CRingItem> p(
        m_pFactory->makeRingItem(v12::PHYSICS_EVENT, 0x1234567890, 2, 100, 1)
    );
    EQ(uint32_t(v12::PHYSICS_EVENT), p->type());
    EQ(sizeof(v12::RingItemHeader) + sizeof(v12::BodyHeader), size_t(p->size()));
    ASSERT(p->hasBodyHeader());
    EQ(uint64_t(0x1234567890), p->getEventTimestamp());
    EQ(uint32_t(2), p->getSourceId());
    EQ(uint32_t(1), p->getBarrierType());
    CPPUNIT_ASSERT_NO_THROW(
        v12::CRingItem* p12 = dynamic_cast<v12::CRingItem*>(p.get())
    );
    
}
// Make ring item from another item. (no body header).

void v12facttest::mkringitem_3()
{
    std::unique_ptr<::CRingItem> src(m_pFactory->makeRingItem(v12::PHYSICS_EVENT, 100));
    uint8_t* pBody = reinterpret_cast<uint8_t*>(src->getBodyPointer());
    for(int i =0; i < 10; i++) {
        *pBody++ = i;
    }
    src->setBodyCursor(pBody);
    src->updateSize();
    size_t srcSize = src->size();
    
    std::unique_ptr<::CRingItem> cpy(m_pFactory->makeRingItem(*src));
    EQ(srcSize, size_t(cpy->size()));
    EQ(src->type(), cpy->type());
    ASSERT(!cpy->hasBodyHeader());
    
    pBody = reinterpret_cast<uint8_t*>(src->getBodyPointer());
    uint8_t* p = reinterpret_cast<uint8_t*>(cpy->getBodyPointer());
    for (int i =0; i < 10; i++) {
        EQ(*pBody, *p);
        p++; pBody++;
    }
    
    
}
// Copy an item with a body header:
void v12facttest::mkringitem_4()
{
    std::unique_ptr<::CRingItem> src(
        m_pFactory->makeRingItem(
            v12::PHYSICS_EVENT, 0x1234567890, 1, 100, 2
        )
    );
    uint8_t* pBody = reinterpret_cast<uint8_t*>(src->getBodyPointer());
    for(int i =0; i < 10; i++) {
        *pBody++ = i;
    }
    src->setBodyCursor(pBody);
    src->updateSize();
    size_t srcSize = src->size();
    
    std::unique_ptr<::CRingItem> cpy(m_pFactory->makeRingItem(*src));
    EQ(srcSize, size_t(cpy->size()));
    EQ(src->type(), cpy->type());
    ASSERT(cpy->hasBodyHeader());
    
    EQ(src->getEventTimestamp(), cpy->getEventTimestamp());
    EQ(src->getSourceId(), cpy->getSourceId());
    EQ(src->getBarrierType(), cpy->getBarrierType());
    
    pBody = reinterpret_cast<uint8_t*>(src->getBodyPointer());
    uint8_t* p = reinterpret_cast<uint8_t*>(cpy->getBodyPointer());
    for (int i =0; i < 10; i++) {
        EQ(*pBody, *p);
        p++; pBody++;
    }
    
    
}