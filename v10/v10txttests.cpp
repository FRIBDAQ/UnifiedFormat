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
    CPPUNIT_TEST(construct_2);
    CPPUNIT_TEST(construct_3);
    
    CPPUNIT_TEST(getstrings);
    
    CPPUNIT_TEST(offset_1);
    CPPUNIT_TEST(offset_2);
    CPPUNIT_TEST(elapsed);
    CPPUNIT_TEST(divisor);
    
    CPPUNIT_TEST(timestamp_1);
    CPPUNIT_TEST(timestamp_2);
    
    CPPUNIT_TEST(originalsid);
    CPPUNIT_TEST(getbodyheader);
    CPPUNIT_TEST(typestring);
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
    
    void getstrings();
    
    void offset_1();
    void offset_2();
    void elapsed();
    void divisor();
    
    void timestamp_1();
    void timestamp_2();
    
    void originalsid();
    void getbodyheader();
    void typestring();
};

CPPUNIT_TEST_SUITE_REGISTRATION(v10txttest);

// Construct with type and strings.
void v10txttest::construct_1()
{
    std::vector<std::string> strings;
    time_t now = time(nullptr);
    v10::CRingTextItem item(v10::MONITORED_VARIABLES, strings);
    const v10::TextItem* p = reinterpret_cast<const v10::TextItem*>(item.getItemPointer());
    
    EQ(v10::MONITORED_VARIABLES, p->s_header.s_type);
    EQ(uint32_t(sizeof(v10::TextItem)), p->s_header.s_size);
    
    EQ(uint32_t(0), p->s_timeOffset);
    ASSERT(p->s_timestamp - now <= 1);
    EQ(uint32_t(0), p->s_stringCount);
}
// Construct with type and non empty strings.
void v10txttest::construct_2()
{
    std::vector<std::string> strings = {
        "String 1", "String 2", "Could have more strings"
    };
    time_t now = time(nullptr);
    v10::CRingTextItem item(v10::MONITORED_VARIABLES, strings);
    const v10::TextItem* p = reinterpret_cast<const v10::TextItem*>(item.getItemPointer());
    
    
    EQ(v10::MONITORED_VARIABLES, p->s_header.s_type);
    uint32_t expectedSize = sizeof(v10::TextItem);
    for (int i =0; i < strings.size(); i++) {
        expectedSize += strings[i].size() + 1;
    }
    EQ(expectedSize, p->s_header.s_size);
    
    EQ(uint32_t(0), p->s_timeOffset);
    ASSERT(p->s_timestamp - now <= 1);
    EQ(uint32_t(strings.size()), p->s_stringCount);
    
    const char* pS = p->s_strings;
    for (int i =0; i < strings.size(); i++) {
        std::string s = pS;
        EQ(strings[i], s);
        pS += s.size() +1;
    }
    
}
// Full construction with strings:

void v10txttest::construct_3()
{
    std::vector<std::string> strings = {
        "String 1", "String 2", "Could have more strings"
    };
    time_t now = time(nullptr);
    v10::CRingTextItem item(v10::MONITORED_VARIABLES, strings, 100, now);
    const v10::TextItem* p = reinterpret_cast<const v10::TextItem*>(item.getItemPointer());
    uint32_t expectedSize = sizeof(v10::TextItem);
    for (int i =0; i < strings.size(); i++) {
        expectedSize += strings[i].size() + 1;
    }
    
    EQ(v10::MONITORED_VARIABLES, p->s_header.s_type);
    EQ(expectedSize, p->s_header.s_size);
   
    EQ(uint32_t(100), p->s_timeOffset);
    EQ(uint32_t(now), p->s_timestamp);
    EQ(uint32_t(strings.size()), p->s_stringCount);

    const char* pS = p->s_strings;
    for (int i =0; i < strings.size(); i++) {
        std::string s = pS;
        EQ(strings[i], s);
        pS += s.size() +1;
    }
}
void v10txttest::getstrings()
{
    std::vector<std::string> strings = {
        "String 1", "String 2", "Could have more strings"
    };
    time_t now = time(nullptr);
    v10::CRingTextItem item(v10::MONITORED_VARIABLES, strings, 100, now);
    
    auto gotten = item.getStrings();
    EQ(strings.size(), gotten.size());
    for (int i = 0; i < strings.size(); i++) {
        EQ(strings[i], gotten[i]);
    }
}

void v10txttest::offset_1()
{
    std::vector<std::string> strings = {
        "String 1", "String 2", "Could have more strings"
    };
    time_t now = time(nullptr);
    v10::CRingTextItem item(v10::MONITORED_VARIABLES, strings, 100, now);
    EQ(uint32_t(100), item.getTimeOffset());
}
void v10txttest::offset_2()
{
    std::vector<std::string> strings = {
        "String 1", "String 2", "Could have more strings"
    };
    time_t now = time(nullptr);
    v10::CRingTextItem item(v10::MONITORED_VARIABLES, strings, 100, now);
    
    item.setTimeOffset(200);
    EQ(uint32_t(200), item.getTimeOffset());
}
void v10txttest::elapsed()
{
    std::vector<std::string> strings = {
        "String 1", "String 2", "Could have more strings"
    };
    time_t now = time(nullptr);
    v10::CRingTextItem item(v10::MONITORED_VARIABLES, strings, 100, now);
    
    EQ(float(100.0), item.computeElapsedTime());
}
void v10txttest::divisor()
{
    std::vector<std::string> strings = {
        "String 1", "String 2", "Could have more strings"
    };
    time_t now = time(nullptr);
    v10::CRingTextItem item(v10::MONITORED_VARIABLES, strings, 100, now);
    
    EQ(uint32_t(1), item.getTimeDivisor());
}

void v10txttest::timestamp_1()
{
    std::vector<std::string> strings = {
        "String 1", "String 2", "Could have more strings"
    };
    time_t now = time(nullptr);
    v10::CRingTextItem item(v10::MONITORED_VARIABLES, strings, 100, now);
    
    EQ(now, item.getTimestamp());
}
void v10txttest::timestamp_2()
{
    std::vector<std::string> strings = {
        "String 1", "String 2", "Could have more strings"
    };
    time_t now = time(nullptr);
    v10::CRingTextItem item(v10::MONITORED_VARIABLES, strings, 100, now);
    
    item.setTimestamp(now+100);
    EQ(now+100, item.getTimestamp());
}

void v10txttest::originalsid()
{
    std::vector<std::string> strings = {
        "String 1", "String 2", "Could have more strings"
    };
    time_t now = time(nullptr);
    v10::CRingTextItem item(v10::MONITORED_VARIABLES, strings, 100, now);
    
    EQ(uint32_t(0), item.getOriginalSourceId());
}
void v10txttest::getbodyheader()
{
    std::vector<std::string> strings = {
        "String 1", "String 2", "Could have more strings"
    };
    time_t now = time(nullptr);
    v10::CRingTextItem item(v10::MONITORED_VARIABLES, strings, 100, now);
    
    ASSERT(item.getBodyHeader() == nullptr);
}
void v10txttest::typestring()
{
    std::vector<std::string> strings = {
        "String 1", "String 2", "Could have more strings"
    };
    time_t now = time(nullptr);
    v10::CRingTextItem item(v10::MONITORED_VARIABLES, strings, 100, now);
    EQ(std::string(" Monitored Variables: "), item.typeName());
}