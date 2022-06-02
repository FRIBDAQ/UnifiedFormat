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

/** @file:  v12factorytests.cpp
 *  @brief: Tests for the V12 object factory.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <memory>
#include "DataFormat.h"
#include <DataFormat.h>
#include "RingItemFactory.h"
#include "CRingItem.h"
#include <CRingBuffer.h>
#include <CAbnormalEndItem.h>
#include "CDataFormatItem.h"   // need the v12
#include "CGlomParameters.h"
#include "CPhysicsEventItem.h"
#include <CRingFragmentItem.h>
#include <CRingPhysicsEventCountItem.h>
#include <CRingScalerItem.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>   // where memfd_create really  lives.
#include <sys/types.h>
#include <sstream>      // For get std::istream e..
#include <time.h>


// In our tests, we use std::unique_ptr to ensure there's not
// any memory leaking.


class v12facttest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v12facttest);
    CPPUNIT_TEST(mkringitem_1);
    CPPUNIT_TEST(mkringitem_2);
    CPPUNIT_TEST(mkringitem_3);
    CPPUNIT_TEST(mkringitem_4);
    CPPUNIT_TEST(mkringitem_5);
    CPPUNIT_TEST(mkringitem_6);
    
    CPPUNIT_TEST(get_1);
    CPPUNIT_TEST(get_2);
    CPPUNIT_TEST(get_3);
    CPPUNIT_TEST(get_4);
    CPPUNIT_TEST(get_5);
    CPPUNIT_TEST(get_6);
    
    CPPUNIT_TEST(put_1);
    CPPUNIT_TEST(put_2);
    CPPUNIT_TEST(put_3);
    CPPUNIT_TEST(put_4);
    CPPUNIT_TEST(put_5);
    CPPUNIT_TEST(put_6);
    
    CPPUNIT_TEST(abend_1);
    CPPUNIT_TEST(abend_2);
    CPPUNIT_TEST(abend_3);
    
    CPPUNIT_TEST(fmt_1);
    CPPUNIT_TEST(fmt_2);
    CPPUNIT_TEST(fmt_3);
    
    CPPUNIT_TEST(glompar_1);
    CPPUNIT_TEST(glompar_2);
    CPPUNIT_TEST(glompar_3);
    
    CPPUNIT_TEST(mkphys_1);
    CPPUNIT_TEST(mkphys_2);
    CPPUNIT_TEST(mkphys_3);
    CPPUNIT_TEST(mkphys_4);
    CPPUNIT_TEST(mkphys_5);
    
    CPPUNIT_TEST(frag_1);
    CPPUNIT_TEST(frag_2);
    CPPUNIT_TEST(frag_3);
    CPPUNIT_TEST(frag_4);
    
    CPPUNIT_TEST(ecount_1);
    CPPUNIT_TEST(ecount_2);
    CPPUNIT_TEST(ecount_3);
    
    CPPUNIT_TEST(scaler_1);
    CPPUNIT_TEST(scaler_2);
    CPPUNIT_TEST(scaler_3);
    CPPUNIT_TEST(scaler_4);
    CPPUNIT_TEST_SUITE_END();
    
private:
    v12::RingItemFactory* m_pFactory;
    CRingBuffer*          m_pProducer;
    CRingBuffer*          m_pConsumer;
public:
    void setUp() {
        m_pFactory = new v12::RingItemFactory;
        m_pProducer = CRingBuffer::createAndProduce("v12factory");
        m_pConsumer = new CRingBuffer("v12factory");
    }
    void tearDown() {
        delete m_pFactory;
        delete m_pProducer;
        delete m_pConsumer;
        try {
            CRingBuffer::remove("v12factory");
        }
        catch (...) {}
        
    }
protected:
    void mkringitem_1();
    void mkringitem_2();
    void mkringitem_3();
    void mkringitem_4();
    void mkringitem_5();
    void mkringitem_6();
    
    void get_1();
    void get_2();
    void get_3();
    void get_4();
    void get_5();
    void get_6();
    
    void put_1();
    void put_2();
    void put_3();
    void put_4();
    void put_5();
    void put_6();
    
    void abend_1();
    void abend_2();
    void abend_3();
    
    void fmt_1();
    void fmt_2();
    void fmt_3();
    
    void glompar_1();
    void glompar_2();
    void glompar_3();
    
    void mkphys_1();
    void mkphys_2();
    void mkphys_3();
    void mkphys_4();
    void mkphys_5();
        
    void frag_1();
    void frag_2();
    void frag_3();
    void frag_4();
    
    void ecount_1();
    void ecount_2();
    void ecount_3();
    
    void scaler_1();
    void scaler_2();
    void scaler_3();
    void scaler_4();
};

CPPUNIT_TEST_SUITE_REGISTRATION(v12facttest);

// Empty ring item with capacity:

void v12facttest::mkringitem_1()
{
    std::unique_ptr<::CRingItem> p(m_pFactory->makeRingItem(v12::PHYSICS_EVENT, 100));
    EQ(uint32_t(v12::PHYSICS_EVENT), p->type());
    EQ(sizeof(v12::RingItemHeader) + sizeof(uint32_t), size_t(p->size()));
    
    const v12::RingItem* pItem =
        reinterpret_cast<const v12::RingItem*>(p->getItemPointer());
    EQ(
       sizeof(v12::RingItemHeader) + sizeof(uint32_t),
       size_t(pItem->s_header.s_size)
    );
    EQ(v12::PHYSICS_EVENT, pItem->s_header.s_type);
    EQ(sizeof(uint32_t), size_t(pItem->s_body.u_noBodyHeader.s_empty));
}
// Ring item with body header:

void v12facttest::mkringitem_2()
{
    std::unique_ptr<::CRingItem> p(
        m_pFactory->makeRingItem(v12::PHYSICS_EVENT, 0x1234567890, 2, 100, 1)
    );
    EQ(uint32_t(v12::PHYSICS_EVENT), p->type());
    EQ(sizeof(v12::RingItemHeader) + sizeof(v12::BodyHeader), size_t(p->size()));
    ASSERT(p->hasBodyHeader());
    EQ(uint64_t(0x1234567890), p->getEventTimestamp());
    EQ(uint32_t(2), p->getSourceId());
    EQ(uint32_t(1), p->getBarrierType());
    CPPUNIT_ASSERT_NO_THROW(
        v12::CRingItem* p12 = dynamic_cast<v12::CRingItem*>(p.get())
    );
    
}
// Make ring item from another item. (no body header).

void v12facttest::mkringitem_3()
{
    std::unique_ptr<::CRingItem> src(m_pFactory->makeRingItem(v12::PHYSICS_EVENT, 100));
    uint8_t* pBody = reinterpret_cast<uint8_t*>(src->getBodyPointer());
    for(int i =0; i < 10; i++) {
        *pBody++ = i;
    }
    src->setBodyCursor(pBody);
    src->updateSize();
    size_t srcSize = src->size();
    
    std::unique_ptr<::CRingItem> cpy(m_pFactory->makeRingItem(*src));
    EQ(srcSize, size_t(cpy->size()));
    EQ(src->type(), cpy->type());
    ASSERT(!cpy->hasBodyHeader());
    
    pBody = reinterpret_cast<uint8_t*>(src->getBodyPointer());
    uint8_t* p = reinterpret_cast<uint8_t*>(cpy->getBodyPointer());
    for (int i =0; i < 10; i++) {
        EQ(*pBody, *p);
        p++; pBody++;
    }
    
    
}
// Copy an item with a body header:
void v12facttest::mkringitem_4()
{
    std::unique_ptr<::CRingItem> src(
        m_pFactory->makeRingItem(
            v12::PHYSICS_EVENT, 0x1234567890, 1, 100, 2
        )
    );
    uint8_t* pBody = reinterpret_cast<uint8_t*>(src->getBodyPointer());
    for(int i =0; i < 10; i++) {
        *pBody++ = i;
    }
    src->setBodyCursor(pBody);
    src->updateSize();
    size_t srcSize = src->size();
    
    std::unique_ptr<::CRingItem> cpy(m_pFactory->makeRingItem(*src));
    EQ(srcSize, size_t(cpy->size()));
    EQ(src->type(), cpy->type());
    ASSERT(cpy->hasBodyHeader());
    
    EQ(src->getEventTimestamp(), cpy->getEventTimestamp());
    EQ(src->getSourceId(), cpy->getSourceId());
    EQ(src->getBarrierType(), cpy->getBarrierType());
    
    pBody = reinterpret_cast<uint8_t*>(src->getBodyPointer());
    uint8_t* p = reinterpret_cast<uint8_t*>(cpy->getBodyPointer());
    for (int i =0; i < 10; i++) {
        EQ(*pBody, *p);
        p++; pBody++;
    }
    
    
}

// Make ring item from another raw item. (no body header).

void v12facttest::mkringitem_5()
{
    std::unique_ptr<::CRingItem> src(m_pFactory->makeRingItem(v12::PHYSICS_EVENT, 100));
    uint8_t* pBody = reinterpret_cast<uint8_t*>(src->getBodyPointer());
    for(int i =0; i < 10; i++) {
        *pBody++ = i;
    }
    src->setBodyCursor(pBody);
    src->updateSize();
    size_t srcSize = src->size();
    
    const ::RingItem* pItem = reinterpret_cast<const ::RingItem*>(
        src->getItemPointer()
    );
    std::unique_ptr<::CRingItem> cpy(m_pFactory->makeRingItem(pItem));
    EQ(srcSize, size_t(cpy->size()));
    EQ(src->type(), cpy->type());
    ASSERT(!cpy->hasBodyHeader());
    
    pBody = reinterpret_cast<uint8_t*>(src->getBodyPointer());
    uint8_t* p = reinterpret_cast<uint8_t*>(cpy->getBodyPointer());
    for (int i =0; i < 10; i++) {
        EQ(*pBody, *p);
        p++; pBody++;
    }
    
    
}
// Copy an item with a body header:
void v12facttest::mkringitem_6()
{
    std::unique_ptr<::CRingItem> src(
        m_pFactory->makeRingItem(
            v12::PHYSICS_EVENT, 0x1234567890, 1, 100, 2
        )
    );
    uint8_t* pBody = reinterpret_cast<uint8_t*>(src->getBodyPointer());
    for(int i =0; i < 10; i++) {
        *pBody++ = i;
    }
    src->setBodyCursor(pBody);
    src->updateSize();
    size_t srcSize = src->size();
    
    const ::RingItem* pItem = reinterpret_cast<const ::RingItem*>(
        src->getItemPointer()
    );
    
    std::unique_ptr<::CRingItem> cpy(m_pFactory->makeRingItem(pItem));
    EQ(srcSize, size_t(cpy->size()));
    EQ(src->type(), cpy->type());
    ASSERT(cpy->hasBodyHeader());
    
    EQ(src->getEventTimestamp(), cpy->getEventTimestamp());
    EQ(src->getSourceId(), cpy->getSourceId());
    EQ(src->getBarrierType(), cpy->getBarrierType());
    
    pBody = reinterpret_cast<uint8_t*>(src->getBodyPointer());
    uint8_t* p = reinterpret_cast<uint8_t*>(cpy->getBodyPointer());
    for (int i =0; i < 10; i++) {
        EQ(*pBody, *p);
        p++; pBody++;
    }
    
    
}

// Get a ring item from a ringbufer (no body header).
void v12facttest::get_1()
{
    
    std::unique_ptr<::CRingItem> src(
        m_pFactory->makeRingItem(v12::PHYSICS_EVENT, 100)
    );
    uint8_t* p = reinterpret_cast<uint8_t*>(src->getBodyPointer());
    for (int i = 0; i < 10; i++) {
        *p++ = i;
    }
    src->setBodyCursor(p);
    src->updateSize();
    
    // put the item to the ring buffer
    
    m_pProducer->put(src->getItemPointer(), src->size());
    
    // get it out:
    
    std::unique_ptr<::CRingItem> cpy(m_pFactory->getRingItem(*m_pConsumer));
    EQ(src->size(), cpy->size());
    EQ(0, memcmp(src->getItemPointer(), cpy->getItemPointer(), src->size()));
}

// Get a ring item with a body header from a ringbuffer.
void v12facttest::get_2()
{
    std::unique_ptr<::CRingItem> src(
        m_pFactory->makeRingItem(v12::PHYSICS_EVENT, 0x1234567890, 1, 100, 2)
    );
    uint8_t* p = reinterpret_cast<uint8_t*>(src->getBodyPointer());
    for (int i = 0; i < 10; i++) {
        *p++ = i;
    }
    src->setBodyCursor(p);
    src->updateSize();
    
    // put the item to the ring buffer
    
    m_pProducer->put(src->getItemPointer(), src->size());
    
    // get it out:
    
    std::unique_ptr<::CRingItem> cpy(m_pFactory->getRingItem(*m_pConsumer));
    EQ(src->size(), cpy->size());
    EQ(0, memcmp(src->getItemPointer(), cpy->getItemPointer(), src->size()));
}
// Get from fd (memfd will be used.)

void v12facttest::get_3()
{
    
    std::unique_ptr<::CRingItem> src(
        m_pFactory->makeRingItem(v12::PHYSICS_EVENT, 100)
    );
    uint8_t* p = reinterpret_cast<uint8_t*>(src->getBodyPointer());
    for (int i = 0; i < 10; i++) {
        *p++ = i;
    }
    src->setBodyCursor(p);
    src->updateSize();
    
    // put the item to the ring buffer
    
    int fd = memfd_create("testing", 0);
    write(fd, src->getItemPointer(), src->size());
    lseek(fd, SEEK_SET, 0);
    
    
    // get it out:
    
    std::unique_ptr<::CRingItem> cpy(m_pFactory->getRingItem(fd));
    close(fd);
    EQ(src->size(), cpy->size());
    EQ(0, memcmp(src->getItemPointer(), cpy->getItemPointer(), src->size()));
    
}
// Get from fd with body header.

void v12facttest::get_4()
{
    std::unique_ptr<::CRingItem> src(
        m_pFactory->makeRingItem(v12::PHYSICS_EVENT, 0x1234567890, 1, 100, 2)
    );
    uint8_t* p = reinterpret_cast<uint8_t*>(src->getBodyPointer());
    for (int i = 0; i < 10; i++) {
        *p++ = i;
    }
    src->setBodyCursor(p);
    src->updateSize();
    
    // put the item to the ring buffer
    
    int fd = memfd_create("testing", 0);
    write(fd, src->getItemPointer(), src->size());
    lseek(fd, SEEK_SET, 0);
    
    
    // get it out:
    
    std::unique_ptr<::CRingItem> cpy(m_pFactory->getRingItem(fd));
    close(fd);
    EQ(src->size(), cpy->size());
    EQ(0, memcmp(src->getItemPointer(), cpy->getItemPointer(), src->size()));
    
}
// Get non body header from istream.
void v12facttest::get_5()
{
    std::unique_ptr<::CRingItem> src(
        m_pFactory->makeRingItem(v12::PHYSICS_EVENT, 100)
    );
    uint8_t* p = reinterpret_cast<uint8_t*>(src->getBodyPointer());
    for (int i = 0; i < 10; i++) {
        *p++ = i;
    }
    src->setBodyCursor(p);
    src->updateSize();
    
    // put the item to the ring buffer
    
    std::stringstream s;
    s.write(reinterpret_cast<char*>(src->getItemPointer()), src->size());
    s.seekp(0);
    
    
    // get it out:
    
    std::unique_ptr<::CRingItem> cpy(m_pFactory->getRingItem(s));
    
    EQ(src->size(), cpy->size());
    EQ(0, memcmp(src->getItemPointer(), cpy->getItemPointer(), src->size()));
    
}
// get body header item from istream

void v12facttest::get_6()
{
    std::unique_ptr<::CRingItem> src(
        m_pFactory->makeRingItem(v12::PHYSICS_EVENT, 0x1234567890, 1, 100, 2)
    );
    uint8_t* p = reinterpret_cast<uint8_t*>(src->getBodyPointer());
    for (int i = 0; i < 10; i++) {
        *p++ = i;
    }
    src->setBodyCursor(p);
    src->updateSize();
    
    // put the item to the ring buffer
    
    std::stringstream s;
    s.write(reinterpret_cast<char*>(src->getItemPointer()), src->size());
    s.seekp(0);
    
    
    // get it out:
    
    std::unique_ptr<::CRingItem> cpy(m_pFactory->getRingItem(s));
    
    EQ(src->size(), cpy->size());
    EQ(0, memcmp(src->getItemPointer(), cpy->getItemPointer(), src->size()));
    
}
// Put non body header item into ringbuffer:

void v12facttest::put_1()
{
    std::unique_ptr<::CRingItem> src(
        m_pFactory->makeRingItem(v12::PHYSICS_EVENT, 100)
    );
    uint8_t* p = reinterpret_cast<uint8_t*>(src->getBodyPointer());
    for (int i = 0; i < 10; i++) {
        *p++ = i;
    }
    src->setBodyCursor(p);
    src->updateSize();
    
    // put the item to the ring buffer
    
    m_pFactory->putRingItem(src.get(), *m_pProducer);
    
    // get it out:
    
    std::unique_ptr<::CRingItem> cpy(m_pFactory->getRingItem(*m_pConsumer));
    EQ(src->size(), cpy->size());
    EQ(0, memcmp(src->getItemPointer(), cpy->getItemPointer(), src->size()));
}
// Put body header item into ring buffer.

void v12facttest::put_2()
{
    std::unique_ptr<::CRingItem> src(
        m_pFactory->makeRingItem(v12::PHYSICS_EVENT, 0x1234567890, 1, 100, 2)
    );
    uint8_t* p = reinterpret_cast<uint8_t*>(src->getBodyPointer());
    for (int i = 0; i < 10; i++) {
        *p++ = i;
    }
    src->setBodyCursor(p);
    src->updateSize();
    
    // put the item to the ring buffer
    
    m_pFactory->putRingItem(src.get(), *m_pProducer);
    
    // get it out:
    
    std::unique_ptr<::CRingItem> cpy(m_pFactory->getRingItem(*m_pConsumer));
    EQ(src->size(), cpy->size());
    EQ(0, memcmp(src->getItemPointer(), cpy->getItemPointer(), src->size()));
}
// Put non body header item in file descriptor.

void v12facttest::put_3()
{
    std::unique_ptr<::CRingItem> src(
        m_pFactory->makeRingItem(v12::PHYSICS_EVENT, 100)
    );
    uint8_t* p = reinterpret_cast<uint8_t*>(src->getBodyPointer());
    for (int i = 0; i < 10; i++) {
        *p++ = i;
    }
    src->setBodyCursor(p);
    src->updateSize();
    
    // put the item to the ring buffer
    
    int fd = memfd_create("testing", 0);
    m_pFactory->putRingItem(src.get(), fd);
    lseek(fd, SEEK_SET, 0);
    
    
    // get it out:
    
    std::unique_ptr<::CRingItem> cpy(m_pFactory->getRingItem(fd));
    close(fd);
    EQ(src->size(), cpy->size());
    EQ(0, memcmp(src->getItemPointer(), cpy->getItemPointer(), src->size()));
}
// put body header item in file descriptor.
void v12facttest::put_4()
{
    std::unique_ptr<::CRingItem> src(
        m_pFactory->makeRingItem(v12::PHYSICS_EVENT, 0x1234567890, 1, 100, 2)
    );
    uint8_t* p = reinterpret_cast<uint8_t*>(src->getBodyPointer());
    for (int i = 0; i < 10; i++) {
        *p++ = i;
    }
    src->setBodyCursor(p);
    src->updateSize();
    
    // put the item to the ring buffer
    
    int fd = memfd_create("testing", 0);
    m_pFactory->putRingItem(src.get(), fd);
    lseek(fd, SEEK_SET, 0);
    
    
    // get it out:
    
    std::unique_ptr<::CRingItem> cpy(m_pFactory->getRingItem(fd));
    close(fd);
    EQ(src->size(), cpy->size());
    EQ(0, memcmp(src->getItemPointer(), cpy->getItemPointer(), src->size()));
    
}
void v12facttest::put_5()
{
    std::unique_ptr<::CRingItem> src(
        m_pFactory->makeRingItem(v12::PHYSICS_EVENT, 100)
    );
    uint8_t* p = reinterpret_cast<uint8_t*>(src->getBodyPointer());
    for (int i = 0; i < 10; i++) {
        *p++ = i;
    }
    src->setBodyCursor(p);
    src->updateSize();
    
    // put the item to the ring buffer
    
    std::stringstream s;
    m_pFactory->putRingItem(src.get(), s);
    s.seekp(0);
    
    
    // get it out:
    
    std::unique_ptr<::CRingItem> cpy(m_pFactory->getRingItem(s));
    
    EQ(src->size(), cpy->size());
    EQ(0, memcmp(src->getItemPointer(), cpy->getItemPointer(), src->size()));
    
}
void v12facttest::put_6()
{
    std::unique_ptr<::CRingItem> src(
        m_pFactory->makeRingItem(v12::PHYSICS_EVENT, 0x1234567890, 1, 100, 2)
    );
    uint8_t* p = reinterpret_cast<uint8_t*>(src->getBodyPointer());
    for (int i = 0; i < 10; i++) {
        *p++ = i;
    }
    src->setBodyCursor(p);
    src->updateSize();
    
    // put the item to the ring buffer
    
    std::stringstream s;
    m_pFactory->putRingItem(src.get(), s);
    s.seekp(0);
    
    
    // get it out:
    
    std::unique_ptr<::CRingItem> cpy(m_pFactory->getRingItem(s));
    
    EQ(src->size(), cpy->size());
    EQ(0, memcmp(src->getItemPointer(), cpy->getItemPointer(), src->size()));
    
}
// abnormal end from nothing:

void v12facttest::abend_1()
{
    std::unique_ptr<::CAbnormalEndItem> p(m_pFactory->makeAbnormalEndItem());
    
    EQ(v12::ABNORMAL_ENDRUN, p->type());
    EQ(sizeof(v12::AbnormalEndItem), size_t(p->size()));
    ASSERT(!p->hasBodyHeader());
}
// abend from proper ring item

void v12facttest::abend_2()
{
    std::unique_ptr<::CAbnormalEndItem> src(m_pFactory->makeAbnormalEndItem());
    
    std::unique_ptr<::CAbnormalEndItem> p;
    CPPUNIT_ASSERT_NO_THROW(
        p.reset(m_pFactory->makeAbnormalEndItem(*src))
    );
    EQ(src->size(), p->size());
    EQ(0, memcmp(p->getItemPointer(), src->getItemPointer(), p->size()));
}

// invalid type makes a std::bad_cast

void v12facttest::abend_3()
{
    std::unique_ptr<::CRingItem> src1(m_pFactory->makeRingItem(v12::PHYSICS_EVENT, 100));
    
    CPPUNIT_ASSERT_THROW(
        m_pFactory->makeAbnormalEndItem(*src1), std::bad_cast
    );
    // Bad sizes:
    
    std::unique_ptr<CRingItem> src2(m_pFactory->makeRingItem(
        v12::ABNORMAL_ENDRUN, 0X1234567890, 1, 100, 2)
    ); // Bad size!
    
    CPPUNIT_ASSERT_THROW(
        m_pFactory->makeAbnormalEndItem(*src2), std::bad_cast
    );
    
}
// Make a data format item.

void v12facttest::fmt_1()
{
    std::unique_ptr<::CDataFormatItem> item(m_pFactory->makeDataFormatItem());
    EQ(v12::RING_FORMAT, item->type());
    EQ(sizeof(v12::DataFormat), size_t(item->size()));
    EQ(uint16_t(12), item->getMajor());
}
// copy a data format item (legally).

void v12facttest::fmt_2()
{
    std::unique_ptr<::CDataFormatItem> item(m_pFactory->makeDataFormatItem());
    std::unique_ptr<::CDataFormatItem> copy;
    CPPUNIT_ASSERT_NO_THROW(
        copy.reset(m_pFactory->makeDataFormatItem(*item))
    );
    EQ(v12::RING_FORMAT, copy->type());
    EQ(sizeof(v12::DataFormat), size_t(copy->size()));
    EQ(uint16_t(12), copy->getMajor());
}
// Various illegal ring item copy constructions for makeFormatItem.

void v12facttest::fmt_3()
{
    // wrong type:
    
    v12::CRingItem badtype(v12::PHYSICS_EVENT, 100);
    CPPUNIT_ASSERT_THROW(
        m_pFactory->makeDataFormatItem(badtype), std::bad_cast
    );
    // bad version number
    
    v12::CDataFormatItem badvsn;
    v12::pDataFormat p = reinterpret_cast<v12::pDataFormat>(badvsn.getItemPointer());
    p->s_majorVersion--;
    
    CPPUNIT_ASSERT_THROW(
        m_pFactory->makeDataFormatItem(badvsn), std::bad_cast
    );
}
// Make glom parameters item from parameters.

void v12facttest::glompar_1()
{
    std::unique_ptr<::CGlomParameters> item(m_pFactory->makeGlomParameters(100, true, 1));
    EQ(v12::EVB_GLOM_INFO, item->type());
    EQ(sizeof(v12::GlomParameters), size_t(item->size()));
    EQ(uint64_t(100), item->coincidenceTicks());
    ASSERT(item->isBuilding());
    EQ(::CGlomParameters::last, item->timestampPolicy());
}
// make glom parameters as a  copy:

void v12facttest::glompar_2()
{
    v12::CGlomParameters original(100, true, ::CGlomParameters::last);
    std::unique_ptr<::CGlomParameters> item;
    CPPUNIT_ASSERT_NO_THROW(
        item.reset(m_pFactory->makeGlomParameters(original))
    );
    
    EQ(v12::EVB_GLOM_INFO, item->type());
    EQ(sizeof(v12::GlomParameters), size_t(item->size()));
    EQ(uint64_t(100), item->coincidenceTicks());
    ASSERT(item->isBuilding());
    EQ(::CGlomParameters::last, item->timestampPolicy());
}
// Illegal attempts to copy an item to a glom parameters fail

void v12facttest::glompar_3()
{
    // Bad type:
    
    v12::CRingItem badtype(v12::PHYSICS_EVENT, 100);
    CPPUNIT_ASSERT_THROW(
        m_pFactory->makeGlomParameters(badtype), std::bad_cast
    );
    // Bad size:
    
    v12::CGlomParameters badsize(100, true, ::CGlomParameters::last);
    uint8_t* p = reinterpret_cast<uint8_t*>(badsize.getBodyCursor());
    p++;                    // this is safe even badsize is 'shrink wrapped'.
    badsize.setBodyCursor(p); // Since only the header size is modified.
    badsize.updateSize();
    CPPUNIT_ASSERT_THROW(
        m_pFactory->makeGlomParameters(badsize), std::bad_cast
    );
}
// Make a real physics item (no body header).

void v12facttest::mkphys_1()
{
    std::unique_ptr<::CPhysicsEventItem> item(
        m_pFactory->makePhysicsEventItem(100)
    );
    EQ(v12::PHYSICS_EVENT, item->type());
    EQ(sizeof(v12::RingItemHeader) + sizeof(uint32_t), size_t(item->size()));
    ASSERT(!item->hasBodyHeader());
}
// Make a real physics event with a body header:

void v12facttest::mkphys_2()
{
    std::unique_ptr<::CPhysicsEventItem> item(
        m_pFactory->makePhysicsEventItem(0x1234567890, 1, 2, 100)
    );
    EQ(v12::PHYSICS_EVENT, item->type());
    EQ(sizeof(v12::RingItemHeader) + sizeof(v12::BodyHeader), size_t(item->size()));
    ASSERT(item->hasBodyHeader());
    EQ(uint64_t(0x1234567890), item->getEventTimestamp());
    EQ(uint32_t(1), item->getSourceId());
    EQ(uint32_t(2), item->getBarrierType());
}
// Copy a non body header physics item (success).

void v12facttest::mkphys_3()
{
    v12::CPhysicsEventItem original;
    std::unique_ptr<::CPhysicsEventItem> item(m_pFactory->makePhysicsEventItem(original));
    EQ(v12::PHYSICS_EVENT, item->type());
    EQ(sizeof(v12::RingItemHeader) + sizeof(uint32_t), size_t(item->size()));
    ASSERT(!item->hasBodyHeader());
    
}
// copy a  physics event with a body header (success).

void v12facttest::mkphys_4()
{
    v12::CPhysicsEventItem original(0x1234567890, 1, 2, 100);
    std::unique_ptr<::CPhysicsEventItem> item(m_pFactory->makePhysicsEventItem(original));
    EQ(v12::PHYSICS_EVENT, item->type());
    EQ(sizeof(v12::RingItemHeader) + sizeof(v12::BodyHeader), size_t(item->size()));
    ASSERT(item->hasBodyHeader());
    EQ(uint64_t(0x1234567890), item->getEventTimestamp());
    EQ(uint32_t(1), item->getSourceId());
    EQ(uint32_t(2), item->getBarrierType());
    
}
// making a physics item by copying a non physics item is an error:

void v12facttest::mkphys_5()
{
    std::unique_ptr<::CAbnormalEndItem> badtype(m_pFactory->makeAbnormalEndItem());
    CPPUNIT_ASSERT_THROW(
        m_pFactory->makePhysicsEventItem(*badtype), std::bad_cast
    );
}
//fragment item with no payload.

void v12facttest::frag_1()
{
    std::unique_ptr<::CRingFragmentItem> item(
        m_pFactory->makeRingFragmentItem(0x1234567890, 1, 0, nullptr, 2)
    );
    EQ(v12::EVB_FRAGMENT, item->type());
    EQ(sizeof(v12::EventBuilderFragment), size_t(item->size()));
    ASSERT(item->hasBodyHeader());
    EQ(uint64_t(0x1234567890), item->getEventTimestamp());
    EQ(uint32_t(1), item->getSourceId());
    EQ(uint32_t(2), item->getBarrierType());
}
// fragment itemw with a payload:

void v12facttest::frag_2()
{
    uint8_t payload[100];
    for (int i=0; i < 100; i++) {
        payload[i] = i;
    }
    std::unique_ptr<::CRingFragmentItem> item(
        m_pFactory->makeRingFragmentItem(
            0x1234567890, 1, sizeof(payload), payload, 2
        )
    );
    EQ(v12::EVB_FRAGMENT, item->type());
    EQ(
       sizeof(v12::EventBuilderFragment) + sizeof(payload),
       size_t(item->size())
    );
    ASSERT(item->hasBodyHeader());
    EQ(uint64_t(0x1234567890), item->getEventTimestamp());
    EQ(uint32_t(1), item->getSourceId());
    EQ(uint32_t(2), item->getBarrierType());
    EQ(sizeof(payload), item->payloadSize());
    uint8_t* p = reinterpret_cast<uint8_t*>(item->payloadPointer());
    for (int i=0; i < sizeof(payload); i++) {
        EQ(payload[i], *p);
        p++;
    }
}
// Good copy of a fragment item:

void v12facttest::frag_3()
{
    uint8_t payload[100];
    for (int i=0; i < 100; i++) {
        payload[i] = i;
    }
    std::unique_ptr<::CRingFragmentItem> original(
        m_pFactory->makeRingFragmentItem(
            0x1234567890, 1, sizeof(payload), payload, 2
        )
    );
    std::unique_ptr<::CRingFragmentItem> item;
    CPPUNIT_ASSERT_NO_THROW(
        item.reset(m_pFactory->makeRingFragmentItem(*original))
    );
    EQ(v12::EVB_FRAGMENT, item->type());
    EQ(
       sizeof(v12::EventBuilderFragment) + sizeof(payload),
       size_t(item->size())
    );
    ASSERT(item->hasBodyHeader());
    EQ(uint64_t(0x1234567890), item->getEventTimestamp());
    EQ(uint32_t(1), item->getSourceId());
    EQ(uint32_t(2), item->getBarrierType());
    EQ(sizeof(payload), item->payloadSize());
    uint8_t* p = reinterpret_cast<uint8_t*>(item->payloadPointer());
    for (int i=0; i < sizeof(payload); i++) {
        EQ(payload[i], *p);
        p++;
    }
}
// copying a non fragment item fails:
void v12facttest::frag_4()
{
    std::unique_ptr<::CAbnormalEndItem> original(
        m_pFactory->makeAbnormalEndItem()
    );
    CPPUNIT_ASSERT_THROW(
        m_pFactory->makeRingFragmentItem(*original), std::bad_cast
    );
}
// Make a count item from parameters:

void v12facttest::ecount_1()
{
    time_t now = time(nullptr);
    std::unique_ptr<::CRingPhysicsEventCountItem> item(
        m_pFactory->makePhysicsEventCountItem(111111, 10, now)
    );
    
    EQ(v12::PHYSICS_EVENT_COUNT, item->type());
    EQ(
       sizeof(v12::RingItemHeader) + sizeof(v12::BodyHeader)
       + sizeof(v12::PhysicsEventCountItemBody),
       size_t(item->size())
    );
    ASSERT(item->hasBodyHeader());
    EQ(uint32_t(10), item->getTimeOffset());
    EQ(now, item->getTimestamp());
    EQ(uint64_t(111111), item->getEventCount());
    
}
// Copy from original item is fine:

void v12facttest::ecount_2()
{
    time_t now = time(nullptr);
    std::unique_ptr<::CRingPhysicsEventCountItem> original(
        m_pFactory->makePhysicsEventCountItem(111111, 10, now)
    );
    
    std::unique_ptr<::CRingPhysicsEventCountItem> item;
    CPPUNIT_ASSERT_NO_THROW(
        item.reset(m_pFactory->makePhysicsEventCountItem(*original))
    );
    EQ(v12::PHYSICS_EVENT_COUNT, item->type());
    EQ(
       sizeof(v12::RingItemHeader) + sizeof(v12::BodyHeader)
       + sizeof(v12::PhysicsEventCountItemBody),
       size_t(item->size())
    );
    ASSERT(item->hasBodyHeader());
    EQ(uint32_t(10), item->getTimeOffset());
    EQ(now, item->getTimestamp());
    EQ(uint64_t(111111), item->getEventCount());
}
// Copy from non event count is a bad  cast.
void v12facttest::ecount_3()
{
    std::unique_ptr<::CAbnormalEndItem> badtype(m_pFactory->makeAbnormalEndItem());
    CPPUNIT_ASSERT_THROW(
        m_pFactory->makePhysicsEventCountItem(*badtype), std::bad_cast
    );
    
    
}
// Simple scaler creation:

void v12facttest::scaler_1()
{
    auto now = time(nullptr);
    
    std::unique_ptr<::CRingScalerItem> item(
        m_pFactory->makeScalerItem(32)
    );
    EQ(v12::PERIODIC_SCALERS, item->type());
    ASSERT(!item->hasBodyHeader());
    EQ(
        sizeof(v12::RingItemHeader) + sizeof(uint32_t) +
        sizeof(v12::ScalerItemBody) + 32 * sizeof(uint32_t),
        size_t(item->size())
    );
    EQ(uint32_t(0), item->getStartTime());
    EQ(uint32_t(0), item->getEndTime());
    ASSERT(item->getTimestamp() - now <= 1);
    ASSERT(item->isIncremental());
    EQ(uint32_t(32), item->getScalerCount());
    for (int i = 0; i < 32; i++) {
        EQ(uint32_t(0), item->getScaler(i));
    }
}
// Full scaler creation - note this makes  a body header.

void v12facttest::scaler_2()
{
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    auto now = time(nullptr);
    std::unique_ptr<::CRingScalerItem> item(
        m_pFactory->makeScalerItem(
            10, 20, now, scalers,true, 2
        )
    );
    EQ(v12::PERIODIC_SCALERS, item->type());
    ASSERT(item->hasBodyHeader());
    EQ(
        sizeof(v12::RingItemHeader) + sizeof(v12::BodyHeader) +
        sizeof(v12::ScalerItemBody) + 32 * sizeof(uint32_t),
        size_t(item->size())
    );
    EQ(uint32_t(10), item->getStartTime());
    EQ(uint32_t(20), item->getEndTime());
    EQ(now, item->getTimestamp());
    ASSERT(item->isIncremental());
    EQ(uint32_t(32), item->getScalerCount());
    for (int i = 0; i < 32; i++) {
        EQ(scalers[i], item->getScaler(i));
    }
    EQ(uint32_t(2), item->getOriginalSourceId());
    
    
    
}
// Construct valid copy:

void v12facttest::scaler_3()
{
    std::vector<uint32_t> scalers;
    for (int i =0; i < 32; i++) {
        scalers.push_back(i*100);
    }
    auto now = time(nullptr);
    std::unique_ptr<::CRingScalerItem>  original(
        m_pFactory->makeScalerItem(
            10, 20, now, scalers,true, 2
        )
    );
    
    std::unique_ptr<::CRingScalerItem> item;
    CPPUNIT_ASSERT_NO_THROW(
        item.reset(m_pFactory->makeScalerItem(*original))
    );
    
    EQ(v12::PERIODIC_SCALERS, item->type());
    ASSERT(item->hasBodyHeader());
    EQ(
        sizeof(v12::RingItemHeader) + sizeof(v12::BodyHeader) +
        sizeof(v12::ScalerItemBody) + 32 * sizeof(uint32_t),
        size_t(item->size())
    );
    EQ(uint32_t(10), item->getStartTime());
    EQ(uint32_t(20), item->getEndTime());
    EQ(now, item->getTimestamp());
    ASSERT(item->isIncremental());
    EQ(uint32_t(32), item->getScalerCount());
    for (int i = 0; i < 32; i++) {
        EQ(scalers[i], item->getScaler(i));
    }
    EQ(uint32_t(2), item->getOriginalSourceId());
    
    
}
// Invalid cop construction.

void v12facttest::scaler_4()
{
    std::unique_ptr<::CAbnormalEndItem> original(m_pFactory->makeAbnormalEndItem());
    CPPUNIT_ASSERT_THROW(
        m_pFactory->makeScalerItem(*original), std::bad_cast
    );
}