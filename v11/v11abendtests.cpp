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

/** @file:  v11abendtests.cpp
 *  @brief: Test v11::CAbnormalEndItem
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CAbnormalEndItem.h"
#include "DataFormat.h"

using namespace ufmt;

class v11abendtest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v11abendtest);
    CPPUNIT_TEST(construct);
    CPPUNIT_TEST(bodyhdr_1);
    CPPUNIT_TEST(bodyhdr_2);
    CPPUNIT_TEST_SUITE_END();
    
private:

public:
    void setUp() {
        
    }
    void tearDown() {
        
    }
protected:
    void construct();
    void bodyhdr_1();
    void bodyhdr_2();
};

CPPUNIT_TEST_SUITE_REGISTRATION(v11abendtest);
// constructor makes correctly shaped item.
void v11abendtest::construct()
{
    v11::CAbnormalEndItem item;
    EQ(v11::ABNORMAL_ENDRUN, item.type());
    EQ(sizeof(v11::AbnormalEndItem), size_t(item.size()));
    
    v11::pAbnormalEndItem pItem =
        reinterpret_cast<v11::pAbnormalEndItem>(item.getItemPointer());
    EQ(uint32_t(0), pItem->s_mbz);
}
// getBodyHeader returns nullptr.

void v11abendtest::bodyhdr_1()
{
    v11::CAbnormalEndItem item;
    ASSERT(item.getBodyHeader() == nullptr);
}
//  setBodyHeader throws;

void v11abendtest::bodyhdr_2()
{
    v11::CAbnormalEndItem item;
    CPPUNIT_ASSERT_THROW(
        item.setBodyHeader(0, 0, 0),
        std::logic_error
    );
}