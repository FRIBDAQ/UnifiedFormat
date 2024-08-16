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

/** @file:  v11fragtests.cpp
 *  @brief: Test v11::CRingFragmentItem.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CRingFragmentItem.h"
#include "DataFormat.h"

using namespace ufmt;

class v11fragtest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v11fragtest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    
    CPPUNIT_TEST(stamp);
    CPPUNIT_TEST(sid);
    CPPUNIT_TEST(barrier);
    
    CPPUNIT_TEST(payload_1);
    CPPUNIT_TEST(payload_2);
    CPPUNIT_TEST(payload_3);
    
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
    
    void stamp();
    void sid();
    void barrier();
    
    void payload_1();
    void payload_2();
    void payload_3();
    
    void bodyhdr_1();
    void bodyhdr_2();
    void bodyhdr_3();
};

CPPUNIT_TEST_SUITE_REGISTRATION(v11fragtest);

// Construct empty.
void v11fragtest::construct_1()
{
    v11::CRingFragmentItem item(0x1234567890, 1, 0, nullptr, 0);
    EQ(v11::EVB_FRAGMENT, item.type());
    EQ(sizeof(v11::RingItemHeader) + sizeof(v11::BodyHeader), size_t(item.size()));
    v11::pEventBuilderFragment pItem = reinterpret_cast<v11::pEventBuilderFragment>(item.getItemPointer());
    v11::pBodyHeader p = &(pItem->s_bodyHeader);
    EQ(sizeof(v11::BodyHeader), size_t(p->s_size));
    EQ(uint64_t(0x1234567890), p->s_timestamp);
    EQ(uint32_t(1), p->s_sourceId);
    EQ(uint32_t(0), p->s_barrier);
    
    void* pCursor = item.getBodyCursor();
    void* pBody = item.getBodyPointer();
    EQ(pBody, pCursor);
}
// item with nonn ull body
void v11fragtest::construct_2()
{
    uint16_t data[100];
    for (int i =0; i < 100; i++) {
        data[i] = i;
    }
    v11::CRingFragmentItem item(0x1234567890, 1, sizeof(data), data );
    
    EQ(sizeof(v11::EventBuilderFragment) + sizeof(data), size_t(item.size()));
    uint16_t* p = reinterpret_cast<uint16_t*>(item.getBodyPointer());
    for (uint16_t i =0; i < 100; i++) {
        EQ(i, *p);
        p++;
    }
    uint16_t* pCursor = reinterpret_cast<uint16_t*>(item.getBodyCursor());
    EQ(p, pCursor);
}
// Tests for getters:

void v11fragtest::stamp()
{
    v11::CRingFragmentItem item(0x1234567890, 1, 0, nullptr, 0);
    EQ(uint64_t(0x1234567890), item.timestamp());
}
void v11fragtest::sid()
{
    v11::CRingFragmentItem item(0x1234567890, 1, 0, nullptr, 0);
    EQ(uint32_t(1), item.source());
}
void v11fragtest::barrier()
{
    v11::CRingFragmentItem item(0x1234567890, 1, 0, nullptr, 0);
    EQ(uint32_t(0), item.barrierType());
}

// payload size for empty item is zero:

void v11fragtest::payload_1()
{
    v11::CRingFragmentItem item(0x1234567890, 1, 0, nullptr, 0);
    EQ(size_t(0), item.payloadSize());
    EQ(item.getBodyCursor(), item.payloadPointer());
}
// Payload size for nonempty item is data size:

void v11fragtest::payload_2()
{
    uint16_t data[100];
    for (int i =0; i < 100; i++) {
        data[i] = i;
    }
    v11::CRingFragmentItem item(0x1234567890, 1, sizeof(data), data );
    EQ(sizeof(data), item.payloadSize());
}
// Payload pointer points to payload.

void v11fragtest::payload_3()
{
     uint16_t data[100];
    for (int i =0; i < 100; i++) {
        data[i] = i;
    }
    v11::CRingFragmentItem item(0x1234567890, 1, sizeof(data), data );
    
    uint16_t* p = reinterpret_cast<uint16_t*>(item.payloadPointer());
    for (int i = 0; i < 100; i++) {
        EQ(data[i], p[i]);
    }
}
// Has body header is true:

void v11fragtest::bodyhdr_1()
{
    v11::CRingFragmentItem item(0x1234567890, 1, 0, nullptr, 0);
    ASSERT(item.hasBodyHeader());
}
// getBodyheader gives the right stuff:

void v11fragtest::bodyhdr_2()
{
    v11::CRingFragmentItem item(0x1234567890, 1, 0, nullptr, 0);
    const v11::BodyHeader* p= reinterpret_cast<const v11::BodyHeader*>(item.getBodyHeader());
    
    EQ(sizeof(v11::BodyHeader), size_t(p->s_size));
    EQ(uint64_t(0x1234567890), p->s_timestamp);
    EQ(uint32_t(1), p->s_sourceId);
    EQ(uint32_t(0), p->s_barrier);
    
}
// setBody Header modifies in place:

void v11fragtest::bodyhdr_3()
{
    v11::CRingFragmentItem item(0x1234567890, 1, 0, nullptr, 0);
    size_t initial = item.size();
    item.setBodyHeader(0x9876543210, 1, 2);
    size_t final  = item.size();
    EQ(initial, final);
    
    const v11::BodyHeader* p = reinterpret_cast<const v11::BodyHeader*>(item.getBodyHeader());
    EQ(sizeof(v11::BodyHeader), size_t(p->s_size));
    EQ(uint64_t(0x9876543210), p->s_timestamp);
    EQ(uint32_t(1), p->s_sourceId);
    EQ(uint32_t(2), p->s_barrier);
    
}