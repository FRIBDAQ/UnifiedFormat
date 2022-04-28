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

class v12ringtest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v12ringtest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
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