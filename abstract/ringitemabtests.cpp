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

/** @file:  ringitemabtests.cpp
 *  @brief: Test the abstract ring item.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#define private public
#define protected public
#include "CRingItem.h"
#undef private
#undef public
#include <DataFormat.h>
#include <stdint.h>

/** Since the CRingItem class is abstract we need a minimal concrete sublcass
 * to test any of it.
 */

class CTestRingItem : public CRingItem {
public:
    CTestRingItem(pRingItem pItem) : CRingItem(pItem) {}
    CTestRingItem(uint16_t type, size_t maxBody = CRingItemStaticBufferSize -100) :
        CRingItem(type, maxBody) {}
    CTestRingItem(const CTestRingItem& rhs) : CRingItem(rhs) {}
    virtual void* getBodyHeader() const {return nullptr;}
    virtual void setBodyHeader(uint64_t timestamp, uint32_t sourceId,
                         uint32_t barrierType = 0) {}
};


class abringitemtest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(abringitemtest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    CPPUNIT_TEST(construct_3);
    CPPUNIT_TEST(construct_4);
    
    // Selector tests:
    
    CPPUNIT_TEST(getstoragesize_1);
    CPPUNIT_TEST(getstoragesize_2);
    CPPUNIT_TEST(getstoragesize_3);
    
    CPPUNIT_TEST(getbodysize_1);
    CPPUNIT_TEST_SUITE_END();
    
private:

public:
    void setUp() {
        
    }
    void tearDown() {
        
    }
protected:
    void construct_1();   // From raw items:
    void construct_2();
    
    void construct_3();   // From type/size.
    void construct_4();   // Copy construction.
    
    void getstoragesize_1();
    void getstoragesize_2();
    void getstoragesize_3();  // Force allocation.
    
    void getbodysize_1();
};

CPPUNIT_TEST_SUITE_REGISTRATION(abringitemtest);

// Simplest construction is from an existing empty raw ring item.
void abringitemtest::construct_1()
{
    RingItem r;
    r.s_header.s_size = sizeof(RingItemHeader);
    r.s_header.s_type = PHYSICS_EVENT;
    
    CTestRingItem item(&r);
    EQ(sizeof(RingItemHeader), size_t(item.m_pItem->s_header.s_size));
    EQ(PHYSICS_EVENT, item.m_pItem->s_header.s_type);
    // The cursor should be just past the header.
    
    uint8_t* pH = reinterpret_cast<uint8_t*>(item.m_pItem);
    pH += r.s_header.s_size;
    uint8_t* pC = reinterpret_cast<uint8_t*>(item.m_pCursor);
    EQ(pH, pC);
}
// Ring item with a payload:

void abringitemtest::construct_2()
{
    struct myitem {
        RingItemHeader hdr;
        uint8_t payload[100];
    } rawItem;
    for (int i =0; i < 100; i++) {
        rawItem.payload[i] = i;
    }
    rawItem.hdr.s_size= sizeof(rawItem);
    rawItem.hdr.s_type= PHYSICS_EVENT;
    
    CTestRingItem item(reinterpret_cast<pRingItem>(&rawItem));
    EQ(sizeof(rawItem), size_t(item.m_pItem->s_header.s_size));
    EQ(PHYSICS_EVENT, item.m_pItem->s_header.s_type);
    
    pRingItemHeader pH = reinterpret_cast<pRingItemHeader>(item.m_pItem);
    uint8_t* pPayload = reinterpret_cast<uint8_t*>(pH+1);
    for (int i =0; i < 100; i++) {
        EQ(rawItem.payload[i], *pPayload);
        pPayload++;
    }
    // pPayload should match the cursor
    //
    EQ(pPayload, reinterpret_cast<uint8_t*>(item.m_pCursor));
    
}

// Construct from type and (default)maxbody:

void abringitemtest::construct_3()
{
    CTestRingItem item(PHYSICS_EVENT);
    item.updateSize();            // Sets the header size field.
    
    EQ(sizeof(RingItemHeader), size_t(item.m_pItem->s_header.s_size));
    EQ(PHYSICS_EVENT, item.m_pItem->s_header.s_type);
    
    // Cursor should point just after the header:
    
    pRingItemHeader pH = reinterpret_cast<pRingItemHeader>(item.m_pItem);
    uint8_t* pB = reinterpret_cast<uint8_t*>(pH+1);
    EQ(pB, reinterpret_cast<uint8_t*>(item.m_pCursor));
    EQ(CRingItemStaticBufferSize-100, item.m_storageSize);
    
}
// copy construction test:

void abringitemtest::construct_4()
{
    struct myitem {
        RingItemHeader hdr;
        uint8_t payload[100];
    } rawItem;
    for (int i =0; i < 100; i++) {
        rawItem.payload[i] = i;
    }
    rawItem.hdr.s_size= sizeof(rawItem);
    rawItem.hdr.s_type= PHYSICS_EVENT;
    
    CTestRingItem item1(reinterpret_cast<pRingItem>(&rawItem));
    CTestRingItem item(item1);
    
    // all the construct_2 tests should be valid:
    
    EQ(sizeof(rawItem), size_t(item.m_pItem->s_header.s_size));
    EQ(PHYSICS_EVENT, item.m_pItem->s_header.s_type);
    
    pRingItemHeader pH = reinterpret_cast<pRingItemHeader>(item.m_pItem);
    uint8_t* pPayload = reinterpret_cast<uint8_t*>(pH+1);
    for (int i =0; i < 100; i++) {
        EQ(rawItem.payload[i], *pPayload);
        pPayload++;
    }
    // pPayload should match the cursor
    //
    EQ(pPayload, reinterpret_cast<uint8_t*>(item.m_pCursor));
    
}

//  default storage:

void abringitemtest::getstoragesize_1()
{
    CTestRingItem item(PHYSICS_EVENT);
    EQ(size_t(CRingItemStaticBufferSize -100), item.getStorageSize());
}
void abringitemtest::getstoragesize_2()
{
    CTestRingItem item(PHYSICS_EVENT, 100);
    EQ(size_t(100), item.getStorageSize());
}
void abringitemtest::getstoragesize_3()
{
    CTestRingItem item(PHYSICS_EVENT, CRingItemStaticBufferSize*2);
    EQ(size_t(CRingItemStaticBufferSize*2), item.getStorageSize());
    
    // m_pItem better not point at the static buffer:
    
    CPPUNIT_ASSERT(item.m_pItem != pRingItem(&item.m_staticBuffer[0]));
}
// empty body:

void abringitemtest::getbodysize_1()
{
    CTestRingItem item(PHYSICS_EVENT);
    item.updateSize();               // Probably don't need this.
    EQ(size_t(0), item.getBodySize());
}