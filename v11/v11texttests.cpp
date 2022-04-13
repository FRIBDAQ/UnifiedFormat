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

/** @file:  v11texttests.cpp
 *  @brief:  Test v11:CRingTextItem.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CRingTextItem.h"
#include "DataFormat.h"
#include <string.h>
#include <stdexcept>

static size_t
sizeStrings(const std::vector<std::string>& strings)
{
    size_t result(0);
    for (int i =0; i < strings.size(); i++) {
        result += strings[i].size() + 1;
    }
    
    return result;
}

class v11texttest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v11texttest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    CPPUNIT_TEST(construct_3);
    CPPUNIT_TEST(construct_4);
    
    CPPUNIT_TEST(getstrings_1);
    CPPUNIT_TEST(getstrings_2);
    CPPUNIT_TEST_SUITE_END();
    
private:

public:
    void setUp() {
        
    }
    void tearDown() {
        
    }
protected:
    void construct_1();
    void construct_2();
    void construct_3();
    void construct_4();
    
    void getstrings_1();
    void getstrings_2();
};

CPPUNIT_TEST_SUITE_REGISTRATION(v11texttest);

// non body header

void v11texttest::construct_1()
{
    std::vector<std::string> strings = {
        "one string", "two string", "three strings",
        "more"
    };
    time_t now = time(nullptr);
    v11::CRingTextItem item(v11::MONITORED_VARIABLES, strings,
    100, now);
    
    const v11::TextItem* pItem =
        reinterpret_cast<const v11::TextItem*>(item.getItemPointer());
    EQ(v11::MONITORED_VARIABLES, pItem->s_header.s_type);
    EQ(
       sizeof(v11::RingItemHeader) + sizeof(uint32_t) +
       sizeof(v11::TextItemBody) + sizeStrings(strings),
       size_t(pItem->s_header.s_size)
    );
    EQ(uint32_t(0), pItem->s_body.u_noBodyHeader.s_mbz);
    const v11::TextItemBody* pBody =
        &(pItem->s_body.u_noBodyHeader.s_body);
    
    EQ(uint32_t(100), pBody->s_timeOffset);
    EQ(uint32_t(now), pBody->s_timestamp);
    EQ(uint32_t(strings.size()), pBody->s_stringCount);
    EQ(uint32_t(1), pBody->s_offsetDivisor);
    const char* p = pBody->s_strings;
    for (int i =0; i < pBody->s_stringCount; i++) {
        EQ(strings[i], std::string(p));
        p += strlen(p) + 1;
    }
    
}
// body header:

void v11texttest::construct_2()
{
    std::vector<std::string> strings = {
        "one string", "two string", "three strings",
        "more"
    };
    time_t now = time(nullptr);
    v11::CRingTextItem item(
        v11::MONITORED_VARIABLES, 0x1234567890, 2, 3,
        strings,
        100, now
    );
    const v11::TextItem* pItem =
        reinterpret_cast<const v11::TextItem*>(item.getItemPointer());
    EQ(v11::MONITORED_VARIABLES, pItem->s_header.s_type);
    EQ(
       sizeof(v11::RingItemHeader) + sizeof(v11::BodyHeader) +
       sizeof(v11::TextItemBody) + sizeStrings(strings),
       size_t(pItem->s_header.s_size)
    );
    EQ(uint32_t(sizeof(v11::BodyHeader)), pItem->s_body.u_noBodyHeader.s_mbz);
    
    const v11::BodyHeader* pBodyhdr =
        &(pItem->s_body.u_hasBodyHeader.s_bodyHeader);
    EQ(uint32_t(sizeof(v11::BodyHeader)), pBodyhdr->s_size);
    EQ(uint64_t(0x1234567890), pBodyhdr->s_timestamp);
    EQ(uint32_t(2), pBodyhdr->s_sourceId);
    EQ(uint32_t(3), pBodyhdr->s_barrier);
    
    const v11::TextItemBody* pBody =
        &(pItem->s_body.u_hasBodyHeader.s_body);
    
      EQ(uint32_t(100), pBody->s_timeOffset);
    EQ(uint32_t(now), pBody->s_timestamp);
    EQ(uint32_t(strings.size()), pBody->s_stringCount);
    EQ(uint32_t(1), pBody->s_offsetDivisor);
    const char* p = pBody->s_strings;
    for (int i =0; i < pBody->s_stringCount; i++) {
        EQ(strings[i], std::string(p));
        p += strlen(p) + 1;
    } 
}
// no body header bad item type throws

void v11texttest::construct_3()
{
    std::vector<std::string> strings = {
        "one string", "two string", "three strings",
        "more"
    };
    time_t now = time(nullptr);
    CPPUNIT_ASSERT_THROW(
        v11::CRingTextItem item(v11::FIRST_USER_ITEM_CODE, strings,
            100, now),
        std::invalid_argument
    );
}
void v11texttest::construct_4()
{
    std::vector<std::string> strings = {
        "one string", "two string", "three strings",
        "more"
    };
    time_t now = time(nullptr);
    CPPUNIT_ASSERT_THROW(
        v11::CRingTextItem item(
            v11::FIRST_USER_ITEM_CODE, 0x1234567890, 2, 3,
            strings,
            100, now
        ),
        std::invalid_argument
    );
}
// get strings from non body header item.

void v11texttest::getstrings_1()
{
     std::vector<std::string> strings = {
        "one string", "two string", "three strings",
        "more"
    };
    time_t now = time(nullptr);
    v11::CRingTextItem item(
        v11::MONITORED_VARIABLES,  strings, 100, now
    );
    auto result = item.getStrings();
    EQ(strings.size(), result.size());
    for (int i = 0; i < strings.size(); i++) {
        EQ(strings[i], result[i]);
    }
}
// get strings from body header item.
void v11texttest::getstrings_2()
{
    std::vector<std::string> strings = {
        "one string", "two string", "three strings",
        "more"
    };
    time_t now = time(nullptr);
    
    v11::CRingTextItem item(
        v11::MONITORED_VARIABLES, 0x1234567890, 2, 3,
        strings, 100, now
    );
    
    auto result = item.getStrings();
    EQ(strings.size(), result.size());
    for (int i = 0; i < strings.size(); i++) {
        EQ(strings[i], result[i]);
    }

}