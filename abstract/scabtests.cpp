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

/** @file:  scabtests.cpp
 *  @brief: Abstract scaler tests.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CRingScalerItem.h"
#include <time.h>
#include <stdexcept>

class scabtest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(scabtest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    
    CPPUNIT_TEST(starttime_1);
    CPPUNIT_TEST(starttime_2);
    CPPUNIT_TEST(starttime_3);
    
    CPPUNIT_TEST(endtime_1);
    CPPUNIT_TEST(endtime_2);
    CPPUNIT_TEST(endtime_3);
    
    CPPUNIT_TEST(tdiv);
    
    CPPUNIT_TEST(tstamp_1);
    CPPUNIT_TEST(tstamp_2);
    
    CPPUNIT_TEST(incremental);
    
    CPPUNIT_TEST(getscalers);
    
    CPPUNIT_TEST(getscaler_1);
    CPPUNIT_TEST(getscaler_2);
    CPPUNIT_TEST(setscaler_1);
    CPPUNIT_TEST(setscaler_2);
    
    CPPUNIT_TEST(count);
    CPPUNIT_TEST(sid);
    
    CPPUNIT_TEST(type_name);
    
    CPPUNIT_TEST(bodyHeader_1);
    CPPUNIT_TEST(bodyHeader_2);
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
    
    void starttime_1();
    void starttime_2();
    void starttime_3();
    
    void endtime_1();
    void endtime_2();
    void endtime_3();
    
    void tdiv();
    
    void tstamp_1();
    void tstamp_2();
    
    void incremental();
    
    void getscalers();
    
    void getscaler_1();
    void getscaler_2();
    void setscaler_1();
    void setscaler_2();
    
    void count();
    void sid();
    void type_name();
    
    void bodyHeader_1();
    void bodyHeader_2();
};

CPPUNIT_TEST_SUITE_REGISTRATION(scabtest);

// Empty construction.
//
void scabtest::construct_1()
{
    CRingScalerItem *pItem(0);
    try {
        CPPUNIT_ASSERT_NO_THROW(pItem = new CRingScalerItem(16));
        const RingItemHeader* pHdr =
            reinterpret_cast<const RingItemHeader*>(pItem->getItemPointer());
        const ScalerItemBody* pBody =
            reinterpret_cast<const ScalerItemBody*>(pHdr+1);
        EQ(uint32_t(0), pBody->s_intervalStartOffset);
        EQ(uint32_t(0), pBody->s_intervalEndOffset);
        time_t now = time(nullptr);
        ASSERT((now - pBody->s_timestamp) <= 1);  // Could hit sec. boundary.
        EQ(uint32_t(1), pBody->s_intervalDivisor);
        EQ(uint32_t(16), pBody->s_scalerCount);
        EQ(uint32_t(1), pBody->s_isIncremental);
        EQ(uint32_t(0), pBody->s_originalSid);
        
        for (int i = 0; i < 16; i++) {
            EQ(uint32_t(0), pBody->s_scalers[i]);
        }
    }
    catch (...) {             // Because failed assertions are
        delete pItem;         // thrown exceptions.
        throw;
    }
    
    delete pItem;
}
// full construction.
void scabtest::construct_2()
{
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(i);               // Simulated scalers.
    }
    time_t now = time(nullptr);
    
    CRingScalerItem* pItem(0);
    try {
        CPPUNIT_ASSERT_NO_THROW(pItem = new CRingScalerItem(
            10, 20, now, scalers, true, 5, 2
        ));
    }
    catch (...) {             // Because failed assertions are
        delete pItem;         // thrown exceptions.
        throw;
    }
    
    delete pItem;
    
}
// getStartTime gets the correct start time:

void scabtest::starttime_1()
{
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(i);               // Simulated scalers.
    }
    time_t now = time(nullptr);
    
    CRingScalerItem item(10, 20, now, scalers, true, 5, 2);
    EQ(uint32_t(10), item.getStartTime());
}
// Set startime modifies the start time:

void scabtest::starttime_2()
{
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(i);               // Simulated scalers.
    }
    time_t now = time(nullptr);
    
    CRingScalerItem item(10, 20, now, scalers, true, 5, 2);
    item.setStartTime(15);
    EQ(uint32_t(15), item.getStartTime());
}
// computStartTime folds in the divisor.

void scabtest::starttime_3()
{
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(i);               // Simulated scalers.
    }
    time_t now = time(nullptr);
    
    CRingScalerItem item(10, 20, now, scalers, true, 5, 5);
    EQ(float(2.0), item.computeStartTime());
}
// getEndTime works

void scabtest::endtime_1()
{
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(i);               // Simulated scalers.
    }
    time_t now = time(nullptr);
    
    CRingScalerItem item(10, 20, now, scalers, true, 5, 5);
    EQ(uint32_t(20), item.getEndTime());
}
// set end time works

void scabtest::endtime_2()
{
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(i);               // Simulated scalers.
    }
    time_t now = time(nullptr);
    
    CRingScalerItem item(10, 20, now, scalers, true, 5, 5);
    
    item.setEndTime(30);
    EQ(uint32_t(30), item.getEndTime());
    
}
// compute end time works.
void scabtest::endtime_3()
{
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(i);               // Simulated scalers.
    }
    time_t now = time(nullptr);
    
    CRingScalerItem item(10, 20, now, scalers, true, 5, 5);
    
    EQ(float(4.0), item.computeEndTime());
}

// time divisor is returned correctly.
void scabtest::tdiv()
{
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(i);               // Simulated scalers.
    }
    time_t now = time(nullptr);
    
    CRingScalerItem item(10, 20, now, scalers, true, 5, 5);
    EQ(uint32_t(5), item.getTimeDivisor());
}
// Can get the correct timestamp:

void scabtest::tstamp_1()
{
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(i);               // Simulated scalers.
    }
    time_t now = time(nullptr);
    
    CRingScalerItem item(10, 20, now, scalers, true, 5, 5);
    
    EQ(now, item.getTimestamp());
}
// Can set new timestamp
void scabtest::tstamp_2()
{
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(i);               // Simulated scalers.
    }
    time_t now = time(nullptr);
    
    CRingScalerItem item(10, 20, now, scalers, true, 5, 5);
    
    item.setTimestamp(now+5);
    EQ(now+5, item.getTimestamp());
}
void scabtest::incremental()
{
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(i);               // Simulated scalers.
    }
    time_t now = time(nullptr);
    
    CRingScalerItem item(10, 20, now, scalers, true, 5, 5);
    ASSERT(item.isIncremental());
    CRingScalerItem item2(10, 20, now, scalers,false, 5,5);
    ASSERT(!item2.isIncremental());
}

// getscalers gives the scaler array:

void scabtest::getscalers()
{
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(i);               // Simulated scalers.
    }
    time_t now = time(nullptr);
    CRingScalerItem item(10, 20, now, scalers, true, 5, 5);
    
    auto itemScalers = item.getScalers();
    EQ(scalers.size(), itemScalers.size());
    for (int i =0; i < scalers.size(); i++) {
        EQ(scalers[i], itemScalers[i]);
    }
    
    
}

// get with valid indices returns the correct result.
void scabtest::getscaler_1()
{
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(i);               // Simulated scalers.
    }
    time_t now = time(nullptr);
    CRingScalerItem item(10, 20, now, scalers, true, 5, 5);
    
    for (int i =0; i < scalers.size(); i++) {
        EQ(scalers[i], item.getScaler(i));
    }
}
// Get with invalid index throws std::out_of_range

void scabtest::getscaler_2()
{
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(i);               // Simulated scalers.
    }
    time_t now = time(nullptr);
    CRingScalerItem item(10, 20, now, scalers, true, 5, 5);
    
    CPPUNIT_ASSERT_THROW(
        item.getScaler(scalers.size()),
        std::out_of_range
    );
}
// Set scaler with good channels works.
void scabtest::setscaler_1()
{
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(i);               // Simulated scalers.
    }
    time_t now = time(nullptr);
    CRingScalerItem item(10, 20, now, scalers, true, 5, 5);
    
    
    for (int i = 0; i < scalers.size(); i++) {
        item.setScaler(i, i*2);
    }
    auto actual = item.getScalers();
    for (int i =0; i < scalers.size(); i++) {
        EQ(scalers[i]*2, actual[i]);
    }
}
// Set scaler with invaid channel throws std::out_of_range

void scabtest::setscaler_2()
{
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(i);               // Simulated scalers.
    }
    time_t now = time(nullptr);
    CRingScalerItem item(10, 20, now, scalers, true, 5, 5);
    
    
    CPPUNIT_ASSERT_THROW(
        item.setScaler(scalers.size(), 1234),
        std::out_of_range
    );
}
// correct scaler count returned:
void scabtest::count()
{
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(i);               // Simulated scalers.
    }
    time_t now = time(nullptr);
    CRingScalerItem item(10, 20, now, scalers, true, 5, 5);
    EQ(uint32_t(scalers.size()), item.getScalerCount());
}
// Sid correct.
void scabtest::sid()
{
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(i);               // Simulated scalers.
    }
    time_t now = time(nullptr);
    CRingScalerItem item(10, 20, now, scalers, true, 3, 5);
    EQ(uint32_t(3), item.getOriginalSourceId());
}
// Type name is "Scaler"
void scabtest::type_name()
{
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(i);               // Simulated scalers.
    }
    time_t now = time(nullptr);
    CRingScalerItem item(10, 20, now, scalers, true, 5, 5);
    
    EQ(std::string("Scaler"), item.typeName());
}
// getBodyheader throws std::logic_error.
void scabtest::bodyHeader_1()
{
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(i);               // Simulated scalers.
    }
    time_t now = time(nullptr);
    CRingScalerItem item(10, 20, now, scalers, true, 5, 5);
    
    CPPUNIT_ASSERT_THROW(
        item.getBodyHeader(),
        std::logic_error
    );
}
// SEt body header throws std::logic_error

void scabtest::bodyHeader_2()
{
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(i);               // Simulated scalers.
    }
    time_t now = time(nullptr);
    CRingScalerItem item(10, 20, now, scalers, true, 5, 5);
    
    CPPUNIT_ASSERT_THROW(
        item.setBodyHeader(123456789, 1, 0),
        std::logic_error
    );
}