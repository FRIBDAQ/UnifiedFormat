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

/** @file:  v11dfmttests.cpp
 *  @brief: V11 Data format Item tests.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CDataFormatItem.h"
#include "DataFormat.h"


class v11dfmttest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v11dfmttest);
    CPPUNIT_TEST(construct);
    CPPUNIT_TEST(major);
    CPPUNIT_TEST(minor);
    
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
    void major();
    void minor();
    void bodyhdr_1();
    void bodyhdr_2();
};

CPPUNIT_TEST_SUITE_REGISTRATION(v11dfmttest);
// Construction results in the item we expect.
void v11dfmttest::construct()
{
    v11::CDataFormatItem item;
    EQ(v11::RING_FORMAT, item.type());
    EQ(sizeof(v11::DataFormat), size_t(item.size()));
    
    const v11::DataFormat* pItem =
        reinterpret_cast<const v11::DataFormat*>(item.getItemPointer());
    EQ(uint32_t(0), pItem->s_mbz);
    EQ(v11::FORMAT_MAJOR, pItem->s_majorVersion);
    EQ(v11::FORMAT_MINOR, pItem->s_minorVersion);
}
// get major and minor versions:

void v11dfmttest::major()
{
    v11::CDataFormatItem item;
    EQ(v11::FORMAT_MAJOR, item.getMajor());
}
void v11dfmttest::minor()
{
    v11::CDataFormatItem item;
    EQ(v11::FORMAT_MINOR, item.getMinor());
}
// format items don't have a body header:

void v11dfmttest::bodyhdr_1()
{
    v11::CDataFormatItem item;
    ASSERT(item.getBodyHeader() == nullptr);
}
void v11dfmttest::bodyhdr_2()
{
    v11::CDataFormatItem item;
    CPPUNIT_ASSERT_THROW(
        item.setBodyHeader(0, 0),
        std::logic_error
    );
}