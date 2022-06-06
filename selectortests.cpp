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

/** @file:  selectortests.cpp
 *  @brief: Test code that selects ring item factories.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "NSCLDAQFormatFactorySelector.h"
#include <abstract/RingItemFactoryBase.h>
#include <abstract/CDataFormatItem.h>
#include <v11/RingItemFactory.h>
#include <v12/RingItemFactory.h>

#include <memory>


class seltest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(seltest);
    CPPUNIT_TEST(v10_1);
    CPPUNIT_TEST(v11_1);
    CPPUNIT_TEST(v12_1);
    
    CPPUNIT_TEST(v11_2);
    CPPUNIT_TEST(v12_2);
    CPPUNIT_TEST_SUITE_END();
    
private:

public:
    void setUp() {
        FormatSelector::clearCache();    /// empty factory cache.
    }
    void tearDown() {
        
    }
protected:
    void v10_1();
    void v11_1();
    void v12_1();
    void v11_2();
    void v12_2();
};

CPPUNIT_TEST_SUITE_REGISTRATION(seltest);

// A distinguishing characteristic of v10 factories is that can't make
// data format items:

void seltest::v10_1()
{
    auto& fact = FormatSelector::selectFactory(::FormatSelector::v10);
    ASSERT(fact.makeDataFormatItem() == nullptr);
}
// v11 - format item major version is 11.

void seltest::v11_1()
{
    auto& fact = FormatSelector::selectFactory(::FormatSelector::v11);
    std::unique_ptr<::CDataFormatItem> item(fact.makeDataFormatItem());
    EQ(uint16_t(11), item->getMajor());
}
// v12 format item major is 12
void seltest::v12_1()
{
    auto& fact = FormatSelector::selectFactory(::FormatSelector::v12);
    std::unique_ptr<::CDataFormatItem> item(fact.makeDataFormatItem());
    EQ(uint16_t(12), item->getMajor());
}

// construct a v11 factory from a format item.

void seltest::v11_2()
{
    v11::RingItemFactory fact;
    std::unique_ptr<::CDataFormatItem> pItem(fact.makeDataFormatItem());
    
    auto& selfact = FormatSelector::selectFactory(*pItem);
    std::unique_ptr<::CDataFormatItem> item(selfact.makeDataFormatItem());
    EQ(uint16_t(11), item->getMajor());
}
// similarly from v12:

void seltest::v12_2()
{
    v12::RingItemFactory fact;
    std::unique_ptr<::CDataFormatItem> pItem(fact.makeDataFormatItem());
    
    auto& selfact = FormatSelector::selectFactory(*pItem);
    std::unique_ptr<::CDataFormatItem> item(selfact.makeDataFormatItem());
    EQ(uint16_t(12), item->getMajor());
}