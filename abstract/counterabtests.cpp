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

/** @file:  counterabtests.cpp
 *  @brief: Tests for the CRingPhysicsEventCountItem class
 *         in abstract.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CRingPhysicsEventCountItem.h"
#include <time.h>
#include <stdint.h>
#include <stdexcept>

using namespace ufmt;

class counterabtest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(counterabtest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    CPPUNIT_TEST(construct_3);
    
    CPPUNIT_TEST(toffset_1);
    CPPUNIT_TEST(toffset_2);
    CPPUNIT_TEST(toffset_3);
    
    CPPUNIT_TEST(tstamp_1);
    CPPUNIT_TEST(tstamp_2);
    
    CPPUNIT_TEST(evtcount_1);
    CPPUNIT_TEST(evtcount_2);
    
    CPPUNIT_TEST(originalsid);
    
    CPPUNIT_TEST(bodyhdr_1);
    CPPUNIT_TEST(type_name);
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
    void construct_3();
    
    void toffset_1();
    void toffset_2();
    void toffset_3();
    
    void tstamp_1();
    void tstamp_2();
    
    void evtcount_1();
    void evtcount_2();
    
    void originalsid();
    
    void bodyhdr_1();
    
    
    void type_name();
};

CPPUNIT_TEST_SUITE_REGISTRATION(counterabtest);

// Default constructor 
void counterabtest::construct_1()
{
    CRingPhysicsEventCountItem* pItem(0);
    try {
        CPPUNIT_ASSERT_NO_THROW(pItem = new CRingPhysicsEventCountItem);
        uint32_t* pBody =
            reinterpret_cast<uint32_t*>(pItem->getBodyPointer());
        EQ(uint32_t(0), *pBody);    // Time offset.
        pBody++;
        EQ(uint32_t(1), *pBody);    // Offset divisor.
        pBody++;
        
        time_t now = time(nullptr);
        ASSERT((now - *pBody) <= 1); // timestamp - might be on a sec. edge.
        pBody++;
        EQ(uint32_t(0), *pBody);     // original source id.
        pBody++;
        uint64_t* p64 = reinterpret_cast<uint64_t*>(pBody);
        EQ(uint64_t(0), *p64);
    } catch (...) {
        delete pItem;    // violated asertions are exceptions
        throw;
    }
    delete pItem;
}
// COunt and offset contructor but original source id is defaulted.
// as is clock timestamp.

void counterabtest::construct_2()
{
    CRingPhysicsEventCountItem* pItem(0);
    try {
        CPPUNIT_ASSERT_NO_THROW(
            pItem = new CRingPhysicsEventCountItem(2345, 10, 2) 
        );
        uint32_t* pBody =
            reinterpret_cast<uint32_t*>(pItem->getBodyPointer());
        EQ(uint32_t(10), *pBody);    // Time offset.
        pBody++;
        EQ(uint32_t(2), *pBody);    // Offset divisor.
        pBody++;
        
        time_t now = time(nullptr);
        ASSERT((now - *pBody) <= 1); // timestamp - might be on a sec. edge.
        pBody++;
        EQ(uint32_t(0), *pBody);     // original source id.
        pBody++;
        uint64_t* p64 = reinterpret_cast<uint64_t*>(pBody);
        EQ(uint64_t(2345), *p64);
    }
    catch (...) {
        delete pItem;
        throw;
    }
    delete pItem;
}
// fully specified constructor

void counterabtest::construct_3()
{
    CRingPhysicsEventCountItem* pItem(0);
    try {
        CPPUNIT_ASSERT_NO_THROW(
            pItem = new CRingPhysicsEventCountItem(
                666, 15, time(nullptr), 5, 3
            )
        );
        uint32_t* pBody =
            reinterpret_cast<uint32_t*>(pItem->getBodyPointer());
        EQ(uint32_t(15), *pBody);    // Time offset.
        pBody++;
        EQ(uint32_t(3), *pBody);    // Offset divisor.
        pBody++;
        
        time_t now = time(nullptr);
        ASSERT((now - *pBody) <= 1); // timestamp - might be on a sec. edge.
        pBody++;
        EQ(uint32_t(5), *pBody);     // original source id.
        pBody++;
        uint64_t* p64 = reinterpret_cast<uint64_t*>(pBody);
        EQ(uint64_t(666), *p64);
    }
    catch (...) {
        delete pItem;
        throw;
    }
    delete pItem;
}
// Selector to geth the time offset:
void counterabtest::toffset_1()
{
    CRingPhysicsEventCountItem item(124, 5);
    EQ(uint32_t(5), item.getTimeOffset());
}
// set time offset revealed in get time offset.

void counterabtest::toffset_2()
{
    CRingPhysicsEventCountItem item;
    item.setTimeOffset(100);
    EQ(uint32_t(100), item.getTimeOffset());
}
// COmpute the time offset properly given a divisor:

void counterabtest::toffset_3()
{
    CRingPhysicsEventCountItem item(1234, 15, 3);
    EQ(float(5.0), item.computeElapsedTime());
    EQ(uint32_t(3), item.getTimeDivisor());
}
// Get the timestamp gives correct value:

void counterabtest::tstamp_1()
{
    time_t t = time(nullptr);
    CRingPhysicsEventCountItem item(246810, 10, t, 1);
    EQ(t, item.getTimestamp());
}
// setting timestamp:

void counterabtest::tstamp_2()
{
    time_t t = time(nullptr) + 10;    // 10 seconds into the future.
    CRingPhysicsEventCountItem item;  // Stamped with now.
    
    item.setTimestamp(t);             // now 10 seconds + now.
    EQ(t, item.getTimestamp());
}
// We can get the correct event count.

void counterabtest::evtcount_1()
{
    CRingPhysicsEventCountItem item(1234, 15, 3);
    EQ(uint64_t(1234), item.getEventCount());
}
// we can set an event countl
void counterabtest::evtcount_2()
{
    CRingPhysicsEventCountItem item;
    item.setEventCount(9876543210);
    EQ(uint64_t(9876543210), item.getEventCount());
}
// Get the original source id

void counterabtest::originalsid()
{
    CRingPhysicsEventCountItem item(
        123456789, 10, time(nullptr), 12
    );
    
    EQ(uint32_t(12), item.getOriginalSourceId());
}

// Get body header null.
void counterabtest::bodyhdr_1()
{
    CRingPhysicsEventCountItem item;
    ASSERT(
        item.getBodyHeader() == nullptr
    );
}
// typeName is 'Trigger count'

void counterabtest::type_name()
{
    CRingPhysicsEventCountItem item;
    EQ(std::string("Trigger count"), item.typeName());
}