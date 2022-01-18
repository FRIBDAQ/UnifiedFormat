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

/** @file:  ringitemabtests.cpp
 *  @brief: Test the abstract ring item.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#define private public
#define protected public
#include "CRingItem.h"
#undef private
#undef public
#include <DataFormat.h>

/** Since the CRingItem class is abstract we need a minimal concrete sublcass
 * to test any of it.
 */

class CTestRingItem : public CRingItem {
public:
    CTestRingItem(pRingItem pItem) : CRingItem(pItem) {}
    virtual void* getBodyHeader() const {return nullptr;}
    virtual void setBodyHeader(uint64_t timestamp, uint32_t sourceId,
                         uint32_t barrierType = 0) {}
};


class abringitemtest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(abringitemtest);
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

CPPUNIT_TEST_SUITE_REGISTRATION(abringitemtest);

// Simplest construction is from an existing empty raw ring item.
void abringitemtest::construct_1()
{
    RingItem r;
    r.s_header.s_size = sizeof(RingItemHeader);
    r.s_header.s_type = PHYSICS_EVENT;
    
    CTestRingItem item(&r);
    EQ(sizeof(RingItemHeader), size_t(item.m_pItem->s_header.s_size));
    EQ(PHYSICS_EVENT, item.m_pItem->s_header.s_type);
}