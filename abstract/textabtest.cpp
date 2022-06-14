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

/** @file:  textabtest.cpp
 *  @brief:  Tests abstract text ring item class.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CRingTextItem.h"
#include "DataFormat.h"
#include <time.h>
#include <string.h>
#include <stdexcept>

class textabtest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(textabtest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    
    CPPUNIT_TEST(getstrings);
    
    CPPUNIT_TEST(offset_1);
    CPPUNIT_TEST(offset_2);
    
    CPPUNIT_TEST(elapsed_1);
    CPPUNIT_TEST(elapsed_2);
    
    CPPUNIT_TEST(divisor_1);
    CPPUNIT_TEST(divisor_2);
    
    CPPUNIT_TEST(stamp_1);
    CPPUNIT_TEST(stamp_2);
    
    CPPUNIT_TEST(sid);
    
    CPPUNIT_TEST(typename_1);
    CPPUNIT_TEST(typename_2);
    
    CPPUNIT_TEST(bodyhdr_1);
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
    void getstrings();
    
    void offset_1();
    void offset_2();
    
    void elapsed_1();
    void elapsed_2();
    
    void divisor_1();
    void divisor_2();
    
    void stamp_1();
    void stamp_2();
    void sid();
    
    void typename_1();
    void typename_2();
    
    void bodyhdr_1();
};

CPPUNIT_TEST_SUITE_REGISTRATION(textabtest);
// Minimal constructor.
void textabtest::construct_1()
{
    CRingTextItem* pItem(0);
    std::vector<std::string> strs={
        "one string", "two string", "three string", "four"
        "five string", "six string" , "seven string", "more"
    };
    try {
        CPPUNIT_ASSERT_NO_THROW(
            pItem = new CRingTextItem(PACKET_TYPES, strs)
        );
        time_t now = time(nullptr);        
        EQ(PACKET_TYPES, pItem->type());
        const TextItemBody* pBody =
            reinterpret_cast<const TextItemBody*>(pItem->getBodyPointer());
        EQ(uint32_t(0), pBody->s_timeOffset);
        ASSERT(now - pBody->s_timestamp <= 1);
        EQ(strs.size(), size_t(pBody->s_stringCount));
        EQ(uint32_t(1), pBody->s_offsetDivisor);
        EQ(uint32_t(0), pBody->s_originalSid);
        const char* p = pBody->s_strings;
        for (int i = 0; i < strs.size(); i++) {
            EQ(strs[i], std::string(p));
            p += strlen(p) + 1;
        }
    }
    catch (...) {               // Assertion failures are exceptions.
        delete pItem;           // Delete the item and rethrow it.
        throw;
    }
    delete pItem;
}
// Detailed consruction

void textabtest::construct_2()
{
CRingTextItem* pItem(0);
    std::vector<std::string> strs={
        "one string", "two string", "three string", "four"
        "five string", "six string" , "seven string", "more"
    };
    time_t now = time(nullptr);        
    try {
        CPPUNIT_ASSERT_NO_THROW(
            pItem = new CRingTextItem(PACKET_TYPES, strs, 10, now )
        );
        
        EQ(PACKET_TYPES, pItem->type());
        const TextItemBody* pBody =
            reinterpret_cast<const TextItemBody*>(pItem->getBodyPointer());
        EQ(uint32_t(10), pBody->s_timeOffset);
        EQ(now ,time_t(pBody->s_timestamp));
        EQ(strs.size(), size_t(pBody->s_stringCount));
        EQ(uint32_t(1), pBody->s_offsetDivisor);
        EQ(uint32_t(0), pBody->s_originalSid);
        const char* p = pBody->s_strings;
        for (int i = 0; i < strs.size(); i++) {
            EQ(strs[i], std::string(p));
            p += strlen(p) + 1;
        }
    }
    catch (...) {               // Assertion failures are exceptions.
        delete pItem;           // Delete the item and rethrow it.
        throw;
    }
    delete pItem;
}
// can recover the string vector:

void textabtest::getstrings()
{
    std::vector<std::string> strs={
        "one string", "two string", "three string", "four"
        "five string", "six string" , "seven string", "more"
    };
    time_t now = time(nullptr);
    CRingTextItem item(PACKET_TYPES, strs, 10, now );
    
    auto actual = item.getStrings();
    EQ(strs.size(), actual.size());
    for (int i =0; i < strs.size(); i++) {
        EQ(strs[i], actual[i]);
    }
}
// can get the time offset:

void textabtest::offset_1()
{
    std::vector<std::string> strs={
        "one string", "two string", "three string", "four"
        "five string", "six string" , "seven string", "more"
    };
    time_t now = time(nullptr);
    CRingTextItem item(PACKET_TYPES, strs, 10, now );
    
    EQ(uint32_t(10), item.getTimeOffset());
}
// Can modify time offset:

void textabtest::offset_2()
{
    std::vector<std::string> strs={
        "one string", "two string", "three string", "four"
        "five string", "six string" , "seven string", "more"
    };
    time_t now = time(nullptr);
    CRingTextItem item(PACKET_TYPES, strs, 10, now );
    item.setTimeOffset(32);
    EQ(uint32_t(32), item.getTimeOffset());
}
// elapsed time for default divisor:

void textabtest::elapsed_1()
{
    std::vector<std::string> strs={
        "one string", "two string", "three string", "four"
        "five string", "six string" , "seven string", "more"
    };
    time_t now = time(nullptr);
    CRingTextItem item(PACKET_TYPES, strs, 10, now );
    
    EQ(float(10.0), item.computeElapsedTime());
}
// Elapsed time for a non-default divisor.

void textabtest::elapsed_2()
{
    std::vector<std::string> strs={
        "one string", "two string", "three string", "four"
        "five string", "six string" , "seven string", "more"
    };
    time_t now = time(nullptr);
    CRingTextItem item(PACKET_TYPES, strs, 10, now, 2 );
    EQ(float(5.0), item.computeElapsedTime());
}
// Default divisor:
void textabtest::divisor_1()
{
    std::vector<std::string> strs={
        "one string", "two string", "three string", "four"
        "five string", "six string" , "seven string", "more"
    };
    time_t now = time(nullptr);
    CRingTextItem item(PACKET_TYPES, strs, 10, now );
    EQ(uint32_t(1), item.getTimeDivisor());
    
}
void textabtest::divisor_2()
{
    std::vector<std::string> strs={
        "one string", "two string", "three string", "four"
        "five string", "six string" , "seven string", "more"
    };
    time_t now = time(nullptr);
    CRingTextItem item(PACKET_TYPES, strs, 10, now, 5 );
    EQ(uint32_t(5), item.getTimeDivisor());
}
// Can fetch accurate timestamp.
void textabtest::stamp_1()
{
    std::vector<std::string> strs={
        "one string", "two string", "three string", "four"
        "five string", "six string" , "seven string", "more"
    };
    time_t now = time(nullptr);
    CRingTextItem item(PACKET_TYPES, strs, 10, now );
    EQ(now, item.getTimestamp());
}
// Can set timestamp post construction.

void textabtest::stamp_2()
{
    std::vector<std::string> strs={
        "one string", "two string", "three string", "four"
        "five string", "six string" , "seven string", "more"
    };
    time_t now = time(nullptr);
    CRingTextItem item(PACKET_TYPES, strs, 10, now );
    item.setTimestamp(now+10);
    EQ(now+10, item.getTimestamp());
}
// Can fetch source id -- always zero in abstract.

void textabtest::sid()
{
    std::vector<std::string> strs={
        "one string", "two string", "three string", "four"
        "five string", "six string" , "seven string", "more"
    };
    time_t now = time(nullptr);
    CRingTextItem item(PACKET_TYPES, strs, 10, now );
    
    EQ(uint32_t(0), item.getOriginalSourceId());
}
// PACKET_TYPE -> "Packt types"
void textabtest::typename_1()
{
    std::vector<std::string> strs={
        "one string", "two string", "three string", "four"
        "five string", "six string" , "seven string", "more"
    };
    time_t now = time(nullptr);
    CRingTextItem item(PACKET_TYPES, strs, 10, now );
    
    EQ(std::string("Packet types"), item.typeName());
}
// MONITORED_VARIABLES => Monitored Variables

void textabtest::typename_2()
{
    std::vector<std::string> strs={
        "one string", "two string", "three string", "four"
        "five string", "six string" , "seven string", "more"
    };
    time_t now = time(nullptr);
    CRingTextItem item(MONITORED_VARIABLES, strs, 10, now );
    EQ(std::string("Monitored Variables"), item.typeName());
}
// Body header stuff throws logic_error:

void textabtest::bodyhdr_1()
{
    std::vector<std::string> strs={
        "one string", "two string", "three string", "four"
        "five string", "six string" , "seven string", "more"
    };
    time_t now = time(nullptr);
    CRingTextItem item(MONITORED_VARIABLES, strs, 10, now );
    
    ASSERT(
        item.getBodyHeader() == nullptr
    );
}
