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

/** @file:  v11glomtests.cpp
 *  @brief: Tests for v11::CGlomParameters
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CGlomParameters.h"
#include "DataFormat.h"


class v11glomtest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v11glomtest);
    CPPUNIT_TEST(construct);
    CPPUNIT_TEST(getticks);
    CPPUNIT_TEST(isbuilding);
    CPPUNIT_TEST(policy);
    
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
    void getticks();
    void isbuilding();
    void policy();
    void bodyhdr_1();
    void bodyhdr_2();
    
};

CPPUNIT_TEST_SUITE_REGISTRATION(v11glomtest);

void v11glomtest::construct()
{
    v11::CGlomParameters item(100, true, CGlomParameters::last);
    EQ(v11::EVB_GLOM_INFO, item.type());
    EQ(sizeof(v11::GlomParameters), size_t(item.size()));
    const v11::GlomParameters* pItem =
        reinterpret_cast<const v11::GlomParameters*>(item.getItemPointer());
    EQ(uint32_t(0), pItem->s_mbz);
    EQ(uint64_t(100), pItem->s_coincidenceTicks);
    ASSERT(pItem->s_isBuilding != 0);
    EQ(v11::GLOM_TIMESTAMP_LAST, pItem->s_timestampPolicy);
    
}
void v11glomtest::getticks()
{
    v11::CGlomParameters item(100, true, CGlomParameters::last);
    EQ(uint64_t(100), item.coincidenceTicks());
}
void v11glomtest::isbuilding()
{
    v11::CGlomParameters item(100, true, CGlomParameters::last);
    ASSERT(item.isBuilding());
}
void v11glomtest::policy()
{
    v11::CGlomParameters item(100, true, CGlomParameters::last);
    EQ(CGlomParameters::last, item.timestampPolicy());
}

void v11glomtest::bodyhdr_1()
{
    v11::CGlomParameters item(100, true, CGlomParameters::last);
    ASSERT(item.getBodyHeader() == nullptr);
}
void v11glomtest::bodyhdr_2()
{
    v11::CGlomParameters item(100, true, CGlomParameters::last);
    CPPUNIT_ASSERT_THROW(
        item.setBodyHeader(200, 1, 1),
        std::logic_error
    );
}