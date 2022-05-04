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

/** @file:  v12glomtests.cpp
 *  @brief: Tests for  v12::CGlomParameters
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CGlomParameters.h"
#include "DataFormat.h"
#include <stdexcept>

class v12glomtest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v12glomtest);
    CPPUNIT_TEST(construct);
    CPPUNIT_TEST(getticks);
    CPPUNIT_TEST(isBuilding);
    CPPUNIT_TEST(tsPolicy);
    CPPUNIT_TEST(getbhdr);
    CPPUNIT_TEST(setbhdr);
    CPPUNIT_TEST_SUITE_END();
    
private:

public:
    void setUp() {
        
    }
    void tearDown() {
        
    }
protected:
    void construct();
    void getticks();
    void isBuilding();
    void tsPolicy();
    void getbhdr();
    void setbhdr();
};

CPPUNIT_TEST_SUITE_REGISTRATION(v12glomtest);

void v12glomtest::construct()
{
    v12::CGlomParameters item(0x1234, true, CGlomParameters::average);
    const v12::GlomParameters* pItem =
        reinterpret_cast<const v12::GlomParameters*>(item.getItemPointer());
    EQ(v12::EVB_GLOM_INFO, pItem->s_header.s_type);
    EQ(sizeof(v12::GlomParameters), size_t(pItem->s_header.s_size));
    EQ(uint64_t(0x1234), pItem->s_coincidenceTicks);
    EQ(uint16_t(0xffff), pItem->s_isBuilding);
    EQ(v12::GLOM_TIMESTAMP_AVERAGE, pItem->s_timestampPolicy);
    
}
void v12glomtest::getticks()
{
    v12::CGlomParameters item(0x1234, true, CGlomParameters::average);
    EQ(uint64_t(0x1234), item.coincidenceTicks());
}
void v12glomtest::isBuilding()
{
    v12::CGlomParameters item(0x1234, true, CGlomParameters::average);
    ASSERT(item.isBuilding());
}
void v12glomtest::tsPolicy()
{
    v12::CGlomParameters item(0x1234, true, CGlomParameters::average);
    EQ(CGlomParameters::average, item.timestampPolicy());
}
void v12glomtest::getbhdr()
{
    v12::CGlomParameters item(0x1234, true, CGlomParameters::average);
    ASSERT(nullptr == item.getBodyHeader());
}

void v12glomtest::setbhdr()
{
    v12::CGlomParameters item(0x1234, true, CGlomParameters::average);
    CPPUNIT_ASSERT_THROW(
        item.setBodyHeader(0,0,0),
        std::logic_error
    );
}