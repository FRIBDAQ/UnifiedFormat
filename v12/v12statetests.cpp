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

/** @file:  v12statetests.cpp
 *  @brief: Tests for v12::CRingStateChangeitem.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CRingStateChangeItem.h"
#include "DataFormat.h"
#include <time.h>
#include <string.h>
#include <stdexcept>

class v12statetest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v12statetest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    CPPUNIT_TEST(construct_3);
    CPPUNIT_TEST(construct_4);
    CPPUNIT_TEST(construct_5);
    CPPUNIT_TEST(construct_6);
    
    CPPUNIT_TEST(getrun_1);
    CPPUNIT_TEST(getrun_2);
    CPPUNIT_TEST(setrun_1);
    CPPUNIT_TEST(setrun_2);
    
    CPPUNIT_TEST(getelapsed_1);
    CPPUNIT_TEST(getelapsed_2);
    CPPUNIT_TEST(setelapsed_1);
    CPPUNIT_TEST(setelapsed_2);
    CPPUNIT_TEST(getdivisor_1);
    CPPUNIT_TEST(getdivisor_2);
    CPPUNIT_TEST(computetime_1);
    CPPUNIT_TEST(computetime_2);
    
    CPPUNIT_TEST(gettitle_1);
    CPPUNIT_TEST(gettitle_2);
    CPPUNIT_TEST(settitle_1);
    CPPUNIT_TEST(settitle_2);
    
    CPPUNIT_TEST(getclock_1);
    CPPUNIT_TEST(getclock_2);
    CPPUNIT_TEST(setclock_1);
    CPPUNIT_TEST(setclock_2);
    
    CPPUNIT_TEST(getosid_1);
    CPPUNIT_TEST(getosid_2);
    
    CPPUNIT_TEST(bodysize_1);
    CPPUNIT_TEST(bodysize_2);
    CPPUNIT_TEST(getbody_1);
    CPPUNIT_TEST(getbody_2);
    CPPUNIT_TEST(getbody_3);
    CPPUNIT_TEST(getbody_4);
    
    CPPUNIT_TEST(hashdr_1);
    CPPUNIT_TEST(hashdr_2);
    CPPUNIT_TEST(gethdr_1);
    CPPUNIT_TEST(gethdr_2);
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
    
    void getrun_1();
    void getrun_2();
    
    void setrun_1();
    void setrun_2();
    
    void getelapsed_1();
    void getelapsed_2();
    void setelapsed_1();
    void setelapsed_2();
    void getdivisor_1();
    void getdivisor_2();
    void computetime_1();
    void computetime_2();
    
    void gettitle_1();
    void gettitle_2();
    void settitle_1();
    void settitle_2();
    
    void getclock_1();
    void getclock_2();
    void setclock_1();
    void setclock_2();
    
    void getosid_1();
    void getosid_2();
    
    void bodysize_1();
    void bodysize_2();
    void getbody_1();
    void getbody_2();
    void getbody_3();
    void getbody_4();
    
    void hashdr_1();
    void hashdr_2();
    void gethdr_1();
    void gethdr_2();
};

CPPUNIT_TEST_SUITE_REGISTRATION(v12statetest);

/// minimal good construction.

void v12statetest::construct_1()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(v12::BEGIN_RUN);
    const v12::StateChangeItem* pItem =
        reinterpret_cast<const v12::StateChangeItem*>(item.getItemPointer());
    EQ(v12::BEGIN_RUN, pItem->s_header.s_type);
    EQ(
        sizeof(v12::RingItemHeader) + sizeof(uint32_t) +
        sizeof(v12::StateChangeItemBody),
        size_t(pItem->s_header.s_size)
    );
    EQ(sizeof(uint32_t), size_t(pItem->s_body.u_noBodyHeader.s_empty));
    
    const v12::StateChangeItemBody* pBody =
        &(pItem->s_body.u_noBodyHeader.s_body);
    EQ(uint32_t(0), pBody->s_runNumber);
    EQ(uint32_t(0), pBody->s_timeOffset);
    ASSERT((pBody->s_Timestamp - now) < 1);
    EQ(uint32_t(1), pBody->s_offsetDivisor);
    EQ(uint32_t(0), pBody->s_originalSid);
    EQ(size_t(0), strlen(pBody->s_title));
}
// Minimal construction with bad type:

void v12statetest::construct_2()
{
    CPPUNIT_ASSERT_THROW(
        v12::CRingStateChangeItem item(v12::PHYSICS_EVENT),
        std::logic_error
    );
}

// good full non-bodyheader construction.
void v12statetest::construct_3()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        v12::BEGIN_RUN, 1234, 0, now, "This is a title" 
    );
    const v12::StateChangeItem* pItem =
        reinterpret_cast<const v12::StateChangeItem*>(item.getItemPointer());
    EQ(v12::BEGIN_RUN, pItem->s_header.s_type);
    EQ(
        sizeof(v12::RingItemHeader) + sizeof(uint32_t) +
        sizeof(v12::StateChangeItemBody),
        size_t(pItem->s_header.s_size)
    );
    EQ(sizeof(uint32_t), size_t(pItem->s_body.u_noBodyHeader.s_empty));
    
    const v12::StateChangeItemBody* pBody =
        &(pItem->s_body.u_noBodyHeader.s_body);
    EQ(uint32_t(1234), pBody->s_runNumber);
    EQ(uint32_t(0), pBody->s_timeOffset);
    EQ(now, time_t(pBody->s_Timestamp));
    EQ(uint32_t(1), pBody->s_offsetDivisor);
    EQ(uint32_t(0), pBody->s_originalSid);
    EQ(std::string("This is a title"), std::string(pBody->s_title));
}
// full non-body header construction with bad type:

void v12statetest::construct_4()
{
    time_t now = time(nullptr);
    CPPUNIT_ASSERT_THROW(
        v12::CRingStateChangeItem item(
            v12::PERIODIC_SCALERS, 1234, 0, now, "This is a title"
        ), std::logic_error
    );
}
// good construction with body header.

void v12statetest::construct_5()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        0x1234567890, 1, 2, v12::BEGIN_RUN, 12, 0, now, "This is a title"
    );
    const v12::StateChangeItem* pItem =
        reinterpret_cast<const v12::StateChangeItem*>(item.getItemPointer());
        
    EQ(v12::BEGIN_RUN, pItem->s_header.s_type);
    EQ(
        sizeof(v12::RingItemHeader) + sizeof(v12::BodyHeader) +
        sizeof(v12::StateChangeItemBody),
        size_t(pItem->s_header.s_size)
    );
    
    const v12::BodyHeader* pBh = &(pItem->s_body.u_hasBodyHeader.s_bodyHeader);
    EQ(sizeof(v12::BodyHeader), size_t(pBh->s_size));
    EQ(uint64_t(0x1234567890), pBh->s_timestamp);
    EQ(uint32_t(1), pBh->s_sourceId);
    EQ(uint32_t(2), pBh->s_barrier);
    
    const v12::StateChangeItemBody* pBody =
        &(pItem->s_body.u_hasBodyHeader.s_body);
    EQ(uint32_t(12), pBody->s_runNumber);
    EQ(uint32_t(0), pBody->s_timeOffset);
    EQ(now, time_t(pBody->s_Timestamp));
    EQ(uint32_t(1), pBody->s_offsetDivisor);
    EQ(uint32_t(1), pBody->s_originalSid);
    EQ(std::string("This is a title"), std::string(pBody->s_title));
}
// full body header construction but bad type.

void v12statetest::construct_6()
{
    time_t now = time(nullptr);
    CPPUNIT_ASSERT_THROW(
        v12::CRingStateChangeItem item(
            0x1234567890, 1, 2, v12::ABNORMAL_ENDRUN, 12, 0, now, "This is a title"
        ), std::logic_error
    );
}

// get run number from body headerless item.
void v12statetest::getrun_1()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        v12::BEGIN_RUN, 1234, 0, now, "This is a title" 
    );
    EQ(uint32_t(1234), item.getRunNumber());
}
// get run number from item with body header:

void v12statetest::getrun_2()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        0x1234567890, 1, 2, v12::BEGIN_RUN, 12, 0, now, "This is a title"
    );
    EQ(uint32_t(12), item.getRunNumber());
}
// can set a new run number in a non body header item.
void v12statetest::setrun_1()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        v12::BEGIN_RUN, 1234, 0, now, "This is a title" 
    );
    
    item.setRunNumber(666);
    EQ(uint32_t(666), item.getRunNumber());
}
// can set run number in an item with a body header:

void v12statetest::setrun_2()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        0x1234567890, 1, 2, v12::BEGIN_RUN, 12, 0, now, "This is a title"
    );
    item.setRunNumber(12345);
    EQ(uint32_t(12345), item.getRunNumber());
}

// get elapsed time no body  header.

void v12statetest::getelapsed_1()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        v12::PAUSE_RUN, 1234, 100, now, "This is a title" 
    );
    EQ(uint32_t(100), item.getElapsedTime());
}
// Get elapsed time body header.

void v12statetest::getelapsed_2()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        0x1234567890, 1, 2, v12::PAUSE_RUN, 12, 120, now, "This is a title"
    );
    EQ(uint32_t(120), item.getElapsedTime());
}
// set elapsed time no body header:

void v12statetest::setelapsed_1()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        v12::BEGIN_RUN, 1234, 0, now, "This is a title" 
    );
    item.setElapsedTime(200);
    EQ(uint32_t(200), item.getElapsedTime());
}
// set elapsed time body header case.

void v12statetest::setelapsed_2()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        0x1234567890, 1, 2, v12::PAUSE_RUN, 12, 120, now, "This is a title"
    );
    item.setElapsedTime(200);
    EQ(uint32_t(200), item.getElapsedTime());
}
// get elapsed time divisor no body header.

void v12statetest::getdivisor_1()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        v12::BEGIN_RUN, 1234, 0, now, "This is a title" 
    );
    EQ(uint32_t(1), item.getTimeDivisor());
}
// get elapsed time divisor body header

void v12statetest::getdivisor_2()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        0x1234567890, 1, 2, v12::PAUSE_RUN, 12, 120, now, "This is a title",
        2
    );
    EQ(uint32_t(2), item.getTimeDivisor());
}
// compute elapsed time no body header:

void v12statetest::computetime_1()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        v12::BEGIN_RUN, 1234, 10, now, "This is a title" 
    );
    EQ(float(10), item.computeElapsedTime());
}
// compute elapsed time body heaer:

void v12statetest::computetime_2()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        0x1234567890, 1, 2, v12::PAUSE_RUN, 12, 120, now, "This is a title",
        2
    );
    EQ(float(60), item.computeElapsedTime());
}
// Get title - no body header.
void v12statetest::gettitle_1()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        v12::BEGIN_RUN, 1234, 10, now, "This is a title" 
    );
    EQ(std::string("This is a title"), item.getTitle());
}
void v12statetest::gettitle_2()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        0x1234567890, 1, 2, v12::PAUSE_RUN, 12, 120, now, "This is a title",
        2
    );
    EQ(std::string("This is a title"), item.getTitle());
}
// set title no body header:
void v12statetest::settitle_1()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        v12::BEGIN_RUN, 1234, 10, now, "This is a title" 
    );
    item.setTitle("This title has changed");
    EQ(std::string("This title has changed"), item.getTitle());
}
// set title body header.

void v12statetest::settitle_2()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        0x1234567890, 1, 2, v12::PAUSE_RUN, 12, 120, now, "This is a title",
        2
    );
    item.setTitle("This is a different title");
    EQ(std::string("This is a different title"), item.getTitle());
}
// get clock time from no bodyheader.
void v12statetest::getclock_1()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        v12::BEGIN_RUN, 1234, 10, now, "This is a title" 
    );
    EQ(now, item.getTimestamp());
}
// get clock time from body header item:

void v12statetest::getclock_2()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        0x1234567890, 1, 2, v12::PAUSE_RUN, 12, 120, now, "This is a title",
        2
    );
    EQ(now, item.getTimestamp());
}
// set timestamp for non body header:

void v12statetest::setclock_1()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        v12::BEGIN_RUN, 1234, 10, now, "This is a title" 
    );
    item.setTimestamp(now+10);
    EQ(now+10, item.getTimestamp());
}
// set timestamp for body header:
void v12statetest::setclock_2()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        0x1234567890, 1, 2, v12::PAUSE_RUN, 12, 120, now, "This is a title",
        2
    );
    item.setTimestamp(now+10);
    EQ(now+10, item.getTimestamp());
}
// osid for non body header item is 0:

void v12statetest::getosid_1()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        v12::BEGIN_RUN, 1234, 10, now, "This is a title" 
    );
    EQ(uint32_t(0), item.getOriginalSourceId());
}
// osid for body header item is the source id (the body header can
// be changed the osid is immutable).
void v12statetest::getosid_2()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        0x1234567890, 1, 2, v12::PAUSE_RUN, 12, 120, now, "This is a title",
        2
    );
    EQ(uint32_t(1), item.getOriginalSourceId());
}

// body size from non body header:

void v12statetest::bodysize_1()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        v12::BEGIN_RUN, 1234, 10, now, "This is a title" 
    );
    EQ(sizeof(v12::StateChangeItemBody), item.getBodySize());
}
// body size from body header item.

void v12statetest::bodysize_2()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        0x1234567890, 1, 2, v12::PAUSE_RUN, 12, 120, now, "This is a title",
        2
    );
    EQ(sizeof(v12::StateChangeItemBody), item.getBodySize());
}

// Get body  pointer (const) non body header:

void v12statetest::getbody_1()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        v12::BEGIN_RUN, 1234, 10, now, "This is a title" 
    );
    const v12::StateChangeItemBody* p =
        reinterpret_cast<const v12::StateChangeItemBody*>(item.getBodyPointer());
    const v12::StateChangeItem* pItem =
        reinterpret_cast<const v12::StateChangeItem*>(item.getItemPointer());
    EQ(&(pItem->s_body.u_noBodyHeader.s_body), p);
}
// get body pointer (non const) no body header:

void v12statetest::getbody_2()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        v12::BEGIN_RUN, 1234, 10, now, "This is a title" 
    );
     v12::StateChangeItemBody* p =
        reinterpret_cast< v12::StateChangeItemBody*>(item.getBodyPointer());
     v12::StateChangeItem* pItem =
        reinterpret_cast< v12::StateChangeItem*>(item.getItemPointer());
    EQ(&(pItem->s_body.u_noBodyHeader.s_body), p);
}
// Get body pointer (const) body header:

void v12statetest::getbody_3()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        0x1234567890, 1, 2, v12::PAUSE_RUN, 12, 120, now, "This is a title",
        2
    );
    const v12::StateChangeItemBody* p =
        reinterpret_cast<const v12::StateChangeItemBody*>(item.getBodyPointer());
    const v12::StateChangeItem* pItem =
        reinterpret_cast<const v12::StateChangeItem*>(item.getItemPointer());
    EQ(&(pItem->s_body.u_hasBodyHeader.s_body), p);
}
// get body pointer (non const) has body header:

void v12statetest::getbody_4()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        0x1234567890, 1, 2, v12::PAUSE_RUN, 12, 120, now, "This is a title",
        2
    );
     v12::StateChangeItemBody* p =
        reinterpret_cast< v12::StateChangeItemBody*>(item.getBodyPointer());
     v12::StateChangeItem* pItem =
        reinterpret_cast< v12::StateChangeItem*>(item.getItemPointer());
    EQ(&(pItem->s_body.u_hasBodyHeader.s_body), p);
}
// hasBody Header false for non body header.

void v12statetest::hashdr_1()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        v12::BEGIN_RUN, 1234, 10, now, "This is a title" 
    );
    ASSERT(!item.hasBodyHeader());
}
// hasBodyHeader true if there is one:
void v12statetest::hashdr_2()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        0x1234567890, 1, 2, v12::PAUSE_RUN, 12, 120, now, "This is a title",
        2
    );
    ASSERT(item.hasBodyHeader());
}
// getBodyHeader - nullptr if none
void v12statetest::gethdr_1()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        v12::BEGIN_RUN, 1234, 10, now, "This is a title" 
    );
    ASSERT(nullptr == item.getBodyHeader());
}
// getBodyHeader - non null correct value if there is one.
void v12statetest::gethdr_2()
{
    time_t now = time(nullptr);
    v12::CRingStateChangeItem item(
        0x1234567890, 1, 2, v12::PAUSE_RUN, 12, 120, now, "This is a title",
        2
    );
    const v12::BodyHeader* p =
        reinterpret_cast<const v12::BodyHeader*>(item.getBodyHeader());
    ASSERT(p != nullptr);
    const v12::StateChangeItem* pItem =
        reinterpret_cast<const v12::StateChangeItem*>(item.getItemPointer());
    EQ(&(pItem->s_body.u_hasBodyHeader.s_bodyHeader), p);
}