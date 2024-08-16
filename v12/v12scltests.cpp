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


using namespace ufmt;

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
    
    CPPUNIT_TEST(getnscaler_1);
    CPPUNIT_TEST(getnscaler_2);
    
    CPPUNIT_TEST(osid_1);
    CPPUNIT_TEST(osid_2);
    CPPUNIT_TEST(osid_3);
    
    CPPUNIT_TEST(bsize_1);
    CPPUNIT_TEST(bsize_2);
    
    CPPUNIT_TEST(bptr_1);
    CPPUNIT_TEST(bptr_2);
    CPPUNIT_TEST(bptr_3);
    CPPUNIT_TEST(bptr_4);
    
    CPPUNIT_TEST(hasbhdr_1);
    CPPUNIT_TEST(hasbhdr_2);
    
    CPPUNIT_TEST(getbodyhdr_1);
    CPPUNIT_TEST(getbodyhdr_2);
    
    CPPUNIT_TEST(setbodyhdr_1);
    CPPUNIT_TEST(setbodyhdr_2);
    
    CPPUNIT_TEST(getfrts_1);
    CPPUNIT_TEST(getfrts_2);
    
    CPPUNIT_TEST(srcid_1);
    CPPUNIT_TEST(srcid_2);
    
    CPPUNIT_TEST(barrier_1);
    CPPUNIT_TEST(barrier_2);
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
    
    void getnscaler_1();
    void getnscaler_2();
    
    void osid_1();
    void osid_2();
    void osid_3();
    
    void bsize_1();
    void bsize_2();
    
    void bptr_1();
    void bptr_2();
    void bptr_3();
    void bptr_4();
    
    void hasbhdr_1();
    void hasbhdr_2();
    
    void getbodyhdr_1();
    void getbodyhdr_2();
    
    void setbodyhdr_1();
    void setbodyhdr_2();
    
    void getfrts_1();
    void getfrts_2();
    
    void srcid_1();
    void srcid_2();
    
    void barrier_1();
    void barrier_2();
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

// number of scalers from no body header:
void v12scltest::getnscaler_1()
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
    EQ(uint32_t(32), item.getScalerCount());
}
// nmber of scalers from body header:

void v12scltest::getnscaler_2()
{
     time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    v12::CRingScalerItem item(
        0x1234567890, 1, 2, 10, 20, now, scalers, 2, true
    );
    EQ(uint32_t(32), item.getScalerCount());
}
// Original sid of minimal construction:

void v12scltest::osid_1()
{
    v12::CRingScalerItem item(1);
    EQ(uint32_t(0), item.getOriginalSourceId());
}
// original sid of full non body header construction

void v12scltest::osid_2()
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
    EQ(uint32_t(0), item.getOriginalSourceId());
}
// body header initially inherits body header sid

void v12scltest::osid_3()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    v12::CRingScalerItem item(
        0x1234567890, 1, 2, 10, 20, now, scalers, 2, true
    );

    EQ(uint32_t(1), item.getOriginalSourceId());
}

// body size no body header:

void v12scltest::bsize_1()
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
    EQ(sizeof(v12::ScalerItemBody) + 32*sizeof(uint32_t), size_t(item.getBodySize()));
}
// bodysizse if body eader no different:

void v12scltest::bsize_2()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    v12::CRingScalerItem item(
        0x1234567890, 1, 2, 10, 20, now, scalers, 2, true
    );
    EQ(sizeof(v12::ScalerItemBody) + 32*sizeof(uint32_t), size_t(item.getBodySize()));
}
// const body pointer from non body header item.

void v12scltest::bptr_1()
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
    const v12::ScalerItem* pItem = reinterpret_cast<const v12::ScalerItem*>(item.getItemPointer());
    const v12::ScalerItemBody* pSb = &(pItem->s_body.u_noBodyHeader.s_body);
    
    const v12::ScalerItemBody* pIs = reinterpret_cast<const v12::ScalerItemBody*>(item.getBodyPointer());
    
    EQ(pSb, pIs);
}
// const body pointer from  item with body header:

void v12scltest::bptr_2()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    v12::CRingScalerItem item(
        0x1234567890, 1, 2, 10, 20, now, scalers, 2, true
    );
    const v12::ScalerItem* pItem = reinterpret_cast<const v12::ScalerItem*>(item.getItemPointer());
    const v12::ScalerItemBody* pSb = &(pItem->s_body.u_hasBodyHeader.s_body);
    
    const v12::ScalerItemBody* pIs = reinterpret_cast<const v12::ScalerItemBody*>(item.getBodyPointer());
    
    EQ(pSb, pIs);
}
// non const body pointer from non bodyheaer
void v12scltest::bptr_3()
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
     v12::ScalerItem* pItem = reinterpret_cast< v12::ScalerItem*>(item.getItemPointer());
     v12::ScalerItemBody* pSb = &(pItem->s_body.u_noBodyHeader.s_body);
    
     v12::ScalerItemBody* pIs = reinterpret_cast< v12::ScalerItemBody*>(item.getBodyPointer());
    
    EQ(pSb, pIs);
}
// non const body pointer from body header item.

