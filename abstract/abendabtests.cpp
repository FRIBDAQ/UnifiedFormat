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

/** @file:  abendabtests.cpp
 *  @brief: tests for the abstract abnormal end item.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CAbnormalEndItem.h"
#include <stdexcept>

class abendabtest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(abendabtest);
    CPPUNIT_TEST(typeName);
    CPPUNIT_TEST(getbodyheader);
    CPPUNIT_TEST_SUITE_END();
    
private:

public:
    void setUp() {
        
    }
    void tearDown() {
        
    }
protected:
    void typeName();
    void getbodyheader();
};

CPPUNIT_TEST_SUITE_REGISTRATION(abendabtest);

void abendabtest::typeName()
{
    CAbnormalEndItem item;
    EQ(std::string("Abnormal End"), item.typeName());
}
// these two should throw logic errors;

void abendabtest::getbodyheader()
{
    CAbnormalEndItem item;
    ASSERT(item.getBodyHeader() == nullptr);
}
