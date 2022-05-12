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
    CPPUNIT_TEST_SUITE_END();
    
private:

public:
    void setUp() {
        
    }
    void tearDown() {
        
    }
protected:
    void construct_1();
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