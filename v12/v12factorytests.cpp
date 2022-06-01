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
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>   // where memfd_create really  lives.
#include <sys/types.h>
#include <sstream>      // For get std::istream e..


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