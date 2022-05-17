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

/** @file:  v12statetests.cpp
 *  @brief: Tests for v12::CRingStateChangeitem.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CRingStateChangeItem.h"
#include "DataFormat.h"
#include <time.h>
#include <string.h>
#include <stdexcept>

class v12statetest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v12statetest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    CPPUNIT_TEST(construct_3);
    CPPUNIT_TEST(construct_4);
    CPPUNIT_TEST(construct_5);
    CPPUNIT_TEST(construct_6);
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
    void construct_4();
    void construct_5();
    void construct_6();
};

CPPUNIT_TEST_SUITE_REGISTRATION(v12statetest);

/// minimal good construction.

void v12statetest::construct_1()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(v12::BEGIN_RUN);
    const v12::StateChangeItem* pItem =
        reinterpret_cast<const v12::StateChangeItem*>(item.getItemPointer());
    EQ(v12::BEGIN_RUN, pItem->s_header.s_type);
    EQ(
        sizeof(v12::RingItemHeader) + sizeof(uint32_t) +
        sizeof(v12::StateChangeItemBody),
        size_t(pItem->s_header.s_size)
    );
    EQ(sizeof(uint32_t), size_t(pItem->s_body.u_noBodyHeader.s_empty));
    
    const v12::StateChangeItemBody* pBody =
        &(pItem->s_body.u_noBodyHeader.s_body);
    EQ(uint32_t(0), pBody->s_runNumber);
    EQ(uint32_t(0), pBody->s_timeOffset);
    ASSERT((pBody->s_Timestamp - now) < 1);
    EQ(uint32_t(1), pBody->s_offsetDivisor);
    EQ(uint32_t(0), pBody->s_originalSid);
    EQ(size_t(0), strlen(pBody->s_title));
}
// Minimal construction with bad type:

void v12statetest::construct_2()
{
    CPPUNIT_ASSERT_THROW(
        v12::CRingStateChangeItem item(v12::PHYSICS_EVENT),
        std::logic_error
    );
}

// good full non-bodyheader construction.
void v12statetest::construct_3()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        v12::BEGIN_RUN, 1234, 0, now, "This is a title" 
    );
    const v12::StateChangeItem* pItem =
        reinterpret_cast<const v12::StateChangeItem*>(item.getItemPointer());
    EQ(v12::BEGIN_RUN, pItem->s_header.s_type);
    EQ(
        sizeof(v12::RingItemHeader) + sizeof(uint32_t) +
        sizeof(v12::StateChangeItemBody),
        size_t(pItem->s_header.s_size)
    );
    EQ(sizeof(uint32_t), size_t(pItem->s_body.u_noBodyHeader.s_empty));
    
    const v12::StateChangeItemBody* pBody =
        &(pItem->s_body.u_noBodyHeader.s_body);
    EQ(uint32_t(1234), pBody->s_runNumber);
    EQ(uint32_t(0), pBody->s_timeOffset);
    EQ(now, time_t(pBody->s_Timestamp));
    EQ(uint32_t(1), pBody->s_offsetDivisor);
    EQ(uint32_t(0), pBody->s_originalSid);
    EQ(std::string("This is a title"), std::string(pBody->s_title));
}
// full non-body header construction with bad type:

void v12statetest::construct_4()
{
    time_t now = time(nullptr);
    CPPUNIT_ASSERT_THROW(
        v12::CRingStateChangeItem item(
            v12::PERIODIC_SCALERS, 1234, 0, now, "This is a title"
        ), std::logic_error
    );
}
// good construction with body header.

void v12statetest::construct_5()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        0x1234567890, 1, 2, v12::BEGIN_RUN, 12, 0, now, "This is a title"
    );
    const v12::StateChangeItem* pItem =
        reinterpret_cast<const v12::StateChangeItem*>(item.getItemPointer());
        
    EQ(v12::BEGIN_RUN, pItem->s_header.s_type);
    EQ(
        sizeof(v12::RingItemHeader) + sizeof(v12::BodyHeader) +
        sizeof(v12::StateChangeItemBody),
        size_t(pItem->s_header.s_size)
    );
    
    const v12::BodyHeader* pBh = &(pItem->s_body.u_hasBodyHeader.s_bodyHeader);
    EQ(sizeof(v12::BodyHeader), size_t(pBh->s_size));
    EQ(uint64_t(0x1234567890), pBh->s_timestamp);
    EQ(uint32_t(1), pBh->s_sourceId);
    EQ(uint32_t(2), pBh->s_barrier);
    
    const v12::StateChangeItemBody* pBody =
        &(pItem->s_body.u_hasBodyHeader.s_body);
    EQ(uint32_t(12), pBody->s_runNumber);
    EQ(uint32_t(0), pBody->s_timeOffset);
    EQ(now, time_t(pBody->s_Timestamp));
    EQ(uint32_t(1), pBody->s_offsetDivisor);
    EQ(uint32_t(1), pBody->s_originalSid);
    EQ(std::string("This is a title"), std::string(pBody->s_title));
}
// full body header construction but bad type.

void v12statetest::construct_6()
{
    time_t now = time(nullptr);
    CPPUNIT_ASSSERT_THROW(
        v12::CRingStateChangeItem item(
            0x1234567890, 1, 2, v12::ABNORMAL_ENDRUN, 12, 0, now, "This is a title"
        ), std::logic_error
    )
}