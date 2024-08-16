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

/** @file:  v12fmttests.cpp
 *  @brief: Test v12::CDataFormatItem
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CDataFormatItem.h"
#include "DataFormat.h"
#include <stdexcept>

using namespace ufmt;

class v12fmttest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v12fmttest);
    CPPUNIT_TEST(construct);
    CPPUNIT_TEST(major);
    CPPUNIT_TEST(minor);
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
    
    void major();
    void minor();
    void setbodyhdr();
    void getbodyhdr();
};

CPPUNIT_TEST_SUITE_REGISTRATION(v12fmttest);

void v12fmttest::construct()
{
    v12::CDataFormatItem item;
    const v12::DataFormat* pItem =
        reinterpret_cast<const v12::DataFormat*>(item.getItemPointer());
    EQ(v12::RING_FORMAT, pItem->s_header.s_type);
    EQ(sizeof(v12::DataFormat), size_t(pItem->s_header.s_size));
    EQ(v12::FORMAT_MAJOR, pItem->s_majorVersion);
    EQ(v12::FORMAT_MINOR, pItem->s_minorVersion);
    
    EQ(v12::RING_FORMAT, item.type());
    EQ(sizeof(v12::DataFormat), size_t(item.size()));
}
// get major versionl

void v12fmttest::major()
{
    v12::CDataFormatItem item;
    EQ(v12::FORMAT_MAJOR, item.getMajor());
}
// get minor version.
void v12fmttest::minor()
{
    v12::CDataFormatItem item;
    EQ(v12::FORMAT_MINOR, item.getMinor());
}
// get body header is null:

void v12fmttest::getbodyhdr()
{
    v12::CDataFormatItem item;
    ASSERT(nullptr == item.getBodyHeader());
}
// set body header throws:
void v12fmttest::setbodyhdr()
{
    v12::CDataFormatItem item;
    CPPUNIT_ASSERT_THROW(
        item.setBodyHeader(0,0,0),
        std::logic_error
    );
}