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

/** @file:  v11statetest.cpp
 *  @brief: Tests for v11:CRingStateItem
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#include "CRingStateChangeItem.h"
#include "DataFormat.h"

#include <time.h>
#include <string.h>

class v11statetest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v11statetest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    CPPUNIT_TEST(construct_3);
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
};

CPPUNIT_TEST_SUITE_REGISTRATION(v11statetest);

// Simple constructor.
void v11statetest::construct_1()
{
    time_t now = time(nullptr);
    v11::CRingStateChangeItem item;
    const v11::StateChangeItem* pItem =
        reinterpret_cast<const v11::StateChangeItem*>(item.getItemPointer());
    
    EQ(
       sizeof(v11::RingItemHeader) +
       sizeof(uint32_t) +
       sizeof(v11::StateChangeItemBody),
       size_t(pItem->s_header.s_size)
    );
    EQ(v11::BEGIN_RUN, pItem->s_header.s_type);
    EQ(uint32_t(0), pItem->s_body.u_noBodyHeader.s_mbz);
    
    const v11::StateChangeItemBody* pBody = &(pItem->s_body.u_noBodyHeader.s_body);
    EQ(uint32_t(0), pBody->s_runNumber);
    EQ(uint32_t(0), pBody->s_timeOffset);
    ASSERT(pBody->s_Timestamp - now <= 1);
    EQ(uint32_t(1), pBody->s_offsetDivisor);
    EQ(size_t(0), strlen(pBody->s_title));
}
// Full non bod header constructor.

void v11statetest::construct_2()
{
    time_t now = time(nullptr);
    v11::CRingStateChangeItem item(
        v11::PAUSE_RUN, 1234, 10, now, "This is a title"
    );
    const v11::StateChangeItem* pItem =
        reinterpret_cast<const v11::StateChangeItem*>(item.getItemPointer());
    
    EQ(
       sizeof(v11::RingItemHeader) +
       sizeof(uint32_t) +
       sizeof(v11::StateChangeItemBody),
       size_t(pItem->s_header.s_size)
    );
    EQ(v11::PAUSE_RUN, pItem->s_header.s_type);
    EQ(uint32_t(0), pItem->s_body.u_noBodyHeader.s_mbz);
    
    const v11::StateChangeItemBody* pBody = &(pItem->s_body.u_noBodyHeader.s_body);
    EQ(uint32_t(1234), pBody->s_runNumber);
    EQ(uint32_t(10), pBody->s_timeOffset);
    EQ(uint32_t(now), pBody->s_Timestamp);
    EQ(uint32_t(1), pBody->s_offsetDivisor);
    EQ(0, strcmp("This is a title", pBody->s_title));
    
}
// Construct with body header.

void v11statetest::construct_3()
{
    
}

