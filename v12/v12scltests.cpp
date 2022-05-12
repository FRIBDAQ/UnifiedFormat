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
#include <string.h>


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
    
    CPPUNIT_TEST(getts_1);
    CPPUNIT_TEST(getts_2);
    CPPUNIT_TEST(setts_1);
    CPPUNIT_TEST(setts_2);
    
    CPPUNIT_TEST(isincr_1);
    CPPUNIT_TEST(isincr_2);
    CPPUNIT_TEST(isincr_3);
    CPPUNIT_TEST(isincr_4);
    CPPUNIT_TEST(isincr_5);
    
    CPPUNIT_TEST(getscaler_1);
    CPPUNIT_TEST(getscaler_2);
    CPPUNIT_TEST(getscaler_3);
    CPPUNIT_TEST(getscaler_4);
    CPPUNIT_TEST(getscaler_5);
    CPPUNIT_TEST(getscaler_6);
    
    CPPUNIT_TEST(setscaler_1);
    CPPUNIT_TEST(setscaler_2);
    CPPUNIT_TEST(setscaler_3);
    CPPUNIT_TEST(setscaler_4);
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
    
    void getts_1();
    void getts_2();
    void setts_1();
    void setts_2();
    
    void isincr_1();
    void isincr_2();
    void isincr_3();
    void isincr_4();
    void isincr_5();
    
    void getscaler_1();
    void getscaler_2();
    void getscaler_3();
    void getscaler_4();
    void getscaler_5();
    void getscaler_6();
    
    void setscaler_1();
    void setscaler_2();
    void setscaler_3();
    void setscaler_4();
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
// get timestamp nonbody header.
void v12scltest::getts_1()
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
    EQ(now, item.getTimestamp());
}
// get timestamp from body header.
void v12scltest::getts_2()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    v12::CRingScalerItem item(
        0x1234567890, 1, 2, 10, 20, now, scalers, 2
    );
    
    EQ(now, item.getTimestamp());
}
// SEt timestamp in non-bodyh eader.

void v12scltest::setts_1()
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
    item.setTimestamp(now+10);
    EQ(now+10, item.getTimestamp());
}
// set timestamp in body header

void v12scltest::setts_2()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    v12::CRingScalerItem item(
        0x1234567890, 1, 2, 10, 20, now, scalers, 2
    );
    
    item.setTimestamp(now+10);
    EQ(now+10, item.getTimestamp());
}
// minimal constructor makes incremental:

void v12scltest::isincr_1()
{
    v12::CRingScalerItem item(32);
    ASSERT(item.isIncremental());
}
// nonbody header incremental:
void v12scltest::isincr_2()
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
    ASSERT(item.isIncremental());
}
// non body header not incremental

void v12scltest::isincr_3()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    v12::CRingScalerItem item(
        10, 20, now,
        scalers, false, 2
    );
    ASSERT(!item.isIncremental());
}
// body header is incremental:

void v12scltest::isincr_4()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    v12::CRingScalerItem item(
        0x1234567890, 1, 2, 10, 20, now, scalers, 2, true
    );
    ASSERT(item.isIncremental());
}
// body header not incremental:

void v12scltest::isincr_5()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    v12::CRingScalerItem item(
        0x1234567890, 1, 2, 10, 20, now, scalers, 2, false
    );
    ASSERT(!item.isIncremental());
}
// Get all scalers one at a time non body header.

void v12scltest::getscaler_1()
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
    for (int i =0; i < scalers.size(); i++) {
        EQ(scalers[i], item.getScaler(i));
    }
}
// Git invalid scaler nonbody header throws:

void v12scltest::getscaler_2()
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
    CPPUNIT_ASSERT_THROW(
        item.getScaler(32), std::logic_error
    );
}
// Get all scalers in one swoop not body header.

void v12scltest::getscaler_3()
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
    auto gotten = item.getScalers();
    EQ(scalers.size(), gotten.size());
    EQ(0, memcmp(scalers.data(), gotten.data(), scalers.size()*sizeof(uint32_t)));
}
// get scalers one at a time from body header item:

void v12scltest::getscaler_4()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    v12::CRingScalerItem item(
        0x1234567890, 1, 2, 10, 20, now, scalers, 2, true
    );
    for (int i =0; i < scalers.size(); i++) {
        EQ(scalers[i], item.getScaler(i));
    }
}
// get individual scaler bad index - body header.

void v12scltest::getscaler_5()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    v12::CRingScalerItem item(
        0x1234567890, 1, 2, 10, 20, now, scalers, 2, true
    );
    CPPUNIT_ASSERT_THROW(
        item.getScaler(32), std::logic_error
    );
}
// get scalers in one fell swoop; body header.

void v12scltest::getscaler_6()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    v12::CRingScalerItem item(
        0x1234567890, 1, 2, 10, 20, now, scalers, 2, true
    );
    auto gotten = item.getScalers();
    EQ(scalers.size(), gotten.size());
    EQ(0, memcmp(scalers.data(), gotten.data(), scalers.size()*sizeof(uint32_t)));
}

// set scaler channel non body header.

void v12scltest::setscaler_1()
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
    item.setScaler(5, 1234);
    EQ(uint32_t(1234), item.getScaler(5));
}
// set scaler invalid channel:

void v12scltest::setscaler_2()
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
    
    CPPUNIT_ASSERT_THROW(
        item.setScaler(32, 1), std::logic_error
    );
}
// set scaler channel body header
void v12scltest::setscaler_3()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    v12::CRingScalerItem item(
        0x1234567890, 1, 2, 10, 20, now, scalers, 2, true
    );
    
    item.setScaler(10, 1111);
    EQ(uint32_t(1111), item.getScaler(10));
}
void v12scltest::setscaler_4()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    v12::CRingScalerItem item(
        0x1234567890, 1, 2, 10, 20, now, scalers, 2, true
    );
    CPPUNIT_ASSERT_THROW(
        item.setScaler(32, 0), std::logic_error
    );
}
