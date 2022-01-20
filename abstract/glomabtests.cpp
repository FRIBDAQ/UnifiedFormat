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

/** @file:  glomabtests.cpp
 *  @brief: Test the abstract version of CGlomParameters.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CGlomParameters.h"
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <stdexcept>


class glomabtest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(glomabtest);
    CPPUNIT_TEST(ticks);
    CPPUNIT_TEST(isbuilding_1);
    CPPUNIT_TEST(isbuilding_2);
    CPPUNIT_TEST(tspolicy_1);
    CPPUNIT_TEST(tspolicy_2);
    CPPUNIT_TEST(tspolicy_3);
    CPPUNIT_TEST(name);
    CPPUNIT_TEST(getbodyheader);
    CPPUNIT_TEST(setbodyheader);
    CPPUNIT_TEST_SUITE_END();
    
private:
    
public:
    void setUp() {
        
    }
    void tearDown() {
        
    }
protected:
    void ticks();
    void isbuilding_1();
    void isbuilding_2();
    void tspolicy_1();
    void tspolicy_2();
    void tspolicy_3();
    void name();
    void getbodyheader();
    void setbodyheader();
};

CPPUNIT_TEST_SUITE_REGISTRATION(glomabtest);


void glomabtest::ticks()
{
    CGlomParameters p(100, true, CGlomParameters::first);
    EQ(uint64_t(100), p.coincidenceTicks());
}
// Test possible states of the building flag:

void glomabtest::isbuilding_1()
{
    CGlomParameters p(100, true, CGlomParameters::first);
    ASSERT(p.isBuilding());
}
void glomabtest::isbuilding_2()
{
    CGlomParameters p(100, false, CGlomParameters::first);
    ASSERT(!p.isBuilding());
}
// Test timestamp policy states.

void glomabtest::tspolicy_1()
{
    CGlomParameters p(100, true, CGlomParameters::first);
    EQ(CGlomParameters::first, p.timestampPolicy());
}
void glomabtest::tspolicy_2()
{
    CGlomParameters p(100, true, CGlomParameters::last);
    EQ(CGlomParameters::last, p.timestampPolicy());
}
void glomabtest::tspolicy_3()
{
    CGlomParameters p(100, true, CGlomParameters::average);
    EQ(CGlomParameters::average, p.timestampPolicy());
}
void glomabtest::name()
{
    CGlomParameters p(100, true, CGlomParameters::average);
    EQ(std::string("Glom Parameters"), p.typeName());
}

void glomabtest::getbodyheader()
{
    CGlomParameters p(100, true, CGlomParameters::average);
    CPPUNIT_ASSERT_THROW(p.getBodyHeader(), std::logic_error);
}
void glomabtest::setbodyheader()
{
    CGlomParameters p(100, true, CGlomParameters::average);
    CPPUNIT_ASSERT_THROW(p.setBodyHeader(1245, 0, 0), std::logic_error);
}