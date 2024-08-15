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

/** @file:  physabtests.cpp
 *  @brief: Test abstract CPhysicsEventItem
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CPhysicsEventItem.h"
#include <stdexcept>
#include <string>

using namespace ufmt;

class phyabtest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(phyabtest);
    CPPUNIT_TEST(name);
    CPPUNIT_TEST(getBodyHeader);
    CPPUNIT_TEST(setBodyHeader);
    CPPUNIT_TEST_SUITE_END();
    
private:
    CPhysicsEventItem* m_pItem;
public:
    void setUp() {
        m_pItem = new CPhysicsEventItem;
    }
    void tearDown() {
        delete m_pItem;        
    }
protected:
    void name();
    void getBodyHeader();
    void setBodyHeader();
};

CPPUNIT_TEST_SUITE_REGISTRATION(phyabtest);

void phyabtest::name()
{
    EQ(std::string("Event"), m_pItem->typeName());
}

void phyabtest::getBodyHeader()
{
    CPPUNIT_ASSERT_THROW(m_pItem->getBodyHeader(), std::logic_error);
}
void phyabtest::setBodyHeader()
{
    CPPUNIT_ASSERT_THROW(
        m_pItem->setBodyHeader(12345, 0, 0),
        std::logic_error
    );
}