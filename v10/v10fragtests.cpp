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

/** @file:  v10fragtests.cpp
 *  @brief: 
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CRingFragmentItem.h"
#include "DataFormat.h"
#include <string>
using namespace ufmt;
class v10fragtest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v10fragtest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    CPPUNIT_TEST(selectors);
    CPPUNIT_TEST(payload);
    CPPUNIT_TEST(bodyhdr_1);
    CPPUNIT_TEST(bodyhdr_2);
    CPPUNIT_TEST(typeString);
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
    void selectors();
    void payload();
    
    void bodyhdr_1();
    void bodyhdr_2();
    void typeString();
};

CPPUNIT_TEST_SUITE_REGISTRATION(v10fragtest);

void v10fragtest::construct_1()
{
    v10::CRingFragmentItem item(0x123456789, 2, 0, nullptr, 5);
    
    v10::pEventBuilderFragment p =
        reinterpret_cast<v10::pEventBuilderFragment>(item.getItemPointer());
    EQ(v10::EVB_FRAGMENT, p->s_header.s_type);
    EQ(sizeof(v10::EventBuilderFragment) - sizeof(uint32_t),
       size_t(p->s_header.s_size));
    EQ(uint64_t(0x123456789), p->s_timestamp);
    EQ(uint32_t(2), p->s_sourceId);
    EQ(uint32_t(0), p->s_payloadSize);
    EQ(uint32_t(5), p->s_barrierType);
}

// Can see the payload and it's right:

void v10fragtest::construct_2()
{
    uint32_t payload[100];
    for (int i = 0; i < 100; i++) {
        payload[i] = i;
    }
    v10::CRingFragmentItem item(0x9876543210, 2, sizeof(payload), payload);
    
    v10::pEventBuilderFragment p =
        reinterpret_cast<v10::pEventBuilderFragment>(item.getItemPointer());
    EQ(sizeof(payload), size_t(p->s_payloadSize));
    for (int i = 0; i < 100; i ++) {
        EQ(payload[i], p->s_body[i]);
    }
}

// can fetch scalar items from the object.

void v10fragtest::selectors()
{
    uint32_t payload[100];
    for (int i = 0; i < 100; i++) {
        payload[i] = i;
    }
    v10::CRingFragmentItem item(0x9876543210, 2, sizeof(payload), payload,5);
    
    EQ(uint64_t(0x9876543210), item.timestamp());
    EQ(uint32_t(2), item.source());
    EQ(sizeof(payload), item.payloadSize());
    EQ(uint32_t(5), item.barrierType());
    
}
// can fetch payload:

void
v10fragtest::payload()
{
    uint32_t payload[100];
    for (int i = 0; i < 100; i++) {
        payload[i] = i;
    }
    v10::CRingFragmentItem item(0x9876543210, 2, sizeof(payload), payload);
    
    const uint32_t* p = reinterpret_cast<const uint32_t*>(item.payloadPointer());
    for (int i=0; i < 100; i++) {
        EQ(payload[i], p[i]);
    }
}
// hasBOdyHeader is false:

void
v10fragtest::bodyhdr_1()
{
    uint32_t payload[100];
    for (int i = 0; i < 100; i++) {
        payload[i] = i;
    }
    v10::CRingFragmentItem item(0x9876543210, 2, sizeof(payload), payload);
    
    ASSERT(!item.hasBodyHeader());
}
//getbodyheader is null.

void
v10fragtest::bodyhdr_2()
{
    uint32_t payload[100];
    for (int i = 0; i < 100; i++) {
        payload[i] = i;
    }
    v10::CRingFragmentItem item(0x9876543210, 2, sizeof(payload), payload);
    
    ASSERT(item.getBodyHeader() == nullptr);
}
// typeName -> "Event fragment"

void
v10fragtest::typeString()
{
    uint32_t payload[100];
    for (int i = 0; i < 100; i++) {
        payload[i] = i;
    }
    v10::CRingFragmentItem item(0x9876543210, 2, sizeof(payload), payload);
    
    EQ(std::string("Event fragment"), item.typeName());
}