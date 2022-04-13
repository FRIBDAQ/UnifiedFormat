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
    
    CPPUNIT_TEST(elapsed_1);    // getTimeOffset
    CPPUNIT_TEST(elapsed_2);
    CPPUNIT_TEST(elapsed_3);    // setTimeOffset
    CPPUNIT_TEST(elapsed_4);
    CPPUNIT_TEST(elapsed_5);   // computeElapsedTime
    CPPUNIT_TEST(elapsed_6);
    CPPUNIT_TEST(elapsed_7);   // getTimeDivisor
    CPPUNIT_TEST(elapsed_8);
    
    CPPUNIT_TEST(time_1);
    CPPUNIT_TEST(time_2);
    CPPUNIT_TEST(time_3);
    CPPUNIT_TEST(time_4);
    
    CPPUNIT_TEST(sid_1);
    CPPUNIT_TEST(sid_2);
    
    CPPUNIT_TEST(bodyptr_1);
    CPPUNIT_TEST(bodyptr_2);
    CPPUNIT_TEST(bodyptr_3);
    CPPUNIT_TEST(bodyptr_4);
    
    CPPUNIT_TEST(bodyhdr_1); // get
    CPPUNIT_TEST(bodyhdr_2); // set
    CPPUNIT_TEST(bodyhdr_3); // get
    CPPUNIT_TEST(bodyhdr_4); // set.
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
    
    void elapsed_1();
    void elapsed_2();
    void elapsed_3();
    void elapsed_4();
    void elapsed_5();
    void elapsed_6();
    void elapsed_7();
    void elapsed_8();
    
    void time_1();
    void time_2();
    void time_3();
    void time_4();
    
    void sid_1();
    void sid_2();
    
    void bodyptr_1();
    void bodyptr_2();
    void bodyptr_3();
    void bodyptr_4();
    
    void bodyhdr_1();
    void bodyhdr_2();
    void bodyhdr_3();
    void bodyhdr_4();
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
// Get time offst non body header.

void v11texttest::elapsed_1()
{
    std::vector<std::string> strings = {
        "one string", "two string", "three strings",
        "more"
    };
    time_t now = time(nullptr);
    v11::CRingTextItem item(
        v11::MONITORED_VARIABLES,  strings, 100, now
    );
    EQ(uint32_t(100), item.getTimeOffset());
}
// get time offset with body header.
void v11texttest::elapsed_2()
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
    EQ(uint32_t(100), item.getTimeOffset());
}
// Set elapsed time no body header.
void v11texttest::elapsed_3()
{
    std::vector<std::string> strings = {
        "one string", "two string", "three strings",
        "more"
    };
    time_t now = time(nullptr);
    v11::CRingTextItem item(
        v11::MONITORED_VARIABLES,  strings, 100, now
    );
    item.setTimeOffset(12345);
    EQ(uint32_t(12345), item.getTimeOffset());
}
// set elapsed time body header.

void v11texttest::elapsed_4()
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
    item.setTimeOffset(12345);
    EQ(uint32_t(12345), item.getTimeOffset());
}
// compute elapsed time for non-body header.
void v11texttest::elapsed_5()
{
    std::vector<std::string> strings = {
        "one string", "two string", "three strings",
        "more"
    };
    time_t now = time(nullptr);
    v11::CRingTextItem item(
        v11::MONITORED_VARIABLES,  strings, 100, now, 2
    );
    EQ(float(50), item.computeElapsedTime());
}
// compute elaposed time for body header.
void v11texttest::elapsed_6()
{
    std::vector<std::string> strings = {
        "one string", "two string", "three strings",
        "more"
    };
    time_t now = time(nullptr);
    
    v11::CRingTextItem item(
        v11::MONITORED_VARIABLES, 0x1234567890, 2, 3,
        strings, 100, now, 2
    );
    EQ(float(50), item.computeElapsedTime());
}
// Get time divisor from non body header:

