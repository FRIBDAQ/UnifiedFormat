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

/** @file:  fragabtests.cpp
 *  @brief: Tests for abstract CRingFragmentItem.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CRingFragmentItem.h"
#include <stdexcept>
#include <string>
#include <stdint.h>
using namespace ufmt;
class ringfabtest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(ringfabtest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    CPPUNIT_TEST(type_name);
    CPPUNIT_TEST(ts);
    CPPUNIT_TEST(src);
    CPPUNIT_TEST(payload_size);
    CPPUNIT_TEST(payload_pointer);
    CPPUNIT_TEST(btype);
    CPPUNIT_TEST(setheader);
    CPPUNIT_TEST(getbodyheader);
    CPPUNIT_TEST(hasbodyheader);
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
    
    void type_name();
    void ts();
    void src();
    void payload_size();
    void payload_pointer();
    void btype();
    void setheader();
    void getbodyheader();
    void hasbodyheader();
};

CPPUNIT_TEST_SUITE_REGISTRATION(ringfabtest);

// Construction with empty payload sould not result in an exception
void ringfabtest::construct_1()
{
    CPPUNIT_ASSERT_NO_THROW(
        CRingFragmentItem(1234, 1, 0, nullptr, 0)
    );
}
// Construct with a payload.
void ringfabtest::construct_2()
{
    CRingFragmentItem* pFrag(0);
    uint8_t data[100];
    for (int i = 0; i < sizeof(data); i++) {
        data[i] = i;
    }
    CPPUNIT_ASSERT_NO_THROW(
        pFrag = new CRingFragmentItem(1234, 1,  sizeof(data), data,  0)
    );
    // Point past the header:
    
    
    uint32_t* p32 = reinterpret_cast<uint32_t*>(pFrag->CRingItem::getBodyPointer());
    uint64_t* pTs = reinterpret_cast<uint64_t*>(++p32);
    EQ(uint64_t(1234), *pTs);
    p32 = reinterpret_cast<uint32_t*>(++pTs);
    EQ(uint32_t(1), *p32); p32++;
    EQ(uint32_t(0), *p32); p32++;

    
    uint8_t* pBody = reinterpret_cast<uint8_t*>(p32);
    for (uint8_t i =0; i < sizeof(data); i++)  {
        EQ(i, *pBody);
        pBody++;
    }
    
    delete pFrag;
}
// typename is "Event Fragment"
void ringfabtest::type_name()
{
    uint8_t data[100];
    for (int i = 0; i < sizeof(data); i++) {
        data[i] = i;
    }
    CRingFragmentItem item(1234, 1, sizeof(data), data, 0);
    EQ(std::string("Event fragment"), item.typeName());
}
// timestamp is what I tell it to be:

void ringfabtest::ts()
{
    uint8_t data[100];
    for (int i = 0; i < sizeof(data); i++) {
        data[i] = i;
    }
    CRingFragmentItem item(1234, 1, sizeof(data), data, 0);
    EQ(uint64_t(1234), item.timestamp());
}
// source id too:

void ringfabtest::src()
{
    uint8_t data[100];
    for (int i = 0; i < sizeof(data); i++) {
        data[i] = i;
    }
    CRingFragmentItem item(1234, 1, sizeof(data), data, 0);
    EQ(uint32_t(1), item.source());
    
}

// payload size sb size of data.

void ringfabtest::payload_size()
{
    uint8_t data[100];
    for (int i = 0; i < sizeof(data); i++) {
        data[i] = i;
    }
    CRingFragmentItem item(1234, 1, sizeof(data), data, 0);
    EQ(sizeof(data), item.payloadSize());
}
// payload pointer should point to a copy of data;

void ringfabtest::payload_pointer()
{
    uint8_t data[100];
    for (int i = 0; i < sizeof(data); i++) {
        data[i] = i;
    }
    CRingFragmentItem item(1234, 1, sizeof(data), data, 0);
    uint8_t* p = reinterpret_cast<uint8_t*>(item.payloadPointer());
    for (uint8_t i = 0; i < sizeof(data); i++) {
        EQ((int)i, (int)*p);
        p++;
    }
}
void ringfabtest::btype()
{
    uint8_t data[100];
    for (int i = 0; i < sizeof(data); i++) {
        data[i] = i;
    }
    CRingFragmentItem item(1234, 1, sizeof(data), data, 3);
    EQ(uint32_t(3), item.barrierType());
}
// set body header throws logic error.

void ringfabtest::setheader()
{
    uint8_t data[100];
    for (int i = 0; i < sizeof(data); i++) {
        data[i] = i;
    }
    CRingFragmentItem item(1234, 1, sizeof(data), data, 3);
    CPPUNIT_ASSERT_THROW(
        item.setBodyHeader(4321, 2, 1),
        std::logic_error
    );
}
// A pointer to the body header can be gotten:

void ringfabtest::getbodyheader()
{
    
    uint8_t data[100];
    for (int i = 0; i < sizeof(data); i++) {
        data[i] = i;
    }
    CRingFragmentItem item(1234,1, sizeof(data), data, 0);
    uint32_t* p32 = reinterpret_cast<uint32_t*>(item.getBodyHeader());
    p32++;                    // SKip size word.
    uint64_t* p64 = reinterpret_cast<uint64_t*>(p32);
    EQ(uint64_t(1234), *p64);  p64++;
    p32 = reinterpret_cast<uint32_t*>(p64);
    EQ(uint32_t(1), *p32); p32++;
    EQ(uint32_t(0), *p32);

}
void ringfabtest::hasbodyheader()
{
    uint8_t data[100];
    for (int i = 0; i < sizeof(data); i++) {
        data[i] = i;
    }
    CRingFragmentItem item(1234,1, sizeof(data), data, 0);
    ASSERT(item.hasBodyHeader());
}