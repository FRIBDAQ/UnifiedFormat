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

/** @file:  v12abend.cpp
 *  @brief: Test v12::CAbnormalEnd class.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CAbnormalEndItem.h"
#include "DataFormat.h"
#include <stdexcept>

class v12abendtest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v12abendtest);
    CPPUNIT_TEST(construct);
    CPPUNIT_TEST(getbodyhdr);
    CPPUNIT_TEST(setbodyhdr);
    CPPUNIT_TEST_SUITE_END();
    
private:

public:
    void setUp() {
        
    }
    void tearDown() {
        
    }
protected:
    void construct();
    void getbodyhdr();
    void setbodyhdr();
};

CPPUNIT_TEST_SUITE_REGISTRATION(v12abendtest);

void v12abendtest::construct()
{
    v12::CAbnormalEndItem item;
    EQ(v12::ABNORMAL_ENDRUN, item.type());
    EQ(sizeof(v12::AbnormalEndItem), size_t(item.size()));
    
    const v12::AbnormalEndItem* pItem =
        reinterpret_cast<const v12::AbnormalEndItem*>(item.getItemPointer());
    EQ(sizeof(uint32_t), size_t(pItem->s_empty));
    EQ(v12::ABNORMAL_ENDRUN, pItem->s_header.s_type);
    EQ(sizeof(v12::AbnormalEndItem), size_t(pItem->s_header.s_size));
       
}
// getting body header gives nullptr.;
void v12abendtest::getbodyhdr()
{
    v12::CAbnormalEndItem item;
    ASSERT(nullptr == item.getBodyHeader());
}
// Setting body header throws logic error.
void v12abendtest::setbodyhdr()
{
    v12::CAbnormalEndItem item;
    CPPUNIT_ASSERT_THROW(
        item.setBodyHeader(0,0,0),
        std::logic_error
    );
}