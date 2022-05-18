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

/** @file:  v12texttests
 *  @brief: Test suite for v12::CRingTextItem class and methods.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CRingTextItem.h"
#include "DataFormat.h"
#include <time.h>

class v12txttest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v12txttest);
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

CPPUNIT_TEST_SUITE_REGISTRATION(v12txttest);

// Make pretty empty/useless item:
void v12txttest::construct_1()
{
    time_t now = time(nullptr);
    
    v12::CRingTextItem item(v12::PACKET_TYPES, 1024);
    const v12::TextItem* pItem =
        reinterpret_cast<const v12::TextItem*>(item.getItemPointer());
    EQ(v12::PACKET_TYPES, pItem->s_header.s_type);
    EQ(
        sizeof(v12::RingItemHeader) + sizeof(uint32_t) +
        sizeof(v12::TextItemBody),
        size_t(pItem->s_header.s_size)
    );
    EQ(sizeof(uint32_t), size_t(pItem->s_body.u_noBodyHeader.s_empty));
    const v12::TextItemBody* pBody = &(pItem->s_body.u_noBodyHeader.s_body);
    EQ(uint32_t(0), pBody->s_timeOffset);
    ASSERT(pBody->s_timestamp - now <= 1);
    EQ(uint32_t(0), pBody->s_stringCount);
    EQ(uint32_t(1), pBody->s_offsetDivisor);
    EQ(uint32_t(0), pBody->s_originalSid);
}