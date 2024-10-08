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

using namespace ufmt;

class v12fragtest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v12fragtest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    
    CPPUNIT_TEST(getts);        // These are from the per type
    CPPUNIT_TEST(getsid);       // methods.
    CPPUNIT_TEST(payloadsize);
    CPPUNIT_TEST(payloadptr);
    CPPUNIT_TEST(btype);
    
    CPPUNIT_TEST(bodysize);     // From v12::CRingBuffer side casts.
    CPPUNIT_TEST(bodyptr_1);
    CPPUNIT_TEST(bodyptr_2);
    CPPUNIT_TEST(hasbodyhdr);
    CPPUNIT_TEST(getbodyheader);
    CPPUNIT_TEST(setbodyheader);
    CPPUNIT_TEST(bhdrts);
    CPPUNIT_TEST(bhdrsid);
    CPPUNIT_TEST(bhdrbtype);
    
    CPPUNIT_TEST(unktest);
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
    void getts();
    void getsid();
    void payloadsize();
    void payloadptr();
    void btype();
    
    void bodysize();
    void bodyptr_1();
    void bodyptr_2();
    void hasbodyhdr();
    void getbodyheader();
    void setbodyheader();
    void bhdrts();
    void bhdrsid();
    void bhdrbtype();
    
    void unktest();
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
    EQ(sizeof(v12::BodyHeader), size_t(pItem->s_bodyHeader.s_size));
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
// Get the timestamp fromt he event.
void v12fragtest::getts()
{
    uint8_t payload[100];
    for (int i =0; i < sizeof(payload); i++) {
        payload[i] = i;
    }
    v12::CRingFragmentItem item(0x1234568790, 2, sizeof(payload), payload, 1);
    
    EQ(uint64_t(0x1234568790), item.timestamp());
}
// get the source id:
void v12fragtest::getsid()
{
    uint8_t payload[100];
    for (int i =0; i < sizeof(payload); i++) {
        payload[i] = i;
    }
    v12::CRingFragmentItem item(0x1234568790, 2, sizeof(payload), payload, 1);
    EQ(uint32_t(2), item.source());
}
// get the payload size:

void v12fragtest::payloadsize()
{
    uint8_t payload[100];
    for (int i =0; i < sizeof(payload); i++) {
        payload[i] = i;
    }
    v12::CRingFragmentItem item(0x1234568790, 2, sizeof(payload), payload, 1);
    EQ(sizeof(payload), item.payloadSize());
    
}
// paylod pointer is correct:

void v12fragtest::payloadptr()
{
    uint8_t payload[100];
    for (int i =0; i < sizeof(payload); i++) {
        payload[i] = i;
    }
    v12::CRingFragmentItem item(0x1234568790, 2, sizeof(payload), payload, 1);
    v12::pEventBuilderFragment pItem =
        reinterpret_cast<v12::pEventBuilderFragment>(item.getItemPointer());
    void* pSb = pItem->s_body;
    EQ(pSb, item.payloadPointer());    // we already verified contents.
}
// get barrier type:

void v12fragtest::btype()
{
    uint8_t payload[100];
    for (int i =0; i < sizeof(payload); i++) {
        payload[i] = i;
    }
    v12::CRingFragmentItem item(0x1234568790, 2, sizeof(payload), payload, 1);
    EQ(uint32_t(1), item.barrierType());
}


// Get body size - s.b. payload size:

void v12fragtest::bodysize()
{
    uint8_t payload[100];
    for (int i =0; i < sizeof(payload); i++) {
        payload[i] = i;
    }
    v12::CRingFragmentItem item(0x1234568790, 2, sizeof(payload), payload, 1);
    EQ(sizeof(payload), item.getBodySize());
}
// getbody pointer not const.