void v11texttest::elapsed_7()
{
    std::vector<std::string> strings = {
        "one string", "two string", "three strings",
        "more"
    };
    time_t now = time(nullptr);
    v11::CRingTextItem item(
        v11::MONITORED_VARIABLES,  strings, 100, now, 2
    );
    EQ(uint32_t(2), item.getTimeDivisor());
}
// divisor from body header
void v11texttest::elapsed_8()
{
    std::vector<std::string> strings = {
        "one string", "two string", "three strings",
        "more"
    };
    time_t now = time(nullptr);
    
    v11::CRingTextItem item(
        v11::MONITORED_VARIABLES, 0x1234567890, 2, 3,
        strings, 100, now, 2
    );
    EQ(uint32_t(2), item.getTimeDivisor());
}
// get timestamp from non body header.

void v11texttest::time_1()
{
    std::vector<std::string> strings = {
        "one string", "two string", "three strings",
        "more"
    };
    time_t now = time(nullptr);
    v11::CRingTextItem item(
        v11::MONITORED_VARIABLES,  strings, 100, now, 2
    );
    EQ(now, item.getTimestamp());
}
void v11texttest::time_2()
{
    std::vector<std::string> strings = {
        "one string", "two string", "three strings",
        "more"
    };
    time_t now = time(nullptr);
    
    v11::CRingTextItem item(
        v11::MONITORED_VARIABLES, 0x1234567890, 2, 3,
        strings, 100, now, 2
    );
    EQ(now, item.getTimestamp());
}
// set timestamp for non body header.

void v11texttest::time_3()
{
    std::vector<std::string> strings = {
        "one string", "two string", "three strings",
        "more"
    };
    time_t now = time(nullptr);
    v11::CRingTextItem item(
        v11::MONITORED_VARIABLES,  strings, 100, now, 2
    );
    item.setTimestamp(now+10);
    EQ(now+10, item.getTimestamp());
}
// set timestamp for body header.
void v11texttest::time_4()
{
    std::vector<std::string> strings = {
        "one string", "two string", "three strings",
        "more"
    };
    time_t now = time(nullptr);
    
    v11::CRingTextItem item(
        v11::MONITORED_VARIABLES, 0x1234567890, 2, 3,
        strings, 100, now, 2
    );
    item.setTimestamp(now+10);
    EQ(now+10, item.getTimestamp());
}
// original sid from no body header gives 0.

void v11texttest::sid_1()
{
    std::vector<std::string> strings = {
        "one string", "two string", "three strings",
        "more"
    };
    time_t now = time(nullptr);
    v11::CRingTextItem item(
        v11::MONITORED_VARIABLES,  strings, 100, now, 2
    );
    EQ(uint32_t(0), item.getOriginalSourceId());
}
// since there's no original source id in v11, we get the sid
// from the body header if there is one:

void v11texttest::sid_2()
{
    std::vector<std::string> strings = {
        "one string", "two string", "three strings",
        "more"
    };
    time_t now = time(nullptr);
    
    v11::CRingTextItem item(
        v11::MONITORED_VARIABLES, 0x1234567890, 2, 3,
        strings, 100, now, 2
    );
    EQ(uint32_t(2), item.getOriginalSourceId());
}
// body pointer for non body header.
void v11texttest::bodyptr_1()
{
    std::vector<std::string> strings = {
        "one string", "two string", "three strings",
        "more"
    };
    time_t now = time(nullptr);
    v11::CRingTextItem item(
        v11::MONITORED_VARIABLES,  strings, 100, now, 2
    );
    v11::pTextItemBody pBody =
        reinterpret_cast<v11::pTextItemBody>(item.getBodyPointer());
    v11::pTextItem pItem =
        reinterpret_cast<v11::pTextItem>(item.getItemPointer());
    EQ(&(pItem->s_body.u_noBodyHeader.s_body), pBody);
}
// const body pointer for non body header.
void v11texttest::bodyptr_2()
{
    std::vector<std::string> strings = {
        "one string", "two string", "three strings",
        "more"
    };
    time_t now = time(nullptr);
    v11::CRingTextItem item(
        v11::MONITORED_VARIABLES,  strings, 100, now, 2
    );
    const v11::TextItemBody* pBody =
        reinterpret_cast<const v11::TextItemBody*>(item.getBodyPointer());
    const v11::TextItem* pItem =
        reinterpret_cast<const v11::TextItem*>(item.getItemPointer());
    EQ(&(pItem->s_body.u_noBodyHeader.s_body), pBody);
}
// bodypointer for body header item.
void v11texttest::bodyptr_3()
{
    std::vector<std::string> strings = {
        "one string", "two string", "three strings",
        "more"
    };
    time_t now = time(nullptr);
    
    v11::CRingTextItem item(
        v11::MONITORED_VARIABLES, 0x1234567890, 2, 3,
        strings, 100, now, 2
    );
    v11::pTextItemBody pBody =
        reinterpret_cast<v11::pTextItemBody>(item.getBodyPointer());
    v11::pTextItem pItem =
        reinterpret_cast<v11::pTextItem>(item.getItemPointer());
    EQ(&(pItem->s_body.u_hasBodyHeader.s_body), pBody);
}

