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

/** @file:  v10rbuftests.cpp
 *  @brief: 
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CRingItem.h"
#include "DataFormat.h"
#include <type_traits>    //is_null_pointer
using namespace ufmt;
class v10rbuftest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v10rbuftest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(bodyptr_1);
    CPPUNIT_TEST(bodyptr_2);
    
    CPPUNIT_TEST(bodysize_1);
    CPPUNIT_TEST(bodysize_2);
    
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
    
    void bodyptr_1();
    void bodyptr_2();
    
    void bodysize_1();
    void bodysize_2();
    
    void bodyhdr_1();
};

CPPUNIT_TEST_SUITE_REGISTRATION(v10rbuftest);

void v10rbuftest::construct_1()
{
    v10::CRingItem item(v10::BEGIN_RUN);
    EQ(v10::BEGIN_RUN, item.type());
}

// body pointer should be just after the v10 header:

void v10rbuftest::bodyptr_1()
{
    v10::CRingItem item(v10::BEGIN_RUN);
    void* pItem = item.getItemPointer();
    void* pBody = item.getBodyPointer();
    
    void *pSbBody = (reinterpret_cast<v10::RingItemHeader*>(pItem) + 1);
    EQ(pSbBody, pBody);
}
// Same as above with const

void v10rbuftest::bodyptr_2()
{
    v10::CRingItem item(v10::BEGIN_RUN);
    void* pItem = item.getItemPointer();
    const void* pBody = item.getBodyPointer();
    
    const void *pSbBody = (reinterpret_cast<v10::RingItemHeader*>(pItem) + 1);
    EQ(pSbBody, pBody);
}
// Initially body size is 0.

void v10rbuftest::bodysize_1()
{
    v10::CRingItem item(v10::BEGIN_RUN);
    EQ(size_t(0), item.getBodySize());
    
}
// Put some bytes and the body size changes:

void v10rbuftest::bodysize_2()
{
    v10::CRingItem item(v10::BEGIN_RUN);
    uint8_t* p = reinterpret_cast<uint8_t*>(item.getBodyCursor());
    p += 10;
    item.setBodyCursor(p);
    item.updateSize();
    EQ(size_t(10), item.getBodySize());
}
// body header is null:

void v10rbuftest::bodyhdr_1()
{
    v10::CRingItem item(v10::BEGIN_RUN);
    
    ASSERT(item.getBodyHeader() == nullptr);
}