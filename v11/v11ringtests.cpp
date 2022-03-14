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

/** @file:  v11ringtest.cpp
 *  @brief:  Tests for v11::CRingItem.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CRingItem.h"
#include "DataFormat.h"
#include <stdexcept>


class v11ringtest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v11ringtest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    
    CPPUNIT_TEST(bodyptr_1);
    CPPUNIT_TEST(bodyptr_2);
    CPPUNIT_TEST(bodyptr_3);
    CPPUNIT_TEST(bodyptr_4);
    
    CPPUNIT_TEST(bodycursor_1);
    CPPUNIT_TEST(bodycursor_2);
    
    CPPUNIT_TEST(bodysize_1);
    CPPUNIT_TEST(bodysize_2);
    
    CPPUNIT_TEST(hasbodyheader_1);
    CPPUNIT_TEST(hasbodyheader_2);
    
    CPPUNIT_TEST(gettimestamp_1);
    CPPUNIT_TEST(gettimestamp_2);
    
    CPPUNIT_TEST(getsrcid_1);
    CPPUNIT_TEST(getsrcid_2);
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
    void bodyptr_3();
    void bodyptr_4();
    
    void bodycursor_1();
    void bodycursor_2();
    
    void bodysize_1();
    void bodysize_2();
    
    void hasbodyheader_1();
    void hasbodyheader_2();
    
    void gettimestamp_1();
    void gettimestamp_2();
    
    void getsrcid_1();
    void getsrcid_2();
};

CPPUNIT_TEST_SUITE_REGISTRATION(v11ringtest);

// Bodyheader-less construction.
void v11ringtest::construct_1()
{
    v11::CRingItem item(v11::PHYSICS_EVENT, 100);
    const v11::RingItem* pItem = reinterpret_cast<const v11::RingItem*>(item.getItemPointer());
    
    EQ(v11::PHYSICS_EVENT, pItem->s_header.s_type);
    EQ(sizeof(v11::RingItemHeader) + sizeof(uint32_t), size_t(pItem->s_header.s_size));
    EQ(uint32_t(0), pItem->s_body.u_noBodyHeader.s_mbz);
}
// Construction with body header.

void v11ringtest::construct_2()
{
    v11::CRingItem item(
        v11::PHYSICS_EVENT,
        0x1234567890, 2, 0, 100
    );
    const v11::RingItem* pItem = reinterpret_cast<const v11::RingItem*>(item.getItemPointer());
    EQ(v11::PHYSICS_EVENT, pItem->s_header.s_type);
    EQ(sizeof(v11::RingItemHeader) + sizeof(v11::BodyHeader), size_t(pItem->s_header.s_size));
    EQ(sizeof(v11::BodyHeader), size_t(pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_size));
    
    const v11::BodyHeader* pH = &(pItem->s_body.u_hasBodyHeader.s_bodyHeader);
    EQ(sizeof(v11::BodyHeader), size_t(pH->s_size));
    EQ(uint64_t(0x1234567890), pH->s_timestamp);
    EQ(uint32_t(2), pH->s_sourceId);
    EQ(uint32_t(0), pH->s_barrier);
    
    
}
// Body pointer for non -body header item is correct (const).

void v11ringtest::bodyptr_1()
{
    v11::CRingItem item(v11::PHYSICS_EVENT, 100);
    
    // figure out the expected body ptr:
    
    const uint8_t* p = reinterpret_cast<const uint8_t*>(item.getItemPointer());
    p += sizeof(v11::RingItemHeader);
    p += sizeof(uint32_t);
    
    const void* pActual = item.getBodyPointer();
    
    EQ(p, reinterpret_cast<const uint8_t*>(pActual));
}
// Body pointer for body header item is correct (const)

void v11ringtest::bodyptr_2()
{
    v11::CRingItem item (v11::PHYSICS_EVENT,
        0x1234567890, 2, 0, 100
    );
    const uint8_t*  p = reinterpret_cast<const uint8_t*>(item.getItemPointer());
    p += sizeof(v11::RingItemHeader) + sizeof(v11::BodyHeader);
    
    const void* pActual = item.getBodyPointer();
    EQ(p, reinterpret_cast<const uint8_t*>(pActual));
}
// body pointe for body headerless item is correct (non-const)

void v11ringtest::bodyptr_3()
{
v11::CRingItem item(v11::PHYSICS_EVENT, 100);
    
    // figure out the expected body ptr:
    
    uint8_t* p = reinterpret_cast<uint8_t*>(item.getItemPointer());
    p += sizeof(v11::RingItemHeader);
    p += sizeof(uint32_t);
    
    void* pActual = item.getBodyPointer();
    
    EQ(p, reinterpret_cast<uint8_t*>(pActual));
}    

// body pointer for body headered item is correct (non-const).

void v11ringtest::bodyptr_4()
{
    v11::CRingItem item (v11::PHYSICS_EVENT,
        0x1234567890, 2, 0, 100
    );
    uint8_t*  p = reinterpret_cast<uint8_t*>(item.getItemPointer());
    p += sizeof(v11::RingItemHeader) + sizeof(v11::BodyHeader);
    
    void* pActual = item.getBodyPointer();
    EQ(p, reinterpret_cast<uint8_t*>(pActual));
}

// Body cursor initiall points to the body when no body headers

void v11ringtest::bodycursor_1()
{
    v11::CRingItem item(v11::PHYSICS_EVENT, 100);
    void* pBody =  item.getBodyPointer();
    void* pCursor = item.getBodyCursor();
    
    EQ(pBody, pCursor);
}
// Same as above for an item with a body  header:

void v11ringtest::bodycursor_2()
{
    v11::CRingItem item(
        v11::PHYSICS_EVENT,
        0x1234567890, 2, 0, 100
    );
    void* pBody =  item.getBodyPointer();
    void* pCursor = item.getBodyCursor();
    
    EQ(pBody, pCursor);
}
//body size for non body header items:

void v11ringtest::bodysize_1()
{
    v11::CRingItem item(v11::PHYSICS_EVENT, 1000);
    uint8_t* p = reinterpret_cast<uint8_t*>(item.getBodyCursor());
    for (int i =0; i < 100; i++) {
        *p++ = i;
    }
    item.setBodyCursor(p); item.updateSize();
    EQ(size_t(100), item.getBodySize());
}
// same as above but the item has a body header:

void v11ringtest::bodysize_2()
{
    v11::CRingItem item(
        v11::PHYSICS_EVENT,
        0x1234567890, 2, 0, 1000
    );
    uint8_t* p = reinterpret_cast<uint8_t*>(item.getBodyCursor());
    for (int i =0; i < 100; i++) {
        *p++ = i;
    }
    item.setBodyCursor(p); item.updateSize();
    EQ(size_t(100), item.getBodySize());
}
// hasbodyheader -- does not.
void v11ringtest::hasbodyheader_1() {
    v11::CRingItem item(v11::PHYSICS_EVENT, 1000);
    ASSERT(!item.hasBodyHeader());
}
// has body header and has it.

void v11ringtest::hasbodyheader_2() {
    v11::CRingItem item(
        v11::PHYSICS_EVENT,
        0x1234567890, 2, 0, 1000
    );
    ASSERT(item.hasBodyHeader());
}
// No body header :: gettimestamp throws:

void v11ringtest::gettimestamp_1()
{
    v11::CRingItem item(v11::PHYSICS_EVENT, 1000);
    CPPUNIT_ASSERT_THROW(
        item.getEventTimestamp(),
        std::logic_error
    );
}
// has body header gettimestamp gives it up:

void v11ringtest::gettimestamp_2()
{
    v11::CRingItem item(
        v11::PHYSICS_EVENT,
        0x1234567890, 2, 0, 1000
    );
    uint64_t ts;
    CPPUNIT_ASSERT_NO_THROW(
        ts = item.getEventTimestamp()
    );
    EQ(uint64_t(0x1234567890), ts);
}
// Get sourceid tests:
void v11ringtest::getsrcid_1()
{
    v11::CRingItem item(v11::PHYSICS_EVENT, 1000);
    CPPUNIT_ASSERT_THROW(
        item.getSourceId(),
        std::logic_error
    );
}
void v11ringtest::getsrcid_2()
{
    v11::CRingItem item(
        v11::PHYSICS_EVENT,
        0x1234567890, 2, 0, 1000
    );
    uint32_t sid;
    CPPUNIT_ASSERT_NO_THROW(
        sid = item.getSourceId()
    );
    EQ(uint32_t(2), sid);
}