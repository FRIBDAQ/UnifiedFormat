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

/** @file:  v11factoryTests.cpp
 *  @brief: Test for the V11 ring item factory.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "RingItemFactory.h"
#include "DataFormat.h"

#include <CRingItem.h>
#include <string.h>

/*
 * Note in the implementation
 * the try/catch blocks use that assertion failures are signaled
 * via exception throws the catch delete rethrow pattern
 * ensures there will be no memory leaks in the presence of
 * test failures.
 */

class v11facttest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v11facttest);
    CPPUNIT_TEST(base_1);
    CPPUNIT_TEST(base_2);
    CPPUNIT_TEST(base_3);
    CPPUNIT_TEST(base_4);
    CPPUNIT_TEST_SUITE_END();
    
private:
    v11::RingItemFactory* m_pFactory;
public:
    void setUp() {
        m_pFactory = new v11::RingItemFactory;
    }
    void tearDown() {
        delete m_pFactory;
    }
protected:
    void base_1();
    void base_2();
    void base_3();
    void base_4();
};

CPPUNIT_TEST_SUITE_REGISTRATION(v11facttest);

// Make a ring item without a body header.
void v11facttest::base_1()
{
    ::CRingItem* pItem = m_pFactory->makeRingItem(v11::PHYSICS_EVENT, 100);
    try {
        const v11::RingItem* p =
            reinterpret_cast<const v11::RingItem*>(pItem->getItemPointer());
        EQ(sizeof(v11::RingItemHeader) + sizeof(uint32_t), size_t(p->s_header.s_size));
        EQ(v11::PHYSICS_EVENT, p->s_header.s_type);
        EQ(uint32_t(0), p->s_body.u_noBodyHeader.s_mbz);
        
        // Ensure that body cursor and size() are consistent with
        // contents of the ring item:
        
        EQ(pItem->size(), (p->s_header.s_size));
        const uint8_t* beg = reinterpret_cast<const uint8_t*>(p);
        const uint8_t* end = reinterpret_cast<const uint8_t*>(pItem->getBodyCursor());
        EQ(ptrdiff_t(pItem->size()), end - beg);
    }
    catch (...) {
        delete pItem;
        throw;
    }
    delete pItem;
}
// Make a ring item with a body header:
void v11facttest::base_2()
{
    ::CRingItem* pItem = m_pFactory->makeRingItem(
        v11::PHYSICS_EVENT, 0x1234567890, 2, 100, 1
    );
    try {
        const v11::RingItem* p =
            reinterpret_cast<const v11::RingItem*>(pItem->getItemPointer());
       EQ(
            sizeof(v11::RingItemHeader) + sizeof(v11::BodyHeader),
            size_t(p->s_header.s_size)
        );
        EQ(v11::PHYSICS_EVENT, p->s_header.s_type);
        EQ(
           sizeof(v11::BodyHeader),
           size_t(p->s_body.u_hasBodyHeader.s_bodyHeader.s_size)
        );
        EQ(
            uint64_t(0x1234567890),
            p->s_body.u_hasBodyHeader.s_bodyHeader.s_timestamp
        );
        EQ(
            uint32_t(2),
            p->s_body.u_hasBodyHeader.s_bodyHeader.s_sourceId
        );
        EQ(
            uint32_t(1),
            p->s_body.u_hasBodyHeader.s_bodyHeader.s_barrier
        );
        
        EQ(pItem->size(), (p->s_header.s_size));
        const uint8_t* beg = reinterpret_cast<const uint8_t*>(p);
        const uint8_t* end = reinterpret_cast<const uint8_t*>(pItem->getBodyCursor());
        EQ(ptrdiff_t(pItem->size()), end - beg);
    }
    catch (...) {
        delete pItem;
        throw;
    }
    delete pItem;
}
// Make duplicate from a base class item:
void v11facttest::base_3()
{
    ::CRingItem* pItem1 = m_pFactory->makeRingItem(
        v11::PHYSICS_EVENT, 100
    );
    uint16_t* p = reinterpret_cast<uint16_t*>(pItem1->getBodyCursor());
    for (int i =0; i < 10; i++) {
        *p++ = i;
    }
    pItem1->setBodyCursor(p);
    pItem1->updateSize();
    
    ::CRingItem* pItem2= m_pFactory->makeRingItem(*pItem1);
    
    try {
        const v11::RingItem* p1 =
            reinterpret_cast<v11::RingItem*>(pItem1->getItemPointer());
        const v11::RingItem* p2 =
            reinterpret_cast<v11::RingItem*>(pItem2->getItemPointer());
        EQ(p1->s_header.s_size, p2->s_header.s_size);
        EQ(0, memcmp(p1, p2, p1->s_header.s_size));
        
        EQ(pItem2->size(), (p2->s_header.s_size));
        const uint8_t* beg = reinterpret_cast<const uint8_t*>(p2);
        const uint8_t* end = reinterpret_cast<const uint8_t*>(pItem2->getBodyCursor());
        EQ(ptrdiff_t(pItem2->size()), end - beg);
    }
    catch (...) {
        delete pItem1;
        delete pItem2;
        
        throw;
    }
    delete pItem1;
    delete pItem2;
}
// DUplicate from raw

void v11facttest::base_4()
{}