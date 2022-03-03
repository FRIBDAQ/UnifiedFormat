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

/** @file:  v10txttests.cpp
 *  @brief: Tests for v10::CRingTextItem
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CRingTextItem.h"
#include "DataFormat.h"

#include <time.h>
#include <string>
#include <vector>

class v10txttest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v10txttest);
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

CPPUNIT_TEST_SUITE_REGISTRATION(v10txttest);

// Construct with type and strings.
void v10txttest::construct_1()
{
    std::vector<std::string> strings;
    time_t now = time(nullptr);
    v10::CRingTextItem item(v10::MONITORED_VARIABLES, strings);
    const v10::TextItem* p = reinterpret_cast<const v10::TextItem*>(item.getItemPointer());
    
    EQ(MONITORED_VARIABLES, p->s_header.s_type);
    EQ(uint32_t(sizeof(v10::TextItem)), p->s_header.s_size);
    
    EQ(uint32_t(0), p->s_timeOffset);
    ASSERT(p->s_timeOffset - now <= 1);
    EQ(uint32_t(0), p->s_stringCount);
}