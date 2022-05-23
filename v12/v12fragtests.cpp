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

/** @file:  v12fragtests.cpp
 *  @brief: Tests of v12::CRingFragmentItem and v12::CUnknownFragment
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CRingFragmentItem.h"
#include "CUnknownFragment.h"
#include "DataFormat.h"
#include <string.h>

class v12fragtest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v12fragtest);
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

CPPUNIT_TEST_SUITE_REGISTRATION(v12fragtest);

// empty item.
void v12fragtest::construct_1()
{
    v12::CRingFragmentItem item(0x1234567890, 2, 0, nullptr, 1);
    v12::pEventBuilderFragment pItem =
        reinterpret_cast<v12::pEventBuilderFragment>(item.getItemPointer());
    EQ(v12::EVB_FRAGMENT, pItem->s_header.s_type);
    EQ(sizeof(v12::EventBuilderFragment), size_t(pItem->s_header.s_size));
    EQ(uint64_t(0x1234567890), pItem->s_bodyHeader.s_timestamp);
    EQ(uint32_t(2), pItem->s_bodyHeader.s_sourceId);
    EQ(uint32_t(1), pItem->s_bodyHeader.s_barrier);
}
// data in item.

void v12fragtest::construct_2()
{
    uint8_t payload[100];
    for (int i =0; i < sizeof(payload); i++) {
        payload[i] = i;
    }
    v12::CRingFragmentItem item(0x1234568790, 2, sizeof(payload), payload, 1);
    
    // from construct_1 we can assume we only need to check the item size and
    // payload contents.
    v12::pEventBuilderFragment pItem =
        reinterpret_cast<v12::pEventBuilderFragment>(item.getItemPointer());
    EQ(
        sizeof(v12::EventBuilderFragment) + sizeof(payload),
        size_t(pItem->s_header.s_size)
    );
    
    EQ(0, memcmp(payload, pItem->s_body, sizeof(payload)));
}