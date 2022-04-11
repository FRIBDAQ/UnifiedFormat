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

/** @file:  v11statetest.cpp
 *  @brief: Tests for v11:CRingStateItem
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#include "CRingStateChangeItem.h"
#include "DataFormat.h"

#include <time.h>
#include <string.h>

class v11statetest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v11statetest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    CPPUNIT_TEST(construct_3);
    
    CPPUNIT_TEST(run_1);
    CPPUNIT_TEST(run_2);
    CPPUNIT_TEST(run_3);
    CPPUNIT_TEST(run_4);
    
    CPPUNIT_TEST(elapsed_1);
    CPPUNIT_TEST(elapsed_2);
    CPPUNIT_TEST(elapsed_3);
    CPPUNIT_TEST(elapsed_4);
    CPPUNIT_TEST(elapsed_5);
    CPPUNIT_TEST(elapsed_6);
    
    CPPUNIT_TEST(title_1);
    CPPUNIT_TEST(title_2);
    CPPUNIT_TEST(title_3);
    CPPUNIT_TEST(title_4);
    
    CPPUNIT_TEST(time_1);
    CPPUNIT_TEST(time_2);
    CPPUNIT_TEST(time_3);
    CPPUNIT_TEST(time_4);
    
    CPPUNIT_TEST(originalsid_1);
    CPPUNIT_TEST(originalsid_2);
    
    // Body pointer methods must work as the
    // stuff we tested so far depends on it internally...
    
    CPPUNIT_TEST(bodyhdr_1);
    CPPUNIT_TEST(bodyhdr_2);
    CPPUNIT_TEST(bodyhdr_3);
    CPPUNIT_TEST(bodyhdr_4);
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
    
    void run_1();
    void run_2();
    void run_3();
    void run_4();
    
    void elapsed_1();
    void elapsed_2();
    void elapsed_3();
    void elapsed_4();
    void elapsed_5();
    void elapsed_6();
    
    void title_1();
    void title_2();
    void title_3();
    void title_4();
    
    void time_1();
    void time_2();
    void time_3();
    void time_4();
    
    void originalsid_1();
    void originalsid_2();
    
    void bodyhdr_1();
    void bodyhdr_2();
    void bodyhdr_3();
    void bodyhdr_4();
};

CPPUNIT_TEST_SUITE_REGISTRATION(v11statetest);

// Simple constructor.
void v11statetest::construct_1()
{
    time_t now = time(nullptr);
    v11::CRingStateChangeItem item;
    const v11::StateChangeItem* pItem =
        reinterpret_cast<const v11::StateChangeItem*>(item.getItemPointer());
    
    EQ(
       sizeof(v11::RingItemHeader) +
       sizeof(uint32_t) +
       sizeof(v11::StateChangeItemBody),
       size_t(pItem->s_header.s_size)
    );
    EQ(v11::BEGIN_RUN, pItem->s_header.s_type);
    EQ(uint32_t(0), pItem->s_body.u_noBodyHeader.s_mbz);
    
    const v11::StateChangeItemBody* pBody = &(pItem->s_body.u_noBodyHeader.s_body);
    EQ(uint32_t(0), pBody->s_runNumber);
    EQ(uint32_t(0), pBody->s_timeOffset);
    ASSERT(pBody->s_Timestamp - now <= 1);
    EQ(uint32_t(1), pBody->s_offsetDivisor);
    EQ(size_t(0), strlen(pBody->s_title));
}
// Full non bod header constructor.

void v11statetest::construct_2()
{
    time_t now = time(nullptr);
    v11::CRingStateChangeItem item(
        v11::PAUSE_RUN, 1234, 10, now, "This is a title"
    );
    const v11::StateChangeItem* pItem =
        reinterpret_cast<const v11::StateChangeItem*>(item.getItemPointer());
    
    EQ(
       sizeof(v11::RingItemHeader) +
       sizeof(uint32_t) +
       sizeof(v11::StateChangeItemBody),
       size_t(pItem->s_header.s_size)
    );
    EQ(v11::PAUSE_RUN, pItem->s_header.s_type);
    EQ(uint32_t(0), pItem->s_body.u_noBodyHeader.s_mbz);
    
    const v11::StateChangeItemBody* pBody = &(pItem->s_body.u_noBodyHeader.s_body);
    EQ(uint32_t(1234), pBody->s_runNumber);
    EQ(uint32_t(10), pBody->s_timeOffset);
    EQ(uint32_t(now), pBody->s_Timestamp);
    EQ(uint32_t(1), pBody->s_offsetDivisor);
    EQ(0, strcmp("This is a title", pBody->s_title));
    
}
// Construct with body header.

void v11statetest::construct_3()
{
    time_t now = time(nullptr);
    v11::CRingStateChangeItem item(
        0x1234567890, 1, 0, v11::END_RUN, 12, 100, now,
        "This is my title", 2
    );
    const v11::StateChangeItem* pItem =
        reinterpret_cast<const v11::StateChangeItem*>(item.getItemPointer());
    
    EQ(
       sizeof(v11::RingItemHeader) +
       sizeof(v11::BodyHeader) +
       sizeof(v11::StateChangeItemBody),
       size_t(pItem->s_header.s_size)
    );
    EQ(v11::END_RUN, pItem->s_header.s_type);
    
    
    const v11::BodyHeader* pBh =
        &(pItem->s_body.u_hasBodyHeader.s_bodyHeader);
    EQ(sizeof(v11::BodyHeader), size_t(pBh->s_size));
    EQ(uint64_t(0x1234567890), pBh->s_timestamp);
    EQ(uint32_t(1), pBh->s_sourceId);
    EQ(uint32_t(0), pBh->s_barrier);
    
    const v11::StateChangeItemBody* pBody = &(pItem->s_body.u_hasBodyHeader.s_body);
    EQ(uint32_t(12), pBody->s_runNumber);
    EQ(uint32_t(100), pBody->s_timeOffset);
    EQ(uint32_t(now), pBody->s_Timestamp);
    EQ(uint32_t(2), pBody->s_offsetDivisor);
    EQ(0, strcmp("This is my title", pBody->s_title));
}

// getRunNumber for non body header item
void v11statetest::run_1()
{
    time_t now = time(nullptr);
    v11::CRingStateChangeItem item(
        v11::PAUSE_RUN, 1234, 10, now, "This is a title"
    );
    EQ(uint32_t(1234), item.getRunNumber());
    
}
// set run number for non body header item.

void v11statetest::run_2()
{
    time_t now = time(nullptr);
    v11::CRingStateChangeItem item(
        v11::PAUSE_RUN, 1234, 10, now, "This is a title"
    );
    item.setRunNumber(666);
    EQ(uint32_t(666), item.getRunNumber());
}
// get run number for body header item.
void v11statetest::run_3()
{
    time_t now = time(nullptr);
    v11::CRingStateChangeItem item(
        0x1234567890, 1, 0, v11::END_RUN, 12, 100, now,
        "This is my title", 2
    );
    EQ(uint32_t(12), item.getRunNumber());
}
// set run number for body header item.

void v11statetest::run_4()
{
    time_t now = time(nullptr);
    v11::CRingStateChangeItem item(
        0x1234567890, 1, 0, v11::END_RUN, 12, 100, now,
        "This is my title", 2
    );
    item.setRunNumber(1111);
    EQ(uint32_t(1111), item.getRunNumber());
}

// get elapsed time raw from non body header:

void v11statetest::elapsed_1()
{
    time_t now = time(nullptr);
    v11::CRingStateChangeItem item(
        v11::PAUSE_RUN, 1234, 10, now, "This is a title"
    );
    EQ(uint32_t(10), item.getElapsedTime());
}
// Set elapsed time in non body header.
void v11statetest::elapsed_2()
{
    time_t now = time(nullptr);
    v11::CRingStateChangeItem item(
        v11::PAUSE_RUN, 1234, 10, now, "This is a title"
    );
    item.setElapsedTime(20);
    EQ(uint32_t(20), item.getElapsedTime());
}
// compute elapsed time in non body header:

void v11statetest::elapsed_3()
{
    time_t now = time(nullptr);
    v11::CRingStateChangeItem item(
        v11::PAUSE_RUN, 1234, 10, now, "This is a title"
    );
    v11::pStateChangeItem pItem =
        reinterpret_cast<v11::pStateChangeItem>(item.getItemPointer());
    v11::pStateChangeItemBody pBody =
        &(pItem->s_body.u_noBodyHeader.s_body);
    pBody->s_offsetDivisor = 2;
    EQ(float(5) , item.computeElapsedTime());
}
// Get elapsed time from body header item.

void v11statetest::elapsed_4()
{
    time_t now = time(nullptr);
    v11::CRingStateChangeItem item(
        0x1234567890, 1, 0, v11::END_RUN, 12, 100, now,
        "This is my title", 2
    );
    
    EQ(uint32_t(100), item.getElapsedTime());
}
// set elapsed time in body header item.

void v11statetest::elapsed_5()
{
    time_t now = time(nullptr);
    v11::CRingStateChangeItem item(
        0x1234567890, 1, 0, v11::END_RUN, 12, 100, now,
        "This is my title", 2
    );
    item.setElapsedTime(200);
    EQ(uint32_t(200), item.getElapsedTime());
}
// compute elapsed in body header item.
void v11statetest::elapsed_6()
{
    time_t now = time(nullptr);
    v11::CRingStateChangeItem item(
        0x1234567890, 1, 0, v11::END_RUN, 12, 100, now,
        "This is my title", 2
    );
    EQ(float(50), item.computeElapsedTime());
}
// Get title with non body header,

void v11statetest::title_1()
{
    time_t now = time(nullptr);
    v11::CRingStateChangeItem item(
        v11::PAUSE_RUN, 1234, 10, now, "This is a title"
    );
    EQ(std::string("This is a title"), item.getTitle());
}
// set title with non body header.
void v11statetest::title_2()
{
    time_t now = time(nullptr);
    v11::CRingStateChangeItem item(
        v11::PAUSE_RUN, 1234, 10, now, "This is a title"
    );
    std::string newtitle("I Changed the title");
    item.setTitle(newtitle);
    EQ(newtitle, item.getTitle());
}
// get title from body header item.

void v11statetest::title_3()
{
    time_t now = time(nullptr);
    std::string title("This is my title");
    v11::CRingStateChangeItem item(
        0x1234567890, 1, 0, v11::END_RUN, 12, 100, now,
        title, 2
    );
    EQ(title, item.getTitle());
}
// set title from body header item.
void v11statetest::title_4()
{
    time_t now = time(nullptr);
    std::string title("This is my title");
    v11::CRingStateChangeItem item(
        0x1234567890, 1, 0, v11::END_RUN, 12, 100, now,
        title, 2
    );
    std::string newtitle("I Changed the title");
    item.setTitle(newtitle);
    EQ(newtitle, item.getTitle());
}

// Get time from non body header
void v11statetest::time_1()
{
    time_t now = time(nullptr);
    v11::CRingStateChangeItem item(
        v11::PAUSE_RUN, 1234, 10, now, "This is a title"
    );
    EQ(now, item.getTimestamp());
}
// Set time from non body header:
void v11statetest::time_2()
{
    time_t now = time(nullptr);
    v11::CRingStateChangeItem item(
        v11::PAUSE_RUN, 1234, 10, now, "This is a title"
    );
    item.setTimestamp(now+10);
    EQ(now+10, item.getTimestamp());
}
// get time from body header item.

void v11statetest::time_3()
{
    time_t now = time(nullptr);
    std::string title("This is my title");
    v11::CRingStateChangeItem item(
        0x1234567890, 1, 0, v11::END_RUN, 12, 100, now,
        title, 2
    );
    EQ(now, item.getTimestamp());
}
// set time from body header item

void v11statetest::time_4()
{
    time_t now = time(nullptr);
    std::string title("This is my title");
    v11::CRingStateChangeItem item(
        0x1234567890, 1, 0, v11::END_RUN, 12, 100, now,
        title, 2
    );
    item.setTimestamp(now+10);
    EQ(now+10, item.getTimestamp());
    
}
// Original sid from non body header is 0.
void v11statetest::originalsid_1()
{
    time_t now = time(nullptr);
    v11::CRingStateChangeItem item(
        v11::PAUSE_RUN, 1234, 10, now, "This is a title"
    );
    EQ(uint32_t(0), item.getOriginalSourceId());
}
// original sid from body header just gets the sid.

void v11statetest::originalsid_2()
{
    time_t now = time(nullptr);
    std::string title("This is my title");
    v11::CRingStateChangeItem item(
        0x1234567890, 1, 0, v11::END_RUN, 12, 100, now,
        title, 2
    );
    EQ(uint32_t(1), item.getOriginalSourceId());
}

/**
 * getBodyHeader from non body header -> nullptr.
 */