void v12fragtest::bodyptr_1()
{
    uint8_t payload[100];
    for (int i =0; i < sizeof(payload); i++) {
        payload[i] = i;
    }
    v12::CRingFragmentItem item(0x1234568790, 2, sizeof(payload), payload, 1);
    v12::pEventBuilderFragment pItem =
        reinterpret_cast<v12::pEventBuilderFragment>(item.getItemPointer());
    void* pSb = pItem->s_body;
    EQ(pSb, item.getBodyPointer());
}


// get body pointer const
void v12fragtest::bodyptr_2()
{
    uint8_t payload[100];
    for (int i =0; i < sizeof(payload); i++) {
        payload[i] = i;
    }
    v12::CRingFragmentItem item(0x1234568790, 2, sizeof(payload), payload, 1);
    const v12::EventBuilderFragment* pItem =
        reinterpret_cast<const v12::EventBuilderFragment*>(item.getItemPointer());
    const void* pSb = pItem->s_body;
    const void* pis = item.getBodyPointer();
    EQ(pSb, pis);
}

// There's always a body header:

void v12fragtest::hasbodyhdr()
{
    uint8_t payload[100];
    for (int i =0; i < sizeof(payload); i++) {
        payload[i] = i;
    }
    v12::CRingFragmentItem item(0x1234568790, 2, sizeof(payload), payload, 1);
    ASSERT(item.hasBodyHeader());
}
// getBodyheader is correct:

void v12fragtest::getbodyheader()
{
    uint8_t payload[100];
    for (int i =0; i < sizeof(payload); i++) {
        payload[i] = i;
    }
    v12::CRingFragmentItem item(0x1234568790, 2, sizeof(payload), payload, 1);
    v12::pEventBuilderFragment pItem =
        reinterpret_cast<v12::pEventBuilderFragment>(item.getItemPointer());
    EQ(reinterpret_cast<void*>(&(pItem->s_bodyHeader)), item.getBodyHeader());
}
// setbody header - no size change and modifies properly.

void v12fragtest::setbodyheader()
{
    uint8_t payload[100];
    for (int i =0; i < sizeof(payload); i++) {
        payload[i] = i;
    }
    v12::CRingFragmentItem item(0x1234568790, 2, sizeof(payload), payload, 1);
    size_t initial = item.size();
    item.setBodyHeader(0x9876543210, 3, 0);
    size_t final = item.size();
    EQ(initial, final);
    EQ(uint64_t(0x9876543210), item.timestamp());
    EQ(uint32_t(3), item.source());
    EQ(uint32_t(0), item.barrierType());
}
// timestamp from body header methods are right:

void v12fragtest::bhdrts()
{
    uint8_t payload[100];
    for (int i =0; i < sizeof(payload); i++) {
        payload[i] = i;
    }
    v12::CRingFragmentItem item(0x1234568790, 2, sizeof(payload), payload, 1);
    EQ(item.timestamp(), item.getEventTimestamp());
}
// sid from body heaeder method is right.
void v12fragtest::bhdrsid()
{
    uint8_t payload[100];
    for (int i =0; i < sizeof(payload); i++) {
        payload[i] = i;
    }
    v12::CRingFragmentItem item(0x1234568790, 2, sizeof(payload), payload, 1);
    EQ(item.source(), item.getSourceId());
}
// barriertype from header is right:

void v12fragtest::bhdrbtype()
{
    uint8_t payload[100];
    for (int i =0; i < sizeof(payload); i++) {
        payload[i] = i;
    }
    v12::CRingFragmentItem item(0x1234568790, 2, sizeof(payload), payload, 1);
    EQ(item.barrierType(), item.getBarrierType());
}
// Unknow fragment just constructs to a fragment with  EVB_UNKNOWN_PAYLOAD
// type

void v12fragtest::unktest()
{
    uint8_t payload[100];
    for (int i =0; i < sizeof(payload); i++) {
        payload[i] = i;
    }
    v12::CUnknownFragment item(0x1234568790, 2, 1, sizeof(payload), payload);
    EQ(v12::EVB_UNKNOWN_PAYLOAD, item.type());
}