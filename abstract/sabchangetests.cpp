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

/** @file:  sabchangetests.cpp
 *  @brief: 
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CRingStateChangeItem.h"
#include <stdexcept>
#include <time.h>
#include <string.h>

class abschangetest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(abschangetest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    CPPUNIT_TEST(construct_3);
    
    CPPUNIT_TEST(run_1);
    CPPUNIT_TEST(run_2);
    
    CPPUNIT_TEST(elapsed_1);
    CPPUNIT_TEST(elapsed_2);
    CPPUNIT_TEST(elapsed_3);
    CPPUNIT_TEST(elapsed_4);
    
    CPPUNIT_TEST(title_1);
    CPPUNIT_TEST(title_2);
    
    CPPUNIT_TEST(sid);
    
    CPPUNIT_TEST(stamp_1);
    CPPUNIT_TEST(stamp_2);
    
    CPPUNIT_TEST(bodyhdr_1);
    CPPUNIT_TEST(bodyhdr_2);
    
    CPPUNIT_TEST(typenames);
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
    
    void elapsed_1();
    void elapsed_2();
    void elapsed_3();
    void elapsed_4();
    
    void title_1();
    void title_2();
    
    void sid();
    
    void stamp_1();
    void stamp_2();
    
    void bodyhdr_1();
    void bodyhdr_2();
    
    void typenames();
};

CPPUNIT_TEST_SUITE_REGISTRATION(abschangetest);

// Construct only with reason:
void abschangetest::construct_1()
{
    CPPUNIT_ASSERT_NO_THROW(CRingStateChangeItem i1(BEGIN_RUN));
    CPPUNIT_ASSERT_NO_THROW(CRingStateChangeItem i2(END_RUN));
    CPPUNIT_ASSERT_NO_THROW(CRingStateChangeItem i3(PAUSE_RUN));
    CPPUNIT_ASSERT_NO_THROW(CRingStateChangeItem i4(RESUME_RUN));
    
    CPPUNIT_ASSERT_THROW(
        CRingStateChangeItem item(PHYSICS_EVENT),
        std::logic_error
    );
}
// construct with reason the body needs to be right:

void abschangetest::construct_2()
{
    
    CRingStateChangeItem item(BEGIN_RUN);
    time_t now = time(nullptr);
    
    const RingItemHeader* pH = reinterpret_cast<const RingItemHeader*>(item.getItemPointer());
    EQ(BEGIN_RUN, pH->s_type);
    
    const uint32_t* p32 = reinterpret_cast<const uint32_t*>(pH+1);
    EQ(uint32_t(0), *p32); p32++;   // run
    EQ(uint32_t(0), *p32); p32++;   // offset
    ASSERT(now - *p32 <= 1); p32++; // CLock time.
    EQ(uint32_t(1), *p32); p32++;    // divisor.
    EQ(uint32_t(0), *p32); p32++;    // original sid.
    
    const uint8_t* p8 = reinterpret_cast<const uint8_t*>(p32);
    EQ(uint8_t(0), *p8);
}

// Full construction.

void abschangetest::construct_3()
{
    time_t now = time(nullptr);
    CRingStateChangeItem item(
        END_RUN, 12, 100, now, "This is my title"
    );
     const RingItemHeader* pH = reinterpret_cast<const RingItemHeader*>(item.getItemPointer());
    EQ(END_RUN, pH->s_type);
    
    const uint32_t* p32 = reinterpret_cast<const uint32_t*>(pH+1);
    EQ(uint32_t(12), *p32); p32++;
    EQ(uint32_t(100), *p32); p32++;
    EQ(uint32_t(now), *p32); p32++;
    EQ(uint32_t(1), *p32); p32++;    // divisor.
    EQ(uint32_t(0), *p32); p32++;    // original sid.
    
    EQ(0, strcmp("This is my title", reinterpret_cast<const char*>(p32)));
}
// Can fetch the run out with getRunNumber:
void abschangetest::run_1()
{
    time_t now = time(nullptr);
    CRingStateChangeItem item(
        END_RUN, 12, 100, now, "This is my title"
    );
    EQ(uint32_t(12), item.getRunNumber());
}
// canset run number with setRunNumber:

void abschangetest::run_2()
{
    CRingStateChangeItem item(BEGIN_RUN);
    item.setRunNumber(1234);
    EQ(uint32_t(1234), item.getRunNumber());
}

// Can get the elapsed time.

void abschangetest::elapsed_1()
{
    time_t now = time(nullptr);
    CRingStateChangeItem item(
        END_RUN, 12, 100, now, "This is my title"
    );
    EQ(uint32_t(100), item.getElapsedTime());
}
// Can set elapsed time:

void abschangetest::elapsed_2()
{
    time_t now = time(nullptr);
    CRingStateChangeItem item(
        END_RUN, 12, 100, now, "This is my title"
    );
    
    item.setElapsedTime(200);
    EQ(uint32_t(200), item.getElapsedTime());
}
// Divisor is 1.

void abschangetest::elapsed_3()
{
    time_t now = time(nullptr);
    CRingStateChangeItem item(
        END_RUN, 12, 100, now, "This is my title"
    );
    EQ(uint32_t(1), item.getTimeDivisor());
}
// Elapsed time computed:

void abschangetest::elapsed_4()
{
    time_t now = time(nullptr);
    CRingStateChangeItem item(
        END_RUN, 12, 100, now, "This is my title"
    );
    EQ(float(100.0), item.computeElapsedTime());
}

// Can get title:
void abschangetest::title_1()
{
    time_t now = time(nullptr);
    CRingStateChangeItem item(
        END_RUN, 12, 100, now, "This is my title"
    );
    EQ(std::string("This is my title"), item.getTitle());
}
void abschangetest::title_2()
{
    time_t now = time(nullptr);
    CRingStateChangeItem item(
        END_RUN, 12, 100, now, "This is my title"
    );
    
    item.setTitle("Changed Title");
    EQ(std::string("Changed Title"), item.getTitle());
}
// Sid is 0:

void abschangetest::sid()
{
    time_t now = time(nullptr);
    CRingStateChangeItem item(
        END_RUN, 12, 100, now, "This is my title"
    );
    EQ(uint32_t(0), item.getOriginalSourceId());
}

// Can fetch clock timestamp:

void
abschangetest::stamp_1()
{
    time_t now = time(nullptr);
    CRingStateChangeItem item(
        END_RUN, 12, 100, now, "This is my title"
    );
    EQ(now, item.getTimestamp());
}
// Can set timestamp

void abschangetest::stamp_2()
{
    time_t now = time(nullptr);
    CRingStateChangeItem item(
        END_RUN, 12, 100, now, "This is my title"
    );
    item.setTimestamp(now+10);
    EQ(now+10, item.getTimestamp());
}

// Both body  header functions throw logic_errors

void abschangetest::bodyhdr_1()
{
    time_t now = time(nullptr);
    CRingStateChangeItem item(
        END_RUN, 12, 100, now, "This is my title"
    );
    CPPUNIT_ASSERT_THROW(
        item.getBodyHeader(),
        std::logic_error
    );
}
void abschangetest::bodyhdr_2()
{
    time_t now = time(nullptr);
    CRingStateChangeItem item(
        END_RUN, 12, 100, now, "This is my title"
    );
    CPPUNIT_ASSERT_THROW(
        item.setBodyHeader(1245, 1, 1),
        std::logic_error
    );
}

// check type names.

void abschangetest::typenames()
{
    time_t now = time(nullptr);
    CRingStateChangeItem item1(
        END_RUN, 12, 100, now, "This is my title"
    );
    EQ(std::string("End Run"), item1.typeName());
    
    CRingStateChangeItem item2(
        BEGIN_RUN, 12, 100, now, "This is my title"
    );
    EQ(std::string("Begin Run"), item2.typeName());
    
    
    CRingStateChangeItem item3(
        PAUSE_RUN, 12, 100, now, "This is my title"
    );
    EQ(std::string("Pause Run"), item3.typeName());
    
    
    CRingStateChangeItem item4(
        RESUME_RUN, 12, 100, now, "This is my title"
    );
    EQ(std::string("Resume Run"), item4.typeName());
}

