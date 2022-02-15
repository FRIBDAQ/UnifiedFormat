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

/** @file:  v10scalertests.cpp
 *  @brief: Tests for v10::CringScalerp->
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CRingScalerItem.h"
#include "DataFormat.h"
#include <time.h>

class v10scalertest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v10scalertest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    CPPUNIT_TEST(construct_3);
    
    CPPUNIT_TEST(start_1);
    CPPUNIT_TEST(start_2);
    CPPUNIT_TEST(start_3);
    CPPUNIT_TEST(start_4);
    CPPUNIT_TEST(start_5);
    CPPUNIT_TEST(start_6);
    
    CPPUNIT_TEST(end_1);
    CPPUNIT_TEST(end_2);
    CPPUNIT_TEST(end_3);
    CPPUNIT_TEST(end_4);
    CPPUNIT_TEST(end_5);
    CPPUNIT_TEST(end_6);
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
    
    void start_1();
    void start_2();
    void start_3();
    void start_4();
    void start_5();
    void start_6();
    
    void end_1();
    void end_2();
    void end_3();
    void end_4();
    void end_5();
    void end_6();
};

CPPUNIT_TEST_SUITE_REGISTRATION(v10scalertest);

void v10scalertest::construct_1()
{
    time_t now = time(nullptr);
    v10::CRingScalerItem item(10);
    const v10::ScalerItem* p =
        reinterpret_cast<const v10::ScalerItem*>(item.getItemPointer());
        
    EQ(v10::INCREMENTAL_SCALERS, p->s_header.s_type);
    EQ(sizeof(v10::ScalerItem) + 9*sizeof(uint32_t), size_t(p->s_header.s_size));
    
    EQ(uint32_t(0), p->s_intervalStartOffset);
    EQ(uint32_t(0), p->s_intervalEndOffset);
    ASSERT((p->s_timestamp - now) <= 1);
    EQ(uint32_t(10), p->s_scalerCount);
    for(int i =0; i < 10; i++) {
        EQ(uint32_t(0), p->s_scalers[i]);
    }
}

// fuLL construction of incremental scaler.

void v10scalertest::construct_2()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i);
    }
    v10::CRingScalerItem item(
        10, 20, now, scalers
    );
    
    const v10::ScalerItem* p =
        reinterpret_cast<const v10::ScalerItem*>(item.getItemPointer());
        
    EQ(v10::INCREMENTAL_SCALERS, p->s_header.s_type);
    EQ(sizeof(v10::ScalerItem) + 31*sizeof(uint32_t), size_t(p->s_header.s_size));
    
    EQ(uint32_t(10), p->s_intervalStartOffset);
    EQ(uint32_t(20), p->s_intervalEndOffset);
    EQ(uint32_t(now), p->s_timestamp);
    EQ(uint32_t(32), p->s_scalerCount);
    for(int i =0; i < 32; i++) {
        EQ(uint32_t(i), p->s_scalers[i]);
    }
}

// full construction of non-incremental scaler:

void v10scalertest::construct_3()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i);
    }
    v10::CRingScalerItem item(
        10, 20, now, scalers, false, 1, 2
    );
    
    const v10::NonIncrTimestampedScaler* p =
        reinterpret_cast<const v10::NonIncrTimestampedScaler*>(item.getItemPointer());
    EQ(v10::TIMESTAMPED_NONINCR_SCALERS, p->s_header.s_type);
    EQ(
       sizeof(v10::NonIncrTimestampedScaler) + 31*sizeof(uint32_t),
       size_t(p->s_header.s_size)
    );
    EQ(uint64_t(0), p->s_eventTimestamp);
    EQ(uint32_t(10), p->s_intervalStartOffset);
    EQ(uint32_t(20), p->s_intervalEndOffset);
    EQ(uint32_t(2), p->s_intervalDivisor);
    EQ(uint32_t(now), p->s_clockTimestamp);
    EQ(uint32_t(32), p->s_scalerCount);
    for (int i =0; i < 32; i++) {
        EQ(uint32_t(i), p->s_scalers[i]);
    }
}

// getStartTime incremental.

void v10scalertest::start_1()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i);
    }
    v10::CRingScalerItem item(
        10, 20, now, scalers
    );
    
    EQ(uint32_t(10), item.getStartTime());
}
// Set start time incremntal
void v10scalertest::start_2()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i);
    }
    v10::CRingScalerItem item(
        10, 20, now, scalers
    );
    
    item.setStartTime(5);
    EQ(uint32_t(5), item.getStartTime());
}
// Compute start time for incremental:

void v10scalertest::start_3()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i);
    }
    v10::CRingScalerItem item(
        10, 20, now, scalers
    );
    
    EQ(float(10), item.computeStartTime());
}
// Get start time for non incremental.

void v10scalertest::start_4()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i);
    }
    v10::CRingScalerItem item(
        10, 20, now, scalers, false, 0, 2
    );
    EQ(uint32_t(10), item.getStartTime());
    
    
}
// Set start time for non incremental

void v10scalertest::start_5()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i);
    }
    v10::CRingScalerItem item(
        10, 20, now, scalers, false, 0, 2
    );
    item.setStartTime(5);
    EQ(uint32_t(5), item.getStartTime());
    
}
// Compute start time works non incremental.

void v10scalertest::start_6()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i);
    }
    v10::CRingScalerItem item(
        10, 20, now, scalers, false, 0, 2
    );
    EQ(float(5.0), item.computeStartTime());
}

// get end time from incremental

void v10scalertest::end_1()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i);
    }
    v10::CRingScalerItem item(
        10, 20, now, scalers
    );
    EQ(uint32_t(20), item.getEndTime());
}
// set end time for incremental

void v10scalertest::end_2()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i);
    }
    v10::CRingScalerItem item(
        10, 20, now, scalers
    );
    
    item.setEndTime(25);
    EQ(uint32_t(25), item.getEndTime());
}
// Get computed end time for incremental (same as raw end time).

void v10scalertest::end_3()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i);
    }
    v10::CRingScalerItem item(
        10, 20, now, scalers
    );
    EQ(float(20), item.computeEndTime());
}
// Get end time for nonincremental

void v10scalertest::end_4()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i);
    }
    v10::CRingScalerItem item(
        10, 20, now, scalers, false, 0, 2
    );
    EQ(uint32_t(20), item.getEndTime());
}
// set end time.
void v10scalertest::end_5()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i);
    }
    v10::CRingScalerItem item(
        10, 20, now, scalers, false, 0, 2
    );
    item.setEndTime(25);
    EQ(uint32_t(25), item.getEndTime());
}
// computed end time:

void v10scalertest::end_6()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i=0; i < 32; i++) {
        scalers.push_back(i);
    }
    v10::CRingScalerItem item(
        10, 20, now, scalers, false, 0, 2
    );
    EQ(float(10.0), item.computeEndTime());
}