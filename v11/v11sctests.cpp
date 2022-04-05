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

/** @file:  v11sctests.cpp
 *  @brief: Tests for the v11::CRingScalerItem class.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CRingScalerItem.h"
#include "DataFormat.h"



class v11sctest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v11sctest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    CPPUNIT_TEST(construct_3);
    
    CPPUNIT_TEST(starttime_1);   // non body header.
    CPPUNIT_TEST(starttime_2);
    CPPUNIT_TEST(starttime_3);   // Body header
    CPPUNIT_TEST(starttime_4);
    CPPUNIT_TEST(starttime_5);   // Compute no body header.
    CPPUNIT_TEST(starttime_6);   // Compute with body header.
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
    
    void starttime_1();
    void starttime_2();
    void starttime_3();
    void starttime_4();
    void starttime_5();
    void starttime_6();
};

CPPUNIT_TEST_SUITE_REGISTRATION(v11sctest);

// only have the number of scalers present - no body header.
void v11sctest::construct_1()
{
    time_t now = time(nullptr);
    v11::CRingScalerItem item(32);
    const v11::ScalerItem* pItem =
        reinterpret_cast<const v11::ScalerItem*>(item.getItemPointer());
    EQ(sizeof(v11::RingItemHeader) + 33*sizeof(uint32_t) + sizeof(v11::ScalerItemBody),
       size_t(pItem->s_header.s_size));
    EQ(v11::PERIODIC_SCALERS, pItem->s_header.s_type);
    EQ(uint32_t(0), pItem->s_body.u_noBodyHeader.s_mbz);
    
    const v11::ScalerItemBody* pBody = &(pItem->s_body.u_noBodyHeader.s_body);
    EQ(uint32_t(0), pBody->s_intervalStartOffset);
    EQ(uint32_t(0), pBody->s_intervalEndOffset);
    ASSERT(pBody->s_timestamp - now <= 1);
    EQ(uint32_t(1), pBody->s_intervalDivisor);
    EQ(uint32_t(32), pBody->s_scalerCount);
    EQ(uint32_t(1), pBody->s_isIncremental);
    for (int i = 0; i < pBody->s_scalerCount; i++) {
        EQ(uint32_t(0), pBody->s_scalers[i]);
    }
    
}
void v11sctest::construct_2()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i =0;i < 32; i++) {
        scalers.push_back(i*200);
    }
    v11::CRingScalerItem item(10, 20, now, scalers);
    const v11::ScalerItem* pItem =
        reinterpret_cast<const v11::ScalerItem*>(item.getItemPointer());
    EQ(v11::PERIODIC_SCALERS, pItem->s_header.s_type);
    EQ(
        sizeof(v11::RingItemHeader) + sizeof(v11::BodyHeader)
        + sizeof(v11::ScalerItemBody) + scalers.size()*sizeof(uint32_t),
        size_t(pItem->s_header.s_size)
    );
    
    const v11::BodyHeader* pH =
        reinterpret_cast<const v11::BodyHeader*>(&pItem->s_body.u_hasBodyHeader.s_bodyHeader);
    EQ(sizeof(v11::BodyHeader), size_t(pH->s_size));
    EQ(uint64_t(0xffffffffffffffff), pH->s_timestamp);
    EQ(uint32_t(0), pH->s_sourceId);
    EQ(uint32_t(0), pH->s_barrier);
    
    const v11::ScalerItemBody* pBody =
        reinterpret_cast<const v11::ScalerItemBody*>(
            &pItem->s_body.u_hasBodyHeader.s_body
        );
    EQ(uint32_t(10), pBody->s_intervalStartOffset);
    EQ(uint32_t(20), pBody->s_intervalEndOffset);
    EQ(uint32_t(now), pBody->s_timestamp);
    EQ(uint32_t(1), pBody->s_intervalDivisor);
    EQ(uint32_t(scalers.size()), pBody->s_scalerCount);
    EQ(uint32_t(1), pBody->s_isIncremental);
    for (int i = 0; i < scalers.size(); i++) {
        EQ(scalers[i], pBody->s_scalers[i]);
    }
}
// full construcxtion
void v11sctest::construct_3()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(i*123);
    }
    v11::CRingScalerItem item(
        0x1234567890, 1, 0, 10, 20, now, scalers
    );
    
     const v11::ScalerItem* pItem =
        reinterpret_cast<const v11::ScalerItem*>(item.getItemPointer());
    EQ(v11::PERIODIC_SCALERS, pItem->s_header.s_type);
    EQ(
        sizeof(v11::RingItemHeader) + sizeof(v11::BodyHeader)
        + sizeof(v11::ScalerItemBody) + scalers.size()*sizeof(uint32_t),
        size_t(pItem->s_header.s_size)
    );
    
    const v11::BodyHeader* pH =
        reinterpret_cast<const v11::BodyHeader*>(&pItem->s_body.u_hasBodyHeader.s_bodyHeader);
    EQ(sizeof(v11::BodyHeader), size_t(pH->s_size));
    EQ(uint64_t(0x1234567890), pH->s_timestamp);
    EQ(uint32_t(1), pH->s_sourceId);
    EQ(uint32_t(0), pH->s_barrier);
    
    const v11::ScalerItemBody* pBody =
        reinterpret_cast<const v11::ScalerItemBody*>(
            &pItem->s_body.u_hasBodyHeader.s_body
        );
    EQ(uint32_t(10), pBody->s_intervalStartOffset);
    EQ(uint32_t(20), pBody->s_intervalEndOffset);
    EQ(uint32_t(now), pBody->s_timestamp);
    EQ(uint32_t(1), pBody->s_intervalDivisor);
    EQ(uint32_t(scalers.size()), pBody->s_scalerCount);
    EQ(uint32_t(1), pBody->s_isIncremental);
    for (int i = 0; i < scalers.size(); i++) {
        EQ(scalers[i], pBody->s_scalers[i]);
    }                       
    
}
// Get start time when there's no body header:

void
v11sctest::starttime_1()
{
    v11::CRingScalerItem item(32);
    EQ(uint32_t(0), item.getStartTime());
}
// Set start time when there's no body header
void
v11sctest::starttime_2()
{
    v11::CRingScalerItem item(32);
    item.setStartTime(10);
    EQ(uint32_t(10), item.getStartTime());
}

// get start time when there's a body header:

void v11sctest::starttime_3()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(i*123);
    }
    v11::CRingScalerItem item(
        0x1234567890, 1, 0, 10, 20, now, scalers
    );
    EQ(uint32_t(10), item.getStartTime());
}
// set start time when there's a body header

void v11sctest::starttime_4()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(i*123);
    }
    v11::CRingScalerItem item(
        0x1234567890, 1, 0, 10, 20, now, scalers
    );
    
    item.setStartTime(5);
    EQ(uint32_t(5), item.getStartTime());
}
// Compute start time for non body header/
void v11sctest::starttime_5()
{
    v11::CRingScalerItem item(32);
    v11::pScalerItem pItem = reinterpret_cast<v11::pScalerItem>(item.getItemPointer());
    
    // set non zero non-one start and divisor.
    
    pItem->s_body.u_noBodyHeader.s_body.s_intervalStartOffset =10;
    pItem->s_body.u_noBodyHeader.s_body.s_intervalDivisor =2;
    
    EQ(float(5.0), item.computeStartTime());
}
// Compute start time for body header:

void v11sctest::starttime_6()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(i*123);
    }
    v11::CRingScalerItem item(
        0x1234567890, 1, 0, 10, 20, now, scalers, 2
    );
    EQ(float(5.0), item.computeStartTime());
}