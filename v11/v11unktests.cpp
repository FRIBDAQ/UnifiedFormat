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

/** @file:  v11unktests.cpp
 *  @brief:  test v11::CUnknownFragment class.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CUnknownFragment.h"
#include "DataFormat.h"
#include <vector>

class v11unktest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v11unktest);
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

CPPUNIT_TEST_SUITE_REGISTRATION(v11unktest);

void v11unktest::construct_1()
{
    v11::CUnknownFragment item(0x1234567890, 1, 0, 0, nullptr);
    const v11::EventBuilderFragment* pItem =
        reinterpret_cast<const v11::EventBuilderFragment*>(item.getItemPointer());
    EQ(sizeof(v11::RingItemHeader) + sizeof(v11::BodyHeader), size_t(pItem->s_header.s_size));
    EQ(v11::EVB_UNKNOWN_PAYLOAD, pItem->s_header.s_type);
    
    EQ(uint64_t(0x1234567890), pItem->s_bodyHeader.s_timestamp);
    EQ(uint32_t(1), pItem->s_bodyHeader.s_sourceId);
    EQ(uint32_t(0), pItem->s_bodyHeader.s_barrier);
    
}
// with payload now:

void v11unktest::construct_2()
{
    std::vector<std::uint16_t> data;
    for (int i =0; i < 100; i++) {
        data.push_back(i);
    }
    v11::CUnknownFragment item(
        0x1234567890, 1, 0, data.size()*sizeof(uint16_t),  data.data()
    );
    const v11::EventBuilderFragment* pItem =
        reinterpret_cast<const v11::EventBuilderFragment*>(item.getItemPointer());
    EQ(
        sizeof(v11::RingItemHeader) + sizeof(v11::BodyHeader) +
        sizeof(uint16_t) * data.size(),
        size_t(pItem->s_header.s_size)
    );
    const uint16_t* p = reinterpret_cast<const uint16_t*>(pItem->s_body);
    for (int i =0; i < data.size(); i++) {
        EQ(data[i], p[i]);
    }
}