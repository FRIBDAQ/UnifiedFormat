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

/** @file:  v12scltests.cpp
 *  @brief: Tests for v12::CRingScalerItem.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CRingScalerItem.h"
#include "DataFormat.h"
#include <time.h>


class v12scltest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v12scltest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    CPPUNIT_TEST(construct_3);
    
    CPPUNIT_TEST(setstart_1);
    CPPUNIT_TEST(setstart_2);
    
    CPPUNIT_TEST(getstart_1);
    CPPUNIT_TEST(getstart_2);
    
    CPPUNIT_TEST(computestart_1);
    CPPUNIT_TEST(computestart_2);
    
    CPPUNIT_TEST(getend_1);
    CPPUNIT_TEST(getend_2);
    CPPUNIT_TEST(setend_1);
    CPPUNIT_TEST(setend_2);
    
    CPPUNIT_TEST(computeend_1);
    CPPUNIT_TEST(computeend_2);
    
    CPPUNIT_TEST(divisor_1);
    CPPUNIT_TEST(divisor_2);
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
    
    void setstart_1();
    void setstart_2();
    
    void getstart_1();
    void getstart_2();
    
    void computestart_1();
    void computestart_2();
    
    void getend_1();
    void getend_2();
    
    void setend_1();
    void setend_2();
    
    void computeend_1();
    void computeend_2();
    
    void divisor_1();
    void divisor_2();
};

CPPUNIT_TEST_SUITE_REGISTRATION(v12scltest);

// Empty construction - zeroes everywhere but
// divisor  size is reserved properly.
void v12scltest::construct_1()
{
    time_t now = time(nullptr);
    v12::CRingScalerItem item(32);
    const v12::ScalerItem* pItem =
        reinterpret_cast<const v12::ScalerItem*>(item.getItemPointer());
    EQ(uint32_t(v12::PERIODIC_SCALERS), pItem->s_header.s_type);
    EQ(
        sizeof(v12::RingItemHeader) +
        sizeof(uint32_t) +
        sizeof(v12::ScalerItemBody) +
        32*sizeof(uint32_t),  
        size_t(pItem->s_header.s_size)
    );
    EQ(uint32_t(sizeof(uint32_t)), pItem->s_body.u_noBodyHeader.s_empty);
    const v12::ScalerItemBody* pBody =
        reinterpret_cast<const v12::ScalerItemBody*>(&(pItem->s_body.u_noBodyHeader.s_body));
    EQ(uint32_t(0), pBody->s_intervalStartOffset);
    EQ(uint32_t(0), pBody->s_intervalEndOffset);
    ASSERT(pBody->s_timestamp - now < 1);
    EQ(uint32_t(1), pBody->s_intervalDivisor);
    EQ(uint32_t(32), pBody->s_scalerCount);
    EQ(uint32_t(1), pBody->s_isIncremental);
    EQ(uint32_t(0), pBody->s_originalSid);
    for (int i =0; i < 32; i++) {
        EQ(uint32_t(0), pBody->s_scalers[i]);
    }
    
}
// Construct with no body header.
void v12scltest::construct_2()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    v12::CRingScalerItem item(
        10, 20, now,
        scalers
    );
    
    const v12::ScalerItem* pItem =
        reinterpret_cast<v12::ScalerItem*>(item.getItemPointer());
    EQ(uint32_t(v12::PERIODIC_SCALERS), pItem->s_header.s_type);
    EQ(
        sizeof(v12::RingItemHeader) +
        sizeof(uint32_t) +
        sizeof(v12::ScalerItemBody) +
        32*sizeof(uint32_t),  
        size_t(pItem->s_header.s_size)
    );
    EQ(uint32_t(sizeof(uint32_t)), pItem->s_body.u_noBodyHeader.s_empty);
    const v12::ScalerItemBody* pBody =
        reinterpret_cast<const v12::ScalerItemBody*>(&(pItem->s_body.u_noBodyHeader.s_body));
    EQ(uint32_t(10), pBody->s_intervalStartOffset);
    EQ(uint32_t(20), pBody->s_intervalEndOffset);
    EQ(uint32_t(now), pBody->s_timestamp);
    EQ(uint32_t(1), pBody->s_intervalDivisor);
    EQ(uint32_t(32), pBody->s_scalerCount);
    EQ(uint32_t(1), pBody->s_isIncremental);
    EQ(uint32_t(0), pBody->s_originalSid);
    for (int i =0; i < 32; i++) {
        EQ(scalers[i], pBody->s_scalers[i]);
    }
    
}
// vull construction with body header information.

void v12scltest::construct_3()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    v12::CRingScalerItem item(
        0x1234567890, 1, 2, 10, 20, now, scalers
    );
    const v12::ScalerItem* pItem =
        reinterpret_cast<v12::ScalerItem*>(item.getItemPointer());
    EQ(uint32_t(v12::PERIODIC_SCALERS), pItem->s_header.s_type);
    EQ(
        sizeof(v12::RingItemHeader) +
        sizeof(v12::BodyHeader)     +
        sizeof(v12::ScalerItemBody) +
        32*sizeof(uint32_t),  
        size_t(pItem->s_header.s_size)
    );
    
    const v12::BodyHeader* pBh = &(pItem->s_body.u_hasBodyHeader.s_bodyHeader);
    EQ(uint32_t(sizeof(v12::BodyHeader)), pBh->s_size);
    EQ(uint64_t(0x1234567890), pBh->s_timestamp);
    EQ(uint32_t(1), pBh->s_sourceId);
    EQ(uint32_t(2), pBh->s_barrier);
    
    const v12::ScalerItemBody* pBody =
        reinterpret_cast<const v12::ScalerItemBody*>(&(pItem->s_body.u_hasBodyHeader.s_body));
    EQ(uint32_t(10), pBody->s_intervalStartOffset);
    EQ(uint32_t(20), pBody->s_intervalEndOffset);
    EQ(uint32_t(now), pBody->s_timestamp);
    EQ(uint32_t(1), pBody->s_intervalDivisor);
    EQ(uint32_t(32), pBody->s_scalerCount);
    EQ(uint32_t(1), pBody->s_isIncremental);
    EQ(uint32_t(1), pBody->s_originalSid);
    for (int i =0; i < 32; i++) {
        EQ(scalers[i], pBody->s_scalers[i]);
    }
}
// set start no body header.
void
v12scltest::setstart_1()
{
    v12::CRingScalerItem item(1);
    const v12::ScalerItem* pItem = reinterpret_cast<const v12::ScalerItem*>(item.getItemPointer());
    item.setStartTime(123);
    EQ(uint32_t(123),
       pItem->s_body.u_noBodyHeader.s_body.s_intervalStartOffset);
}
// set start body header

void
v12scltest::setstart_2()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    v12::CRingScalerItem item(
        0x1234567890, 1, 2, 10, 20, now, scalers
    );
    const v12::ScalerItem* pItem =
        reinterpret_cast<v12::ScalerItem*>(item.getItemPointer());
    
    item.setStartTime(123);
    EQ(uint32_t(123),
       pItem->s_body.u_hasBodyHeader.s_body.s_intervalStartOffset);
}

// get start time from non body header.

void v12scltest::getstart_1()
{
    v12::CRingScalerItem item(1);
    
    item.setStartTime(123);
    EQ(uint32_t(123), item.getStartTime());
}

// get start time from body header

void v12scltest::getstart_2()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    v12::CRingScalerItem item(
        0x1234567890, 1, 2, 10, 20, now, scalers
    );
    
    item.setStartTime(123);
    EQ(uint32_t(123), item.getStartTime());
}
// computestart for non body header but non 1 divisor.

void v12scltest::computestart_1()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    v12::CRingScalerItem item(
        10, 20, now,
        scalers, true, 2
    );
    EQ(float(5.0), item.computeStartTime());
    
    
}
// computestart for body header item with non 1 divisor.

void v12scltest::computestart_2()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    v12::CRingScalerItem item(
        0x1234567890, 1, 2, 10, 20, now, scalers, 2
    );
    EQ(float(5.0), item.computeStartTime());
}

// get end time non body header.

void v12scltest::getend_1()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    v12::CRingScalerItem item(
        10, 20, now,
        scalers, true, 2
    );
    EQ(uint32_t(20), item.getEndTime());
}
// get end time body header:
void v12scltest::getend_2()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    v12::CRingScalerItem item(
        0x1234567890, 1, 2, 10, 20, now, scalers, 2
    );
    EQ(uint32_t(20), item.getEndTime());
}
// setend non body header.
void v12scltest::setend_1()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    v12::CRingScalerItem item(
        10, 20, now,
        scalers, true, 2
    );
    item.setEndTime(1234);
    EQ(uint32_t(1234), item.getEndTime());
}
// setend on body header item:

void v12scltest::setend_2()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    v12::CRingScalerItem item(
        0x1234567890, 1, 2, 10, 20, now, scalers, 2
    );
    item.setEndTime(1234);
    EQ(uint32_t(1234), item.getEndTime());
}
// compute end time on non body header:
void v12scltest::computeend_1()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    v12::CRingScalerItem item(
        10, 20, now,
        scalers, true, 2
    );
    EQ(float(10.0), item.computeEndTime());
}
// compute end time for body header item.

void v12scltest::computeend_2()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    v12::CRingScalerItem item(
        0x1234567890, 1, 2, 10, 20, now, scalers, 2
    );
    
    EQ(float(10), item.computeEndTime());
}
// get time divisor from nonbody header.

void v12scltest::divisor_1()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    v12::CRingScalerItem item(
        10, 20, now,
        scalers, true, 2
    );
    EQ(uint32_t(2), item.getTimeDivisor());
}
// get time divisor from body header

void v12scltest::divisor_2()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    v12::CRingScalerItem item(
        0x1234567890, 1, 2, 10, 20, now, scalers, 2
    );
    
    EQ(uint32_t(2), item.getTimeDivisor());
}