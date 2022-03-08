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

/** @file:  v10factorytests.cpp
 *  @brief: Test the V10 ring item factory.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "RingItemFactory.h"
#include "DataFormat.h"
#include <CRingBuffer.h>
#include "CRingItem.h"
#include <string.h>



class v10factorytest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v10factorytest);
    CPPUNIT_TEST(ring_1);
    CPPUNIT_TEST(ring_2);
    CPPUNIT_TEST(ring_3);
    CPPUNIT_TEST_SUITE_END();
    
private:
    v10::RingItemFactory* m_pFactory;
public:
    void setUp() {
        m_pFactory = new v10::RingItemFactory;
    }
    void tearDown() {
        delete m_pFactory;
    }
protected:
    void ring_1();
    void ring_2();
    void ring_3();
};

CPPUNIT_TEST_SUITE_REGISTRATION(v10factorytest);

// 'normal' CRingItem creation.

void v10factorytest::ring_1()
{
    CRingItem* item(0);
    try {
        item = m_pFactory->makeRingItem(v10::PHYSICS_EVENT, 100);
        EQ(v10::PHYSICS_EVENT, item->type());
        EQ(sizeof(v10::RingItemHeader), size_t(item->size()));
        EQ(size_t(100), item->getStorageSize());
    } catch (...) {
        delete item;
        throw;
    }
    delete item;
}
// advanced CRingItem create looks like normal.
void
v10factorytest::ring_2()
{
CRingItem* item(0);
    try {
        item = m_pFactory->makeRingItem(
            v10::PHYSICS_EVENT, uint64_t(1234675890), 0, 100
        );
        EQ(v10::PHYSICS_EVENT, item->type());
        EQ(sizeof(v10::RingItemHeader), size_t(item->size()));
        EQ(size_t(100), item->getStorageSize());
    } catch (...) {
        delete item;
        throw;
    }
    delete item;    
}
// Make a ring item from an existing CRingItem object:

void
v10factorytest::ring_3()
{
    v10::CRingItem src(PHYSICS_EVENT, 100);
    uint16_t* p = reinterpret_cast<uint16_t*>(src.getBodyCursor());
    for (int i =0; i < 10; i++) {
        *p++ = i;
    }
    src.setBodyCursor(p); src.updateSize();
    
    
    ::CRingItem* pItem(0);
    try {
        pItem = m_pFactory->makeRingItem(src);
        v10::pRingItemHeader p1 = reinterpret_cast<v10::pRingItemHeader>(src.getItemPointer());
        ::pRingItemHeader p2 = reinterpret_cast<::pRingItemHeader>(pItem->getItemPointer());
        
        EQ(p1->s_size, p2->s_size);
        EQ(0, memcmp(p1, p2, p1->s_size));
    }
    catch (...) {
        delete pItem;
        throw;
    }
    
    delete pItem;
}