void v11statetest::bodyhdr_1()
{
    time_t now = time(nullptr);
    v11::CRingStateChangeItem item(
        v11::PAUSE_RUN, 1234, 10, now, "This is a title"
    );
    ASSERT(item.getBodyHeader() == nullptr);
}

 // setbodyheader makes a new body header
 void v11statetest::bodyhdr_2()
 {
    time_t now = time(nullptr);
    v11::CRingStateChangeItem item(
        v11::PAUSE_RUN, 1234, 10, now, "This is a title"
    );
    //Note this comes from v11::CRingItem so it should be ok.
    item.setBodyHeader(0x1234567890, 1, 5);
    const v11::BodyHeader* pH =
        reinterpret_cast<const v11::BodyHeader*>(item.getBodyHeader());
    ASSERT(pH != nullptr);
    EQ(sizeof(v11::BodyHeader), size_t(pH->s_size));
    EQ(uint64_t(0x1234567890), pH->s_timestamp);
 }
 // getbodyheader from body header item.
 
 void v11statetest::bodyhdr_3()
 {
    time_t now = time(nullptr);
    std::string title("This is my title");
    v11::CRingStateChangeItem item(
        0x1234567890, 1, 0, v11::END_RUN, 12, 100, now,
        title, 2
    );
    const v11::BodyHeader* pH =
        reinterpret_cast<const v11::BodyHeader*>(item.getBodyHeader());
    ASSERT(pH != nullptr);
    EQ(sizeof(v11::BodyHeader), size_t(pH->s_size));
    EQ(uint64_t(0x1234567890), pH->s_timestamp);
 }
 // setbodyheader
 
 void v11statetest::bodyhdr_4()
 {
    time_t now = time(nullptr);
    std::string title("This is my title");
    v11::CRingStateChangeItem item(
        0x1234567890, 1, 0, v11::END_RUN, 12, 100, now,
        title, 2
    );
    const v11::BodyHeader* pH =
        reinterpret_cast<const v11::BodyHeader*>(item.getBodyHeader());
    item.setBodyHeader(0x111111111, 3, 4);
    EQ(uint64_t(0x111111111), pH->s_timestamp);
    EQ(uint32_t(3), pH->s_sourceId);
    EQ(uint32_t(4), pH->s_barrier);
 }
 