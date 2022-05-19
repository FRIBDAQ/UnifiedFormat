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

/** @file:  v12texttests
 *  @brief: Test suite for v12::CRingTextItem class and methods.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CRingTextItem.h"
#include "DataFormat.h"
#include <time.h>


static std::vector<std::string> theStrings = {
    "one string", "two string", "three strings", "four",
    "five strings", "six strings", "seven strings", "no more"
};

static size_t stringLength(const std::vector<std::string>& s)
{
    size_t result(0);
    for (auto& s : theStrings ) {
        result += s.size() + 1;    // +1 for the null terminator.
    }
    
    return result;
}
class v12txttest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v12txttest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    CPPUNIT_TEST(construct_3);
    CPPUNIT_TEST(construct_4);
    CPPUNIT_TEST(construct_5);
    CPPUNIT_TEST(construct_6);
    CPPUNIT_TEST(construct_7);
    CPPUNIT_TEST(construct_8);
    
    CPPUNIT_TEST(getstrings_1);
    CPPUNIT_TEST(getstrings_2);
    
    CPPUNIT_TEST(getoffset_1);
    CPPUNIT_TEST(getoffset_2);
    CPPUNIT_TEST(setoffset_1);
    CPPUNIT_TEST(setoffset_2);
    CPPUNIT_TEST(computeelapsed_1);
    CPPUNIT_TEST(computeelapsed_2);
    CPPUNIT_TEST(getdivisor_1);
    CPPUNIT_TEST(getdivisor_2);
    
    CPPUNIT_TEST(getts_1);
    CPPUNIT_TEST(getts_2);
    CPPUNIT_TEST(setts_1);
    CPPUNIT_TEST(setts_2);
    
    CPPUNIT_TEST(originalsid_1);
    CPPUNIT_TEST(originalsid_2);
    
    CPPUNIT_TEST(bodysize_1);
    CPPUNIT_TEST(bodysize_2);
    CPPUNIT_TEST(bodyptr_1);
    CPPUNIT_TEST(bodyptr_2);
    CPPUNIT_TEST(bodyptr_3);
    CPPUNIT_TEST(bodyptr_4);
    
    CPPUNIT_TEST(hasbhdr_1);
    CPPUNIT_TEST(hasbhdr_2);
    
    CPPUNIT_TEST(getbhdr_1);
    CPPUNIT_TEST(getbhdr_2);
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
    void construct_5();
    void construct_6();
    void construct_7();
    void construct_8();

    void getstrings_1();
    void getstrings_2();
    
    void getoffset_1();
    void getoffset_2();
    void setoffset_1();
    void setoffset_2();
    void computeelapsed_1();
    void computeelapsed_2();
    void getdivisor_1();
    void getdivisor_2();
    
    void getts_1();
    void getts_2();
    void setts_1();
    void setts_2();
    
    void originalsid_1();
    void originalsid_2();
    
    void bodysize_1();
    void bodysize_2();
    
    void bodyptr_1();
    void bodyptr_2();
    void bodyptr_3();
    void bodyptr_4();
    
    void hasbhdr_1();
    void hasbhdr_2();
    void getbhdr_1();
    void getbhdr_2();
};

CPPUNIT_TEST_SUITE_REGISTRATION(v12txttest);

// Make pretty empty/useless item:
void v12txttest::construct_1()
{
    time_t now = time(nullptr);
    
    v12::CRingTextItem item(v12::PACKET_TYPES, 1024);
    const v12::TextItem* pItem =
        reinterpret_cast<const v12::TextItem*>(item.getItemPointer());
    EQ(v12::PACKET_TYPES, pItem->s_header.s_type);
    EQ(
        sizeof(v12::RingItemHeader) + sizeof(uint32_t) +
        sizeof(v12::TextItemBody),
        size_t(pItem->s_header.s_size)
    );
    EQ(sizeof(uint32_t), size_t(pItem->s_body.u_noBodyHeader.s_empty));
    const v12::TextItemBody* pBody = &(pItem->s_body.u_noBodyHeader.s_body);
    EQ(uint32_t(0), pBody->s_timeOffset);
    ASSERT(pBody->s_timestamp - now <= 1);
    EQ(uint32_t(0), pBody->s_stringCount);
    EQ(uint32_t(1), pBody->s_offsetDivisor);
    EQ(uint32_t(0), pBody->s_originalSid);
}
// Can't even make a useless item with a bad type:

void v12txttest::construct_2()
{
    CPPUNIT_ASSERT_THROW(
        v12::CRingTextItem item(v12::PHYSICS_EVENT, 1024),
        std::logic_error
    );
}
// partial construction with some strings.
// success:
void v12txttest::construct_3()
{
    time_t now = time(nullptr);
    v12::CRingTextItem item(v12::PACKET_TYPES, theStrings);
    const v12::TextItem* pItem =
        reinterpret_cast<const v12::TextItem*>(item.getItemPointer());
    EQ(v12::PACKET_TYPES, pItem->s_header.s_type);
    EQ(
        sizeof(v12::RingItemHeader) + sizeof(uint32_t) +
        sizeof(v12::TextItemBody) + stringLength(theStrings),
        size_t(pItem->s_header.s_size)
    );
    EQ(sizeof(uint32_t), size_t(pItem->s_body.u_noBodyHeader.s_empty));
    const v12::TextItemBody* pBody = &(pItem->s_body.u_noBodyHeader.s_body);
    EQ(uint32_t(0), pBody->s_timeOffset);
    ASSERT(pBody->s_timestamp - now <= 1);
    EQ(uint32_t(theStrings.size()), pBody->s_stringCount);
    EQ(uint32_t(1), pBody->s_offsetDivisor);
    EQ(uint32_t(0), pBody->s_originalSid);
    
    // check strings:
    
    const char* p = pBody->s_strings;
    for (int i =0; i < theStrings.size(); i++) {
        EQ(theStrings[i], std::string(p));
        p += theStrings[i].size() + 1;
    }
}
// bad construction with strings:

void v12txttest::construct_4()
{
    CPPUNIT_ASSERT_THROW(
        v12::CRingTextItem item(v12::PHYSICS_EVENT_COUNT, theStrings),
        std::logic_error
    );
}
// Full non body header construction successful:

void v12txttest::construct_5()
{
    time_t now = time(nullptr);
    v12::CRingTextItem item(v12::PACKET_TYPES, theStrings, 10, now, 1);
    
    const v12::TextItem* pItem =
        reinterpret_cast<const v12::TextItem*>(item.getItemPointer());
    EQ(v12::PACKET_TYPES, pItem->s_header.s_type);
    EQ(
        sizeof(v12::RingItemHeader) + sizeof(uint32_t) +
        sizeof(v12::TextItemBody) + stringLength(theStrings),
        size_t(pItem->s_header.s_size)
    );
    EQ(sizeof(uint32_t), size_t(pItem->s_body.u_noBodyHeader.s_empty));
    const v12::TextItemBody* pBody = &(pItem->s_body.u_noBodyHeader.s_body);
    EQ(uint32_t(10), pBody->s_timeOffset);
    EQ(uint32_t(now), pBody->s_timestamp);
    EQ(uint32_t(theStrings.size()), pBody->s_stringCount);
    EQ(uint32_t(1), pBody->s_offsetDivisor);
    EQ(uint32_t(0), pBody->s_originalSid);
    
    const char* p = pBody->s_strings;
    for (int i =0; i < theStrings.size(); i++) {
        EQ(theStrings[i], std::string(p));
        p += theStrings[i].size() + 1;
    }
}
// same as above but bad type.
void v12txttest::construct_6()
{
    time_t now = time(nullptr);
    CPPUNIT_ASSERT_THROW(
        v12::CRingTextItem item(v12::PERIODIC_SCALERS, theStrings, 10, now, 1),
        std::logic_error
    );
}
// full body header construction successful:

void v12txttest::construct_7()
{
    time_t now = time(nullptr);
    v12::CRingTextItem item(
        v12::PACKET_TYPES, 0x1234567890, 1, 2, theStrings, 100, now, 1
    );
    const v12::TextItem* pItem =
        reinterpret_cast<const v12::TextItem*>(item.getItemPointer());
    EQ(v12::PACKET_TYPES, pItem->s_header.s_type);
    EQ(
        sizeof(v12::RingItemHeader) + sizeof(v12::BodyHeader) +
        sizeof(v12::TextItemBody) + stringLength(theStrings),
        size_t(pItem->s_header.s_size)
    );
    
    // Check body header:
    
    const v12::BodyHeader* pBh = &(pItem->s_body.u_hasBodyHeader.s_bodyHeader);
    EQ(sizeof(v12::BodyHeader), size_t(pBh->s_size));
    EQ(uint64_t(0x1234567890), pBh->s_timestamp);
    EQ(uint32_t(1), pBh->s_sourceId);
    EQ(uint32_t(2), pBh->s_barrier);
    
    // check body:
    
    const v12::TextItemBody* pBody = &(pItem->s_body.u_hasBodyHeader.s_body);
    EQ(uint32_t(100), pBody->s_timeOffset);
    EQ(uint32_t(now), pBody->s_timestamp);
    EQ(uint32_t(theStrings.size()), pBody->s_stringCount);
    EQ(uint32_t(1), pBody->s_offsetDivisor);
    EQ(uint32_t(1), pBody->s_originalSid);
    
    const char* p = pBody->s_strings;
    for (int i =0; i < theStrings.size(); i++) {
        EQ(theStrings[i], std::string(p));
        p += theStrings[i].size() + 1;
    }
}
// failed due to bad type:

void v12txttest::construct_8()
{
    time_t now = time(nullptr);
    CPPUNIT_ASSERT_THROW(
        v12::CRingTextItem item(
            v12::BEGIN_RUN, 0x1234567890, 1, 2, theStrings, 100, now, 1
        ), std::logic_error
    );
}

// get strings from no body header item.
void v12txttest::getstrings_1()
{
    time_t now = time(nullptr);
    v12::CRingTextItem item(v12::PACKET_TYPES, theStrings, 10, now, 1);
    
    auto strings = item.getStrings();
    EQ(theStrings.size(), strings.size());
    for (int i =0; i < strings.size(); i++) {
        EQ(theStrings[i], strings[i]);
    }
}

// get strings from body header item.

void v12txttest::getstrings_2()
{
    time_t now = time(nullptr);
    v12::CRingTextItem item(
        v12::PACKET_TYPES, 0x1234567890, 1, 2, theStrings, 100, now, 1
    );
    
    auto strings = item.getStrings();
    EQ(theStrings.size(), strings.size());
    for (int i =0; i < strings.size(); i++) {
        EQ(theStrings[i], strings[i]);
    }
}
// get time offset from non body header..
void v12txttest::getoffset_1()
{
    time_t now = time(nullptr);
    v12::CRingTextItem item(v12::PACKET_TYPES, theStrings, 10, now, 1);
    
    EQ(uint32_t(10), item.getTimeOffset());
}
// get time offset from body header item.

void v12txttest::getoffset_2()
{
    time_t now = time(nullptr);
    v12::CRingTextItem item(
        v12::PACKET_TYPES, 0x1234567890, 1, 2, theStrings, 100, now, 1
    );
    
    EQ(uint32_t(100), item.getTimeOffset());
}
// set offset in non body header:

void v12txttest::setoffset_1()
{
    time_t now = time(nullptr);
    v12::CRingTextItem item(v12::PACKET_TYPES, theStrings, 10, now, 1);
    item.setTimeOffset(200);
    EQ(uint32_t(200), item.getTimeOffset());
}
// set offset in body header item

void v12txttest::setoffset_2()
{
    time_t now = time(nullptr);
    v12::CRingTextItem item(
        v12::PACKET_TYPES, 0x1234567890, 1, 2, theStrings, 100, now, 1
    );
    
    item.setTimeOffset(150);
    EQ(uint32_t(150), item.getTimeOffset());
}
// compute elapsed time no body headzer.

void v12txttest::computeelapsed_1()
{
    time_t now = time(nullptr);
    v12::CRingTextItem item(v12::PACKET_TYPES, theStrings, 10, now, 2);
    EQ(float(5), item.computeElapsedTime());
}
// compute elapsed time  with body header
void v12txttest::computeelapsed_2()
{
    time_t now = time(nullptr);
    v12::CRingTextItem item(
        v12::PACKET_TYPES, 0x1234567890, 1, 2, theStrings, 100, now, 5
    );
    EQ(float(100/5), item.computeElapsedTime());
}
// get divisor from non body header

void v12txttest::getdivisor_1()
{
    time_t now = time(nullptr);
    v12::CRingTextItem item(v12::PACKET_TYPES, theStrings, 10, now, 2);
    EQ(uint32_t(2), item.getTimeDivisor());
}
// get divisor from body header item.

void v12txttest::getdivisor_2()
{
    time_t now = time(nullptr);
    v12::CRingTextItem item(
        v12::PACKET_TYPES, 0x1234567890, 1, 2, theStrings, 100, now, 5
    );
    EQ(uint32_t(5), item.getTimeDivisor());
}

// get timestamp from non body header:

void v12txttest::getts_1()
{
    time_t now = time(nullptr);
    v12::CRingTextItem item(v12::PACKET_TYPES, theStrings, 10, now, 2);
    EQ(now, item.getTimestamp());
}
// Get from body header item.
void v12txttest::getts_2()
{
    time_t now = time(nullptr);
    v12::CRingTextItem item(
        v12::PACKET_TYPES, 0x1234567890, 1, 2, theStrings, 100, now, 5
    );
    EQ(now, item.getTimestamp());
}
// set timestamp for body header item.
void v12txttest::setts_1()
{
    time_t now = time(nullptr);
    v12::CRingTextItem item(v12::PACKET_TYPES, theStrings, 10, now, 2);
    
    item.setTimestamp(now+100);
    EQ(now+100, item.getTimestamp());
}
// for non body header:

void v12txttest::setts_2()
{
    time_t now = time(nullptr);
    v12::CRingTextItem item(
        v12::PACKET_TYPES, 0x1234567890, 1, 2, theStrings, 100, now, 5
    );
    item.setTimestamp(now+100);
    EQ(now+100, item.getTimestamp());
}
// original sid from no body heade ris 0.
void v12txttest::originalsid_1()
{
    time_t now = time(nullptr);
    v12::CRingTextItem item(v12::PACKET_TYPES, theStrings, 10, now, 2);
    
    EQ(uint32_t(0), item.getOriginalSourceId());
}
// sid is osid if body header:

void v12txttest::originalsid_2()
{
    time_t now = time(nullptr);
    v12::CRingTextItem item(
        v12::PACKET_TYPES, 0x1234567890, 1, 2, theStrings, 100, now, 5
    );
    EQ(uint32_t(1), item.getOriginalSourceId());
}
// body size with no body header.

void v12txttest::bodysize_1()
{
    time_t now = time(nullptr);
    v12::CRingTextItem item(v12::PACKET_TYPES, theStrings, 10, now, 2);
    
    EQ(
        sizeof(v12::TextItemBody) + stringLength(theStrings),
        item.getBodySize()
    );
}
// body size with body header is the same:
void v12txttest::bodysize_2()
{
    time_t now = time(nullptr);
    v12::CRingTextItem item(
        v12::PACKET_TYPES, 0x1234567890, 1, 2, theStrings, 100, now, 5
    );
    
    EQ(
        sizeof(v12::TextItemBody) + stringLength(theStrings),
        item.getBodySize()
    );

}
// const getBodyPointer().
// body pointer for non body header:

void v12txttest::bodyptr_1()
{
    time_t now = time(nullptr);
    v12::CRingTextItem item(v12::PACKET_TYPES, theStrings, 10, now, 2);
    
    const v12::TextItem* pItem =
        reinterpret_cast<const v12::TextItem*>(item.getItemPointer());
    const v12::TextItemBody* pBody =
        reinterpret_cast<const v12::TextItemBody*>(item.getBodyPointer());
    EQ(&(pItem->s_body.u_noBodyHeader.s_body), pBody);        
}
// body pointe4r for body header case:

void v12txttest::bodyptr_2()
{
    time_t now = time(nullptr);
    v12::CRingTextItem item(
        v12::PACKET_TYPES, 0x1234567890, 1, 2, theStrings, 100, now, 5
    );
    const v12::TextItem* pItem =
        reinterpret_cast<const v12::TextItem*>(item.getItemPointer());
    const v12::TextItemBody* pBody =
        reinterpret_cast<const v12::TextItemBody*>(item.getBodyPointer());
    EQ(&(pItem->s_body.u_hasBodyHeader.s_body), pBody);  
    
}
// Similar tests but non const calls

void v12txttest::bodyptr_3()
{
    time_t now = time(nullptr);
    v12::CRingTextItem item(v12::PACKET_TYPES, theStrings, 10, now, 2);
    
     v12::TextItem* pItem =
        reinterpret_cast< v12::TextItem*>(item.getItemPointer());
     v12::TextItemBody* pBody =
        reinterpret_cast< v12::TextItemBody*>(item.getBodyPointer());
    EQ(&(pItem->s_body.u_noBodyHeader.s_body), pBody);    
}
void v12txttest::bodyptr_4()
{
    time_t now = time(nullptr);
    v12::CRingTextItem item(
        v12::PACKET_TYPES, 0x1234567890, 1, 2, theStrings, 100, now, 5
    );
     v12::TextItem* pItem =
        reinterpret_cast< v12::TextItem*>(item.getItemPointer());
     v12::TextItemBody* pBody =
        reinterpret_cast< v12::TextItemBody*>(item.getBodyPointer());
    EQ(&(pItem->s_body.u_hasBodyHeader.s_body), pBody);  
}
// does not have body header:

void v12txttest::hasbhdr_1()
{
    time_t now = time(nullptr);
    v12::CRingTextItem item(v12::PACKET_TYPES, theStrings, 10, now, 2);
    ASSERT(! item.hasBodyHeader());
}
// does have body header.

void v12txttest::hasbhdr_2()
{
     time_t now = time(nullptr);
    v12::CRingTextItem item(
        v12::PACKET_TYPES, 0x1234567890, 1, 2, theStrings, 100, now, 5
    );
    ASSERT(item.hasBodyHeader());
}
// If no body header getBodyHeader -> null

void v12txttest::getbhdr_1()
{
    time_t now = time(nullptr);
    v12::CRingTextItem item(v12::PACKET_TYPES, theStrings, 10, now, 2);
    ASSERT(nullptr == item.getBodyHeader());
}
// If body header getBodyHeder -> a non null pointer to it.

void v12txttest::getbhdr_2()
{
    time_t now = time(nullptr);
    v12::CRingTextItem item(
        v12::PACKET_TYPES, 0x1234567890, 1, 2, theStrings, 100, now, 5
    );
    const v12::BodyHeader* p = reinterpret_cast<const v12::BodyHeader*>(item.getBodyHeader());
    ASSERT(nullptr != p);
    const v12::TextItem* pItem =
        reinterpret_cast<const v12::TextItem*>(item.getItemPointer());
    EQ(&(pItem->s_body.u_hasBodyHeader.s_bodyHeader), p);
}