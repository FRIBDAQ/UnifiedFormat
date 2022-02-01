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

class scabtest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(scabtest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
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
    
    CRingSclaerItem* pItem(0);
    try {
        CPPUNIT_ASSERT_NO_THROW(pitem = new CRingScalerItem(
            10, 20, now, scalers, true, 5, 2
        ));
    }
    catch (...) {             // Because failed assertions are
        delete pItem;         // thrown exceptions.
        throw;
    }
    
    delete pItem;
    
}