// const body pointer for body header item.
void v11texttest::bodyptr_4()
{
    std::vector<std::string> strings = {
        "one string", "two string", "three strings",
        "more"
    };
    time_t now = time(nullptr);
    
    v11::CRingTextItem item(
        v11::MONITORED_VARIABLES, 0x1234567890, 2, 3,
        strings, 100, now, 2
    );
    const v11::TextItemBody* pBody =
        reinterpret_cast<const v11::TextItemBody*>(item.getBodyPointer());
    const v11::TextItem* pItem =
        reinterpret_cast<const v11::TextItem*>(item.getItemPointer());
    EQ(&(pItem->s_body.u_hasBodyHeader.s_body), pBody);
}
// Get body header from non body header item is nullptr.

void v11texttest::bodyhdr_1()
{
    std::vector<std::string> strings = {
        "one string", "two string", "three strings",
        "more"
    };
    time_t now = time(nullptr);
    v11::CRingTextItem item(
        v11::MONITORED_VARIABLES,  strings, 100, now, 2
    );
    ASSERT(item.getBodyHeader() == nullptr);   // nullptr is weird - does not work in EQ
}
// With body header this gives a pointer to it:
void v11texttest::bodyhdr_2()
{
    std::vector<std::string> strings = {
        "one string", "two string", "three strings",
        "more"
    };
    time_t now = time(nullptr);
    
    v11::CRingTextItem item(
        v11::MONITORED_VARIABLES, 0x1234567890, 2, 3,
        strings, 100, now, 2
    );
    v11::pTextItem pBody =
        reinterpret_cast<v11::pTextItem>(item.getItemPointer());
    v11::pBodyHeader p  = reinterpret_cast<v11::pBodyHeader>(item.getBodyHeader());
    EQ(&(pBody->s_body.u_hasBodyHeader.s_bodyHeader), p);
}
// Set body header creates one for the non body header item.

void v11texttest::bodyhdr_3()
{
    std::vector<std::string> strings = {
        "one string", "two string", "three strings",
        "more"
    };
    time_t now = time(nullptr);
    v11::CRingTextItem item(
        v11::MONITORED_VARIABLES,  strings, 100, now, 2
    );
    size_t originalSize = item.size();
    item.setBodyHeader(0x1234567890, 1, 2);
    size_t afterSize = item.size();
    EQ(
       originalSize + sizeof(v11::BodyHeader) - sizeof(uint32_t),
       afterSize
    );
    v11::pTextItem pBody = reinterpret_cast<v11::pTextItem>(item.getItemPointer());
    v11::pBodyHeader p  = reinterpret_cast<v11::pBodyHeader>(item.getBodyHeader());
    EQ(&(pBody->s_body.u_hasBodyHeader.s_bodyHeader), p);
}
// just replaces contents of any existing boydy header.
void v11texttest::bodyhdr_4()
{
    std::vector<std::string> strings = {
        "one string", "two string", "three strings",
        "more"
    };
    time_t now = time(nullptr);
    
    v11::CRingTextItem item(
        v11::MONITORED_VARIABLES, 0x1234567890, 2, 3,
        strings, 100, now, 2
    );
    size_t originalSize = item.size();
    item.setBodyHeader(0x555555555, 3, 2);
    size_t afterSize = item.size();
    EQ(originalSize, afterSize);
    
    v11::pTextItem pBody = reinterpret_cast<v11::pTextItem>(item.getItemPointer());
    v11::pBodyHeader p  = reinterpret_cast<v11::pBodyHeader>(item.getBodyHeader());
    EQ(&(pBody->s_body.u_hasBodyHeader.s_bodyHeader), p);
}
