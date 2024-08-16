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

using namespace ufmt;

class v11unktest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v11unktest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    
    CPPUNIT_TEST(bodyptr_1);
    CPPUNIT_TEST(bodyptr_1);
    
    CPPUNIT_TEST(bodyhdr_1);
    CPPUNIT_TEST(bodyhdr_2);
    CPPUNIT_TEST(bodyhdr_3);
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
    
    void bodyptr_1();
    void bodyptr_2();
    
    void bodyhdr_1();
    void bodyhdr_2();
    void bodyhdr_3();
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
// get body pointer not const.
void v11unktest::bodyptr_1()
{
     std::vector<std::uint16_t> data;
    for (int i =0; i < 100; i++) {
        data.push_back(i);
    }
    v11::CUnknownFragment item(
        0x1234567890, 1, 0, data.size()*sizeof(uint16_t),  data.data()
    );
    v11::EventBuilderFragment* pItem =
        reinterpret_cast<v11::EventBuilderFragment*>(item.getItemPointer());
    uint8_t* pBody = reinterpret_cast<uint8_t*>(item.getBodyPointer());
    EQ(&(pItem->s_body[0]), pBody);
}
// get body pointer const.

void v11unktest::bodyptr_2()
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
    const uint8_t* pBody = reinterpret_cast<const uint8_t*>(item.getBodyPointer());
    EQ(&(pItem->s_body[0]), pBody);
}
//  always has a body header:

void v11unktest::bodyhdr_1()
{
    std::vector<std::uint16_t> data;
    for (int i =0; i < 100; i++) {
        data.push_back(i);
    }
    v11::CUnknownFragment item(
        0x1234567890, 1, 0, data.size()*sizeof(uint16_t),  data.data()
    );
    ASSERT(item.hasBodyHeader());
}
// can get the correct body header:

void v11unktest::bodyhdr_2()
{
    std::vector<std::uint16_t> data;
    for (int i =0; i < 100; i++) {
        data.push_back(i);
    }
    v11::CUnknownFragment item(
        0x1234567890, 1, 0, data.size()*sizeof(uint16_t),  data.data()
    );
    const v11::BodyHeader* pBh = reinterpret_cast<const v11::BodyHeader*>(item.getBodyHeader());
    const v11::EventBuilderFragment* pItem =
        reinterpret_cast<const v11::EventBuilderFragment*>(item.getItemPointer());
    EQ(&(pItem->s_bodyHeader), pBh);
    
}
// can modify body header:

void v11unktest::bodyhdr_3()
{
    std::vector<std::uint16_t> data;
    for (int i =0; i < 100; i++) {
        data.push_back(i);
    }
    v11::CUnknownFragment item(
        0x1234567890, 1, 0, data.size()*sizeof(uint16_t),  data.data()
    );
    item.setBodyHeader(0x111111111, 2, 5);
    const v11::BodyHeader* pBh =
        reinterpret_cast<const v11::BodyHeader*>(item.getBodyHeader());
    EQ(sizeof(v11::BodyHeader), size_t(pBh->s_size));
    EQ(uint64_t(0x111111111), pBh->s_timestamp);
    EQ(uint32_t(2), pBh->s_sourceId);
    EQ(uint32_t(5), pBh->s_barrier);
}