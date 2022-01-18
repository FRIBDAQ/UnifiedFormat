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
    CPPUNIT_TEST(getbodysize_2);
    
    CPPUNIT_TEST(getbodypointer_1);
    CPPUNIT_TEST(getbodypointer_2);
    
    CPPUNIT_TEST(getbodycursor_1);
    CPPUNIT_TEST(getbodycursor_2);
    
    CPPUNIT_TEST(getitempointer_1);
    CPPUNIT_TEST(getitempointer_2);
    
    CPPUNIT_TEST(type_1);    // The type is right for items made using
    CPPUNIT_TEST(type_2);    // any of the three
    CPPUNIT_TEST(type_3);    // constructors!
    
    CPPUNIT_TEST(size_1);
    CPPUNIT_TEST(size_2);
    CPPUNIT_TEST(size_3);
    
    CPPUNIT_TEST(mustswap);
    CPPUNIT_TEST(hasbodyheader);
    
    CPPUNIT_TEST(gettimestamp);   // These throw std::string
    CPPUNIT_TEST(getsourceid);
    CPPUNIT_TEST(getbarriertype);
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
    void getbodysize_2();
    
    void getbodypointer_1();
    void getbodypointer_2();
    
    void getbodycursor_1();
    void getbodycursor_2();
    
    void getitempointer_1();
    void getitempointer_2();
    
    void type_1();
    void type_2();
    void type_3();
    
    void size_1();
    void size_2();
    void size_3();
    
    void mustswap();
    void hasbodyheader();
    
    void gettimestamp();
    void getsourceid();
    void getbarriertype();
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
// nonempty body:
void abringitemtest::getbodysize_2()
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
    EQ(sizeof(rawItem.payload), item.getBodySize());
    
}

// const
void abringitemtest::getbodypointer_1()
{
    CTestRingItem item(PHYSICS_EVENT);
    const void* pBodyPointer = item.getBodyPointer();
    const RingItemHeader* pH        = reinterpret_cast<const RingItemHeader*>(item.m_pItem);
    pH++;           /// Should be the body pointer in this impl.
    
    EQ(pBodyPointer, reinterpret_cast<const void*>(pH));
}
// non const
void abringitemtest::getbodypointer_2()
{
    CTestRingItem item(PHYSICS_EVENT);
    void* pBodyPointer = item.getBodyPointer();
    RingItemHeader* pH        = reinterpret_cast<RingItemHeader*>(item.m_pItem);
    pH++;           /// Should be the body pointer in this impl.
    
    EQ(pBodyPointer, reinterpret_cast<void*>(pH));    
}
// empty body get body cursor:

void abringitemtest::getbodycursor_1()
{
    CTestRingItem item(PHYSICS_EVENT);
    EQ(item.getBodyPointer(), item.getBodyCursor());
}
// nonempty body cursor:

void abringitemtest::getbodycursor_2()
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
    
    const uint8_t* pBody = reinterpret_cast<const uint8_t*>(item.getBodyPointer());
    uint8_t* pCursor  = reinterpret_cast<uint8_t*>(item.getBodyCursor());
    size_t n = pCursor - pBody;
    EQ(sizeof(rawItem.payload), n);
}
// get item pointer for static buffer:

void abringitemtest::getitempointer_1()
{
    CTestRingItem item(PHYSICS_EVENT);
    
    // Below are really 2 different method calls.
    
    uint8_t* p1 = reinterpret_cast<uint8_t*>(item.getItemPointer());
    const uint8_t* p2 = reinterpret_cast<const uint8_t*>(item.getItemPointer());
    
    EQ(const_cast<const uint8_t*>(p1), p2);
    EQ(p2, reinterpret_cast<const uint8_t*>(&item.m_staticBuffer[0]));
    EQ(p1, reinterpret_cast<uint8_t*>(item.m_pItem));
}
// get item pointer when new was required.

void abringitemtest::getitempointer_2()
{
    CTestRingItem item(PHYSICS_EVENT, 2*CRingItemStaticBufferSize);
    ASSERT(
        item.getItemPointer() !=
        reinterpret_cast<pRingItem>(&(item.m_staticBuffer[0]))
    );
    pRingItem p1 = item.getItemPointer();
    const RingItem* p2 = item.getItemPointer();   // const
    EQ(const_cast<const RingItem*>(p1), p2);
}
// type correct for first type constructor:

void abringitemtest::type_1()
{
    CTestRingItem item(PHYSICS_EVENT);
    EQ(PHYSICS_EVENT, item.type());
}
// type correct on copy construction.
void abringitemtest::type_2()
{
    CTestRingItem item1(PHYSICS_EVENT);
    CTestRingItem item(item1);
    EQ(PHYSICS_EVENT, item.type());
}
// Type correct on raw construction.

void abringitemtest::type_3()
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
    EQ(PHYSICS_EVENT, item.type());
}
// 'normal' constructor.

void abringitemtest::size_1()
{
    CTestRingItem item(PHYSICS_EVENT);
    EQ(sizeof(RingItemHeader), size_t(item.size()));
}
// Copy constructor

void abringitemtest::size_2()
{
    CTestRingItem item(PHYSICS_EVENT);
    CTestRingItem i2(item);
    EQ(sizeof(RingItemHeader), size_t(i2.size()));
}
// Raw constructor
void abringitemtest::size_3()
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
    
    EQ(rawItem.hdr.s_size, item.size());
}
// must swap is hard-coded to false:

void abringitemtest::mustswap()
{
    CTestRingItem item(PHYSICS_EVENT);
    ASSERT(!item.mustSwap());
}
// hasbodyheader is hard-coded false.
void abringitemtest::hasbodyheader()
{
    CTestRingItem item(PHYSICS_EVENT);
    ASSERT(!item.hasBodyHeader());

}
// in the base type the three tests below should see std::string
// exceptions.

void abringitemtest::gettimestamp()
{
    CTestRingItem item(PHYSICS_EVENT);
    CPPUNIT_ASSERT_THROW(item.getEventTimestamp(), std::string);
}
void abringitemtest::getsourceid()
{
    CTestRingItem item(PHYSICS_EVENT);
    CPPUNIT_ASSERT_THROW(item.getSourceId(), std::string);
}
void abringitemtest::getbarriertype()
{
    CTestRingItem item(PHYSICS_EVENT);
    CPPUNIT_ASSERT_THROW(item.getBarrierType(), std::string);
}