void v12scltest::bptr_4()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    v12::CRingScalerItem item(
        0x1234567890, 1, 2, 10, 20, now, scalers, 2, true
    );
     v12::ScalerItem* pItem = reinterpret_cast< v12::ScalerItem*>(item.getItemPointer());
     v12::ScalerItemBody* pSb = &(pItem->s_body.u_hasBodyHeader.s_body);
    
     v12::ScalerItemBody* pIs = reinterpret_cast< v12::ScalerItemBody*>(item.getBodyPointer());
    
    EQ(pSb, pIs);
}
// non bodyheader has none.

void v12scltest::hasbhdr_1()
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
    ASSERT(!item.hasBodyHeader());
}
// body hgeader has body header
//
void v12scltest::hasbhdr_2()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    v12::CRingScalerItem item(
        0x1234567890, 1, 2, 10, 20, now, scalers, 2, true
    );
    ASSERT(item.hasBodyHeader());
}
// get body header from non body header item is nullptr.

void v12scltest::getbodyhdr_1()
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
    ASSERT(nullptr == item.getBodyHeader());
}
// get body header from  body header is correct:

void v12scltest::getbodyhdr_2()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    v12::CRingScalerItem item(
        0x1234567890, 1, 2, 10, 20, now, scalers, 2, true
    );
    const v12::ScalerItem* pItem =
        reinterpret_cast<const v12::ScalerItem*>(item.getItemPointer());
    const v12::BodyHeader*  pbHdr =
        reinterpret_cast<const v12::BodyHeader*>(item.getBodyHeader());
    ASSERT(nullptr != pbHdr);
    EQ(&(pItem->s_body.u_hasBodyHeader.s_bodyHeader), pbHdr);
}
// setbodyhdr - with non body header starting point.

void v12scltest::setbodyhdr_1()
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
    size_t original = item.size();
    item.setBodyHeader(0x222222222, 1, 2);
    size_t snow      = item.size();
    EQ(original + sizeof(v12::BodyHeader) - sizeof(uint32_t), snow);
    
    const v12::BodyHeader* p =
        reinterpret_cast<const v12::BodyHeader*>(item.getBodyHeader());
    ASSERT(p != nullptr);
    const v12::ScalerItem* pItem =
        reinterpret_cast<const v12::ScalerItem*>(item.getItemPointer());
    EQ(&(pItem->s_body.u_hasBodyHeader.s_bodyHeader), p);
    EQ(sizeof(v12::BodyHeader), size_t(p->s_size));
    EQ(uint64_t(0x222222222), p->s_timestamp);
    EQ(uint32_t(1), p->s_sourceId);
    EQ(uint32_t(2), p->s_barrier);
    
    auto data = item.getScalers();    // Good enough to test data moved.
    EQ(scalers.size(), data.size());
    EQ(
       0,
       memcmp(scalers.data(), data.data(), sizeof(uint32_t)*scalers.size())
    );
       
}
// setbodyhdr with body header starting point:

void v12scltest::setbodyhdr_2()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    v12::CRingScalerItem item(
        0x1234567890, 1, 2, 10, 20, now, scalers, 2, true
    );
    size_t original = item.size();
    item.setBodyHeader(0x543212345, 2, 3);
    size_t final = item.size();
    EQ(original, final);
    
    const v12::BodyHeader* p =
        reinterpret_cast<const v12::BodyHeader*>(item.getBodyHeader());
    ASSERT(p != nullptr);
    EQ(sizeof(v12::BodyHeader), size_t(p->s_size));
    EQ(uint64_t(0x543212345), p->s_timestamp);
    EQ(uint32_t(2), p->s_sourceId);
    EQ(uint32_t(3), p->s_barrier);
}
// get fragment ts with not body header throws

void v12scltest::getfrts_1()
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
        item.getEventTimestamp(), std::logic_error
    );
}
// Get fragment t with body header:

void v12scltest::getfrts_2()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    v12::CRingScalerItem item(
        0x1234567890, 1, 2, 10, 20, now, scalers, 2, true
    );
    
    uint64_t ts;
    CPPUNIT_ASSERT_NO_THROW(
        ts = item.getEventTimestamp()
    );
    EQ(uint64_t(0x1234567890), ts);
}
// get source id from non body header throws:

void v12scltest::srcid_1()
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
        item.getSourceId(), std::logic_error
    );
}
// get source id from body header item:
void v12scltest::srcid_2()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    v12::CRingScalerItem item(
        0x1234567890, 1, 2, 10, 20, now, scalers, 2, true
    );
    
    uint32_t sid;
    CPPUNIT_ASSERT_NO_THROW(
        sid = item.getSourceId()
    );
    EQ(uint32_t(1), sid);
}
// get barrier id from nonbody header throws:

void v12scltest::barrier_1()
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
        item.getBarrierType(), std::logic_error
    );
}

// get barrier type from body header:
void v12scltest::barrier_2()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    v12::CRingScalerItem item(
        0x1234567890, 1, 2, 10, 20, now, scalers, 2, true
    );
    
    uint32_t bid;
    CPPUNIT_ASSERT_NO_THROW(
        bid = item.getBarrierType()
    );
    EQ(uint32_t(2), bid);
}