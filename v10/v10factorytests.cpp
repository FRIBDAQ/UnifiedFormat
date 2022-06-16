
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

/** @file:  v10factorytests.cpp
 *  @brief: Test the V10 ring item factory.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "RingItemFactory.h"
#include "DataFormat.h"
#include <CRingBuffer.h>
#include "CRingItem.h"
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <fstream>
#include <stdexcept>
#include <ios>
#include <typeinfo>
#include <time.h>

#include "CPhysicsEventItem.h"
#include "CRingPhysicsEventCountItem.h"
#include "CRingScalerItem.h"
#include "CRingTextItem.h"
#include "CRingStateChangeItem.h"
#include "CRingFragmentItem.h"
#include <memory>

// A comment about all the try catch blocks:
// ASSERTIONS that fail trigger an exception so we catch all
// exceptions and delete any dynamically created ring item and then
// rethrow allowing CPPUNIT to report the assertion failure.
// SO there's a recurring pattern of:
//
//   ::CsomeRingItemType* pItem(0);
//   try {
//      pItem = makesometypeofitem(....);
//      ... assertions
//   }
//   catch (...) {
//     delete pItem;
//     throw;
//   }
///  delete pItem;
//
//   in all of our tests.

static const char* ringName="v10FactoryRing";
static const char* tempFileTemplate="v10FactoryXXXXXX";

class v10factorytest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v10factorytest);
    CPPUNIT_TEST(ring_1);
    CPPUNIT_TEST(ring_2);
    CPPUNIT_TEST(ring_3);
    CPPUNIT_TEST(ring_4);
    CPPUNIT_TEST(ring_5);
    CPPUNIT_TEST(ring_6);
    CPPUNIT_TEST(ring_7);
    CPPUNIT_TEST(ring_8);
    CPPUNIT_TEST(ring_9);
    CPPUNIT_TEST(ring_10);
    
    CPPUNIT_TEST(abend_1);
    CPPUNIT_TEST(abend_2);
    
    CPPUNIT_TEST(fmt_1);
    CPPUNIT_TEST(fmt_2);
    
    CPPUNIT_TEST(glompar_1);
    CPPUNIT_TEST(glompar_2);
    
    CPPUNIT_TEST(phys_1);
    CPPUNIT_TEST(phys_2);
    CPPUNIT_TEST(phys_3);
    CPPUNIT_TEST(phys_4);
    
    CPPUNIT_TEST(frag_1);
    CPPUNIT_TEST(frag_2);
    CPPUNIT_TEST(frag_3);
    
    CPPUNIT_TEST(count_1);
    CPPUNIT_TEST(count_2);
    CPPUNIT_TEST(count_3);
    CPPUNIT_TEST(count_4);
    
    CPPUNIT_TEST(scaler_1);
    CPPUNIT_TEST(scaler_2);
    CPPUNIT_TEST(scaler_3);
    CPPUNIT_TEST(scaler_4);
    CPPUNIT_TEST(scaler_5);
    CPPUNIT_TEST(scaler_6);
    CPPUNIT_TEST(scaler_7);
    CPPUNIT_TEST(scaler_8);
    
    CPPUNIT_TEST(text_1);
    CPPUNIT_TEST(text_2);
    CPPUNIT_TEST(text_3);
    CPPUNIT_TEST(text_4);
    CPPUNIT_TEST(text_5);
    CPPUNIT_TEST(text_6);
    CPPUNIT_TEST(text_7);
    CPPUNIT_TEST(text_8);
    
    CPPUNIT_TEST(unknown_1);
    CPPUNIT_TEST(unknown_2);
    
    CPPUNIT_TEST(state_1);
    CPPUNIT_TEST(state_2);
    CPPUNIT_TEST(state_3);
    CPPUNIT_TEST(state_4);
    CPPUNIT_TEST(state_5);
    CPPUNIT_TEST(state_6);
    CPPUNIT_TEST(state_7);
    CPPUNIT_TEST(state_8);
    CPPUNIT_TEST_SUITE_END();
    
protected:
    void ring_1();
    void ring_2();
    void ring_3();
    void ring_4();
    void ring_5();
    void ring_6();
    void ring_7();
    void ring_8();
    void ring_9();
    void ring_10();
    
    void abend_1();
    void abend_2();
    
    void fmt_1();
    void fmt_2();
    
    void glompar_1();
    void glompar_2();
    
    void phys_1();
    void phys_2();
    void phys_3();
    void phys_4();
    
    void frag_1();
    void frag_2();
    void frag_3();
    
    void count_1();
    void count_2();
    void count_3();
    void count_4();
    
    void scaler_1();
    void scaler_2();
    void scaler_3();
    void scaler_4();
    void scaler_5();
    void scaler_6();
    void scaler_7();
    void scaler_8();
    
    void text_1();
    void text_2();
    void text_3();
    void text_4();
    void text_5();
    void text_6();
    void text_7();
    void text_8();
    
    void unknown_1();
    void unknown_2();
    
    void state_1();
    void state_2();
    void state_3();
    void state_4();
    void state_5();
    void state_6();
    void state_7();
    void state_8();
    private:
    v10::RingItemFactory* m_pFactory;
    CRingBuffer*          m_pProducer;
    CRingBuffer*          m_pConsumer;
public:
    void setUp() {
        if ( CRingBuffer::isRing(ringName)) {
            CRingBuffer::remove(ringName);
        }
        m_pProducer = CRingBuffer::createAndProduce(ringName);
        m_pConsumer = new CRingBuffer(ringName);
        
        m_pFactory = new v10::RingItemFactory;
    }
    void tearDown() {
        delete m_pFactory;
        delete m_pProducer;
        delete m_pConsumer;
        CRingBuffer::remove(ringName);
    }

private:
    std::pair<std::string, int> makeTempFile();
};

CPPUNIT_TEST_SUITE_REGISTRATION(v10factorytest);

// private utilities:

/**
 * makeTempFile
 *    Makes a temporary file (needed to attach streams):
 * @return std::pair<std::string, int>  The string is the
 *    name of the file and the int is the file descriptor.
 *    Errors throw std::runtime_error
 *    The caller must close the file.
 */
std::pair<std::string, int>
v10factorytest::makeTempFile()
{
    char nameBuffer[strlen(tempFileTemplate) + 1];
    strncpy(nameBuffer, tempFileTemplate, strlen(tempFileTemplate) + 1);
    
    int fd = mkstemp(nameBuffer);
    if (fd == -1) {
        throw std::runtime_error("Failed to make temp file");
    }
    return std::pair<std::string, int>(std::string(nameBuffer), fd);
            

}

// 'normal' CRingItem creation.

void v10factorytest::ring_1()
{
    CRingItem* item(0);
    try {
        item = m_pFactory->makeRingItem(v10::PHYSICS_EVENT, 100);
        EQ(v10::PHYSICS_EVENT, item->type());
        EQ(sizeof(v10::RingItemHeader), size_t(item->size()));
        EQ(size_t(100), item->getStorageSize());
    } catch (...) {
        delete item;
        throw;
    }
    delete item;
}
// advanced CRingItem create looks like normal.
void
v10factorytest::ring_2()
{
CRingItem* item(0);
    try {
        item = m_pFactory->makeRingItem(
            v10::PHYSICS_EVENT, uint64_t(1234675890), 0, 100
        );
        EQ(v10::PHYSICS_EVENT, item->type());
        EQ(sizeof(v10::RingItemHeader), size_t(item->size()));
        EQ(size_t(100), item->getStorageSize());
    } catch (...) {
        delete item;
        throw;
    }
    delete item;    
}
// Make a ring item from an existing CRingItem object:

void
v10factorytest::ring_3()
{
    v10::CRingItem src(PHYSICS_EVENT, 100);
    uint16_t* p = reinterpret_cast<uint16_t*>(src.getBodyCursor());
    for (int i =0; i < 10; i++) {
        *p++ = i;
    }
    src.setBodyCursor(p); src.updateSize();
    
    
    ::CRingItem* pItem(0);
    try {
        pItem = m_pFactory->makeRingItem(src);
        v10::pRingItemHeader p1 = reinterpret_cast<v10::pRingItemHeader>(src.getItemPointer());
        ::pRingItemHeader p2 = reinterpret_cast<::pRingItemHeader>(pItem->getItemPointer());
        
        EQ(p1->s_size, p2->s_size);
        EQ(0, memcmp(p1, p2, p1->s_size));
    }
    catch (...) {
        delete pItem;
        throw;
    }
    
    delete pItem;
}
// from raw ring item struct:

void
v10factorytest::ring_4()
{
#pragma packed(push, 1)
    struct {
        v10::RingItemHeader s_header;
        uint16_t             s_body[100];
    } rawItem;
#pragma packed(pop)
    rawItem.s_header.s_type = PHYSICS_EVENT;
    rawItem.s_header.s_size = sizeof(rawItem);
    for (int i =0; i < 100; i++) {
        rawItem.s_body[i] = i;
    }
    ::CRingItem* pItem(0);
    try {
        pItem = m_pFactory->makeRingItem(reinterpret_cast<const ::RingItem*>(&rawItem));
        EQ(rawItem.s_header.s_size, pItem->size());
        
        EQ(0, memcmp(
            &rawItem, pItem->getItemPointer(), rawItem.s_header.s_size
        ));
    }
    catch(...) {
        delete pItem;
        throw;
    }
    delete pItem;
}
// Test getRingItem with ring buffer.

void
v10factorytest::ring_5()
{
#pragma packed(push, 1)
    struct {
        v10::RingItemHeader s_header;
        uint16_t             s_body[100];
    } rawItem;
#pragma packed(pop)
    rawItem.s_header.s_type = PHYSICS_EVENT;
    rawItem.s_header.s_size = sizeof(rawItem);
    for (int i =0; i < 100; i++) {
        rawItem.s_body[i] = i;
    }
    
    m_pProducer->put(&rawItem, rawItem.s_header.s_size);
    
    ::CRingItem* pItem(0);
    try {
        pItem = m_pFactory->getRingItem(*m_pConsumer);
        ASSERT(pItem);
        EQ(rawItem.s_header.s_size, pItem->size());
        EQ(0, memcmp(
            &rawItem, pItem->getItemPointer(), rawItem.s_header.s_size
        ));
    }
    catch(...) {
        delete pItem;
        throw;
    }
    delete pItem;
}
// get ring item from an fd:
void
v10factorytest::ring_6()
{
#pragma packed(push, 1)
    struct {
        v10::RingItemHeader s_header;
        uint16_t             s_body[100];
    } rawItem;
#pragma packed(pop)
    rawItem.s_header.s_type = PHYSICS_EVENT;
    rawItem.s_header.s_size = sizeof(rawItem);
    for (int i =0; i < 100; i++) {
        rawItem.s_body[i] = i;
    }
    
    int fd = memfd_create("TestFile", 0);
    write(fd, &rawItem, sizeof(rawItem));
    lseek(fd, 0, SEEK_SET);            // Rewinde:
    
    ::CRingItem* pItem(0);
    try {
        pItem = m_pFactory->getRingItem(fd);
        EQ(rawItem.s_header.s_size, pItem->size());
        EQ(0, memcmp(&rawItem, pItem->getItemPointer(), sizeof(rawItem)));
    }
    catch (...) {
        close(fd);
        delete pItem;
        throw;
    }
    
    close(fd);
    delete pItem;
}

// get ring item from std::ifstream.
//
void
v10factorytest::ring_7()
{
#pragma packed(push, 1)
    struct {
        v10::RingItemHeader s_header;
        uint16_t             s_body[100];
    } rawItem;
#pragma packed(pop)
    rawItem.s_header.s_type = PHYSICS_EVENT;
    rawItem.s_header.s_size = sizeof(rawItem);
    for (int i =0; i < 100; i++) {
        rawItem.s_body[i] = i;
    }

    auto fileInfo = makeTempFile();
    int fd = fileInfo.second;
    write(fd, &rawItem, sizeof(rawItem));
    std::ifstream in(fileInfo.first.c_str(), std::ios::binary | std::ios::in);
    if (!in) ASSERT(false);
    
    CRingItem* pItem(0);
    try {
        pItem = m_pFactory->getRingItem(in);
        ASSERT(pItem);
        EQ(rawItem.s_header.s_size, pItem->size());
        EQ(0, memcmp(&rawItem, pItem->getItemPointer(), sizeof(rawItem)));
    }
    catch (...)  {
        delete pItem;
        close(fd);
        in.close();
        unlink(fileInfo.first.c_str());
        throw;       
    }
    
    delete pItem;
    close(fd);
    in.close();
    unlink(fileInfo.first.c_str());
}
// write ring item to an ostream:
void
v10factorytest::ring_8()
{
#pragma packed(push, 1)
    struct {
        v10::RingItemHeader s_header;
        uint16_t             s_body[100];
    } rawItem;
#pragma packed(pop)
    rawItem.s_header.s_type = PHYSICS_EVENT;
    rawItem.s_header.s_size = sizeof(rawItem);
    for (int i =0; i < 100; i++) {
        rawItem.s_body[i] = i;
    }
    auto fileinfo = makeTempFile();
    CRingItem* pItem = m_pFactory->makeRingItem(reinterpret_cast<const ::RingItem*>(&rawItem));
    CRingItem* pReadItem(0);
    std::ofstream out(fileinfo.first.c_str(), std::ios::binary | std::ios::out);
    try {
        m_pFactory->putRingItem(pItem, out);
        out.flush();        // Put it in the file.
        pReadItem = m_pFactory->getRingItem(fileinfo.second);
        
        EQ(pItem->size(), pReadItem->size());
        EQ(0, memcmp(
            pItem->getItemPointer(), pReadItem->getItemPointer(), pItem->size()
        ));
        
    } catch(...) {
        out.close();
        delete pReadItem;
        delete pItem;
        close(fileinfo.second);
        unlink(fileinfo.first.c_str());
        throw;
    }
    
    
    
    close(fileinfo.second);
    unlink(fileinfo.first.c_str());
    delete pItem;
    delete pReadItem;
}
// Write ring item to an fd:

void
v10factorytest::ring_9()
{
#pragma packed(push, 1)
    struct {
        v10::RingItemHeader s_header;
        uint16_t             s_body[100];
    } rawItem;
#pragma packed(pop)
    rawItem.s_header.s_type = PHYSICS_EVENT;
    rawItem.s_header.s_size = sizeof(rawItem);
    for (int i =0; i < 100; i++) {
        rawItem.s_body[i] = i;
    }
    auto fileinfo = makeTempFile();
    CRingItem* pItem = m_pFactory->makeRingItem(reinterpret_cast<const ::RingItem*>(&rawItem));
    CRingItem* pReadItem(0);
    m_pFactory->putRingItem(pItem, fileinfo.second);
    lseek(fileinfo.second, 0, SEEK_SET);
    try {
        pReadItem = m_pFactory->getRingItem(fileinfo.second);
        EQ(pItem->size(), pReadItem->size());
        EQ(0, memcmp(
            pItem->getItemPointer(), pReadItem->getItemPointer(), pItem->size()
        ));
    }
    catch (...) {
        close(fileinfo.second);
        unlink(fileinfo.first.c_str());
        delete pItem;
        delete pReadItem;
        throw;
    }
    
    close(fileinfo.second);
    unlink(fileinfo.first.c_str());
    delete pItem;
    delete pReadItem;
}
// Put/get ring item from a ringbuffer.

void
v10factorytest::ring_10()
{
#pragma packed(push, 1)
    struct {
        v10::RingItemHeader s_header;
        uint16_t             s_body[100];
    } rawItem;
#pragma packed(pop)
    rawItem.s_header.s_type = PHYSICS_EVENT;
    rawItem.s_header.s_size = sizeof(rawItem);
    for (int i =0; i < 100; i++) {
        rawItem.s_body[i] = i;
    }
    CRingItem* pItem = m_pFactory->makeRingItem(reinterpret_cast<const ::RingItem*>(&rawItem));
    CRingItem* pReadItem(0);
    
    try {
        m_pFactory->putRingItem(pItem, *m_pProducer);
        pReadItem = m_pFactory->getRingItem(*m_pConsumer);
        EQ(pItem->size(), pReadItem->size());
        EQ(0, memcmp(
            pItem->getItemPointer(), pReadItem->getItemPointer(), pItem->size()
        ));
    }
    catch(...) {
        delete pItem;
        delete pReadItem;
        throw;
    }
    delete pItem;
    delete pReadItem;
}
// Can't create an abnormal end item:

void
v10factorytest::abend_1()
{
    auto pItem = m_pFactory->makeAbnormalEndItem();
    ASSERT(pItem == nullptr);
}
// from  a ring item is a bad cast:
void
v10factorytest::abend_2()
{
    auto pItem = m_pFactory->makeRingItem(PHYSICS_EVENT, 100);
    try {
        CPPUNIT_ASSERT_THROW(
            m_pFactory->makeAbnormalEndItem(*pItem),
            std::bad_cast
        );
    }
    catch (...) {
        delete pItem;
        throw;
    }
    delete pItem;
}
/// cannot create a data format item:
void
v10factorytest::fmt_1()
{
    auto pItem = m_pFactory->makeDataFormatItem();
    ASSERT(pItem == nullptr);
}
// create from ring item throws bad cast.

void
v10factorytest::fmt_2()
{
    auto pItem = m_pFactory->makeRingItem(PHYSICS_EVENT, 100);
    try {
        CPPUNIT_ASSERT_THROW(
            m_pFactory->makeDataFormatItem(*pItem),
            std::bad_cast
        );
    }
    catch (...) {
        delete pItem;
        throw;
    }
    delete pItem;
}

// Cannot create a glom parameters item:

void
v10factorytest::glompar_1()
{
    auto pItem = m_pFactory->makeGlomParameters(100, true, 1);
    ASSERT(pItem == nullptr);
}
// Creating from existing throws
void
v10factorytest::glompar_2()
{
    auto pItem = m_pFactory->makeRingItem(PHYSICS_EVENT, 100);
    try {
        CPPUNIT_ASSERT_THROW(
            m_pFactory->makeGlomParameters(*pItem),
            std::bad_cast
        );
    }
    catch (...) {
        delete pItem;
        throw;
    }
    delete pItem;
}
// Empty physics event item:

void
v10factorytest::phys_1()
{
    auto pItem = m_pFactory->makePhysicsEventItem(100);
    v10::CPhysicsEventItem* p = dynamic_cast<v10::CPhysicsEventItem*>(pItem);
    ASSERT(p != nullptr);
    
    EQ(PHYSICS_EVENT, pItem->type());
    EQ(sizeof(v10::RingItemHeader), size_t(pItem->size()));
}
// providing all those event source parameter still gives only
// a header:

void
v10factorytest::phys_2()
{
    auto pItem = m_pFactory->makePhysicsEventItem(
        uint64_t(0x12345678909), 1, 0, 100
    );
    v10::CPhysicsEventItem* p = dynamic_cast<v10::CPhysicsEventItem*>(pItem);
    ASSERT(p != nullptr);
    
    EQ(PHYSICS_EVENT, pItem->type());
    EQ(sizeof(v10::RingItemHeader), size_t(pItem->size()));
}
// Making from a physics item is ok:

void
v10factorytest::phys_3()
{
#pragma packed(push, 1)
    struct {
        v10::RingItemHeader s_header;
        uint16_t             s_body[100];
    } rawItem;
#pragma packed(pop)
    rawItem.s_header.s_type = PHYSICS_EVENT;
    rawItem.s_header.s_size = sizeof(rawItem);
    for (int i =0; i < 100; i++) {
        rawItem.s_body[i] = i;
    }
    auto pItem = m_pFactory->makeRingItem(reinterpret_cast<const ::RingItem*>(&rawItem));
    ::CPhysicsEventItem* pCopied(0);
    try {
        CPPUNIT_ASSERT_NO_THROW(
            pCopied = m_pFactory->makePhysicsEventItem(*pItem)
        );
        EQ(rawItem.s_header.s_size, pCopied->size());
        EQ(0, memcmp(&rawItem, pCopied->getItemPointer(), pItem->size()));
    }
    catch (...) {
        delete pItem;
        delete pCopied;
        throw;
    }
    delete pItem;
    delete pCopied;
}
// Copy construction from non physics item blows with bad_cast:

void
v10factorytest::phys_4()
{
#pragma packed(push, 1)
    struct {
        v10::RingItemHeader s_header;
        uint16_t             s_body[100];
    } rawItem;
#pragma packed(pop)
    rawItem.s_header.s_type = v10::BEGIN_RUN;
    rawItem.s_header.s_size = sizeof(rawItem);
    for (int i =0; i < 100; i++) {
        rawItem.s_body[i] = i;
    }
    auto pItem = m_pFactory->makeRingItem(reinterpret_cast<const ::RingItem*>(&rawItem));
    ::CRingItem* pCopied(0);
    try {
        CPPUNIT_ASSERT_THROW(
            pCopied = m_pFactory->makePhysicsEventItem(*pItem),
            std::bad_cast
        );
    }catch (...) {
        delete pItem;
        delete pCopied;
    }
    delete pItem;
    // If we got here, pCopied is still null.
    
}
// Can't make a fragment:
void
v10factorytest::frag_1()
{
    uint8_t data[100];
    for (int i =0; i < 100; i++) data[i] = i;
    std::unique_ptr<::CRingFragmentItem> ptr(m_pFactory->makeRingFragmentItem(
        1243, 1, sizeof(data), data
    ));
    EQ(uint64_t(1243), ptr->timestamp());
    EQ(uint32_t(1), ptr->source());
    EQ(uint32_t(0), ptr->barrierType());
    EQ(sizeof(data), ptr->payloadSize());
    
    const uint8_t* p = reinterpret_cast<const uint8_t*>(ptr->payloadPointer());
    EQ(0, memcmp(data, p, sizeof(data)));
    
    
}
// bad cast to make from another item:
void
v10factorytest::frag_2()
{
#pragma packed(push, 1)
    struct {
        v10::RingItemHeader s_header;
        uint16_t             s_body[100];
    } rawItem;
#pragma packed(pop)
    rawItem.s_header.s_type = v10::PHYSICS_EVENT;
    rawItem.s_header.s_size = sizeof(rawItem);
    for (int i =0; i < 100; i++) {
        rawItem.s_body[i] = i;
    }
    auto pItem = m_pFactory->makeRingItem(reinterpret_cast<const ::RingItem*>(&rawItem));
    CPPUNIT_ASSERT_THROW(
        m_pFactory->makeRingFragmentItem(*pItem),
        std::bad_cast
    );
    delete pItem;
}
void v10factorytest::frag_3()
{
    uint8_t data[100];
    for (int i =0; i < 100; i++) data[i] = i;
    std::unique_ptr<::CRingFragmentItem> pOriginal(m_pFactory->makeRingFragmentItem(
        1243, 1, sizeof(data), data
    ));

    std::unique_ptr<::CRingFragmentItem> pItem;
    CPPUNIT_ASSERT_NO_THROW(
        pItem.reset(m_pFactory->makeRingFragmentItem(*pOriginal))
    );
    EQ(0, memcmp(pOriginal->getItemPointer(), pItem->getItemPointer(), pOriginal->size()));
}
// Made from parameters:L
void v10factorytest::count_1()
{
    time_t now = time(nullptr);
    ::CRingPhysicsEventCountItem* pItem(0);
    try {
        pItem = m_pFactory->makePhysicsEventCountItem(
            12345, 10, now,
            2                 // 1 regardless.
        );
        EQ(v10::PHYSICS_EVENT_COUNT, pItem->type());
        
        v10::CRingPhysicsEventCountItem* p;
        CPPUNIT_ASSERT_NO_THROW(
            p = dynamic_cast<v10::CRingPhysicsEventCountItem*>(pItem)
        );
        EQ(uint32_t(10), p->getTimeOffset());
        EQ(float(10), p->computeElapsedTime());
        EQ(uint32_t(1), p->getTimeDivisor());
        EQ(now, p->getTimestamp());
        EQ(uint64_t(12345), p->getEventCount());
    }
    catch (...) {
        delete pItem;
        throw;
    }
    delete pItem;
}
// Good copy creation.

void
v10factorytest::count_2()
{
    time_t now = time(nullptr);
    v10::PhysicsEventCountItem item;
    item.s_header.s_type = v10::PHYSICS_EVENT_COUNT;
    item.s_header.s_size = sizeof(item);
    item.s_timeOffset = 10;
    item.s_timestamp = now;
    item.s_eventCount = 543210;
    
    auto p = m_pFactory->makeRingItem(reinterpret_cast<const ::RingItem*>(&item));
    ::CRingPhysicsEventCountItem* pItem(0);
    try {
        CPPUNIT_ASSERT_NO_THROW(
            pItem = m_pFactory->makePhysicsEventCountItem(*p)
        );
        v10::CRingPhysicsEventCountItem* p10 =
            dynamic_cast<v10::CRingPhysicsEventCountItem*>(pItem);
        EQ(PHYSICS_EVENT_COUNT, p10->type());
        EQ(uint32_t(10), pItem->getTimeOffset());
        EQ(float(10), pItem->computeElapsedTime());
        EQ(uint32_t(1), pItem->getTimeDivisor());
        EQ(now, pItem->getTimestamp());
        EQ(uint64_t(543210), pItem->getEventCount());
    }
    catch(...) {
        delete pItem;
        delete p;
        throw;
    }
    delete pItem;
    delete p;
}
// Wrong type throws

void
v10factorytest::count_3()
{
    
    time_t now = time(nullptr);
    v10::PhysicsEventCountItem item;
    item.s_header.s_type = v10::PHYSICS_EVENT;
    item.s_header.s_size = sizeof(item);
    item.s_timeOffset = 10;
    item.s_timestamp = now;
    item.s_eventCount = 543210;
    
    auto p = m_pFactory->makeRingItem(reinterpret_cast<const ::RingItem*>(&item));
    try {
        CPPUNIT_ASSERT_THROW(
            m_pFactory->makeRingFragmentItem(*p),
            std::bad_cast
        );
    }
    catch (...) {
        delete p;
        throw;
    }
    delete p;
}
// Wrong size!
void
v10factorytest::count_4()
{
    time_t now = time(nullptr);
    v10::PhysicsEventCountItem item;
    item.s_header.s_type = v10::PHYSICS_EVENT_COUNT;
    item.s_header.s_size = sizeof(item);
    item.s_timeOffset = 10;
    item.s_timestamp = now;
    item.s_eventCount = 543210;
    
    struct _bad {
        v10::PhysicsEventCountItem s_item;
        uint8_t                     s_extra;
    } bad;
    bad.s_item = item;
    bad.s_extra= 0;
    
    auto p = m_pFactory->makeRingItem(reinterpret_cast<const ::RingItem*>(&bad));
    try {
        CPPUNIT_ASSERT_THROW(
            m_pFactory->makeRingFragmentItem(*p),
            std::bad_cast
        );
    }
    catch (...) {
        delete p;
        throw;
    }
    delete p;
}
// Minimal scaler item.  No scaler values.

void
v10factorytest::scaler_1()
{
    time_t now = time(nullptr);
    ::CRingScalerItem* pBase(0);
    v10::CRingScalerItem* pActual(0);   
    try {
        pBase = m_pFactory->makeScalerItem(32);
        CPPUNIT_ASSERT_NO_THROW(
            pActual = dynamic_cast<v10::CRingScalerItem*>(pBase)
            
        );
        EQ(v10::INCREMENTAL_SCALERS, pActual->type());
        EQ(sizeof(v10::ScalerItem) + 31*sizeof(uint32_t), size_t(pActual->size()));
        
        EQ(uint32_t(0), pActual->getStartTime());
        EQ(float(0), pActual->computeStartTime());
        EQ(uint32_t(0), pActual ->getEndTime());
        EQ(float(0), pActual->computeEndTime());
        EQ(uint32_t(1), pActual->getTimeDivisor());
        ASSERT(pActual->getTimestamp() - now <= 1);
        
        
    }
    catch (...) {
        delete pBase;
        throw;
    }
    delete pBase;
}
// scaler with scalers.

void
v10factorytest::scaler_2()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i = 0; i < 32; i++) {scalers.push_back(i);}
    
    ::CRingScalerItem* pBase(0);
    try {
        pBase = m_pFactory->makeScalerItem(
            0, 10, now, scalers,
            true
        );
        v10::CRingScalerItem* pActual(0);
        
        pActual = dynamic_cast<v10::CRingScalerItem*>(pBase);
        ASSERT(pActual);
        EQ(v10::INCREMENTAL_SCALERS, pActual->type());
        EQ(sizeof(v10::ScalerItem) + 31*sizeof(uint32_t), size_t(pActual->size()));
        ASSERT(pActual->isIncremental());
        EQ(uint32_t(0), pActual->getStartTime());
        EQ(float(0), pActual->computeStartTime());
        EQ(uint32_t(10), pActual ->getEndTime());
        EQ(float(10), pActual->computeEndTime());
        EQ(uint32_t(1), pActual->getTimeDivisor());
        EQ(now, pActual->getTimestamp());
        auto itemsc = pActual->getScalers();
        EQ(uint32_t(scalers.size()), uint32_t(itemsc.size()));
        EQ(0, memcmp(scalers.data(), itemsc.data(), scalers.size()*sizeof(uint32_t)));
    }
    catch (...) {
        delete pBase;
        throw;
    }
    delete pBase;
}
// scaler with scalers -nonincremental gives a different beast:

void
v10factorytest::scaler_3()
{
    time_t now = time(nullptr);
    std::vector<uint32_t> scalers;
    for (int i = 0; i < 32; i++) {scalers.push_back(i);}
    
    ::CRingScalerItem* pBase(0);
    try {
        pBase = m_pFactory->makeScalerItem(
            0, 10, now, scalers,
            false
        );
        v10::CRingScalerItem* pActual(0);
        pActual = dynamic_cast<v10::CRingScalerItem*>(pBase);
        ASSERT(pActual);
        EQ(v10::TIMESTAMPED_NONINCR_SCALERS , pActual->type());
        EQ(sizeof(v10::NonIncrTimestampedScaler) + 31*sizeof(uint32_t), size_t(pActual->size()));
        ASSERT(!pActual->isIncremental());
        EQ(uint32_t(0), pActual->getStartTime());
        EQ(float(0), pActual->computeStartTime());
        EQ(uint32_t(10), pActual ->getEndTime());
        EQ(float(10), pActual->computeEndTime());
        EQ(uint32_t(1), pActual->getTimeDivisor());
        EQ(now, pActual->getTimestamp());
        auto itemsc = pActual->getScalers();
        EQ(uint32_t(scalers.size()), uint32_t(itemsc.size()));
        EQ(0, memcmp(scalers.data(), itemsc.data(), scalers.size()*sizeof(uint32_t)));
    }
    catch (...) {
        delete pBase;
        throw;
    }
    delete pBase;
}
// Make incremental scaler from raw ring item:

void
v10factorytest::scaler_4()
{
    time_t now = time(nullptr);
#pragma pack(push, 1)
    struct {
        v10::ScalerItem s_base;
        uint32_t   s_moreScalers[31];    // 32 total.
    } rawItem;
#pragma pack(pop)
    rawItem.s_base.s_header.s_size = sizeof(rawItem);
    rawItem.s_base.s_header.s_type = v10::INCREMENTAL_SCALERS;
    rawItem.s_base.s_intervalStartOffset  = 0;
    rawItem.s_base.s_intervalEndOffset    = 10;
    rawItem.s_base.s_timestamp            = now;
    rawItem.s_base.s_scalerCount          = 32;
    // This loop spills over into s_moreScalers - thanks to C++'s lack
    // of bound checking:
    for (int i =0; i < 32; i++) {
        rawItem.s_base.s_scalers[i] = i;
    }
    
    auto pItem =
        m_pFactory->makeRingItem(reinterpret_cast<const ::RingItem*>(&rawItem));
    ::CRingScalerItem* pBaseScaler(0);
    try {
        CPPUNIT_ASSERT_NO_THROW(
            pBaseScaler = m_pFactory->makeScalerItem(*pItem)
        );
        ::v10::CRingScalerItem* pActual =
            dynamic_cast<::v10::CRingScalerItem*>(pBaseScaler);
        ASSERT(pActual);
        EQ(v10::INCREMENTAL_SCALERS, pActual->type());
        EQ(sizeof(rawItem), size_t(pActual->size()));
        EQ(0, memcmp(&rawItem, pActual->getItemPointer(), sizeof(rawItem)));
    }
    catch (...) {
        delete pItem;
        delete pBaseScaler;
        throw;
    }
    delete pItem;
    delete pBaseScaler;
}
// Make nonincremental timstamped scaler from ring item:

void
v10factorytest::scaler_5()
{
    time_t now = time(nullptr);
#pragma pack(push, 1)
    struct {
        v10::NonIncrTimestampedScaler s_base;
        uint32_t                 s_moreScalers[31];
    } rawItem;
#pragma pack(pop)
    // Fill in the raw item:
    
    rawItem.s_base.s_header.s_size = sizeof(rawItem);
    rawItem.s_base.s_header.s_type = v10::TIMESTAMPED_NONINCR_SCALERS;
    rawItem.s_base.s_eventTimestamp = 0x1234567890;
    rawItem.s_base.s_intervalStartOffset = 0;
    rawItem.s_base.s_intervalEndOffset    = 10;
    rawItem.s_base.s_intervalDivisor      = 2;
    rawItem.s_base.s_clockTimestamp       = now;
    rawItem.s_base.s_scalerCount          = 32;
    for (int i =0; i < 32; i++) {
        rawItem.s_base.s_scalers[i] = i;
    }
    
    auto pItem =
        m_pFactory->makeRingItem(reinterpret_cast<const ::RingItem*>(&rawItem));
    ::CRingScalerItem* pBase(0);
    try {
        CPPUNIT_ASSERT_NO_THROW(
            pBase = m_pFactory->makeScalerItem(*pItem)
        );
        v10::CRingScalerItem* pActual =
            dynamic_cast<v10::CRingScalerItem*>(pBase);
        ASSERT(pActual);
        EQ(v10::TIMESTAMPED_NONINCR_SCALERS, pActual->type());
        EQ(sizeof(rawItem), size_t(pActual->size()));
        EQ(0, memcmp(&rawItem, pActual->getItemPointer(), sizeof(rawItem)));
    }
    catch (...) {
        delete pBase;
        delete pItem;
        throw;
    }
    delete pBase;
    delete pItem;
}
// Ok format but bad type:

void
v10factorytest::scaler_6()
{
#pragma pack(push, 1)
    struct {
        v10::ScalerItem s_base;
        uint32_t   s_moreScalers[31];    // 32 total.
    } rawItem;
#pragma pack(pop)
    rawItem.s_base.s_header.s_size = sizeof(rawItem);
    rawItem.s_base.s_header.s_type = v10::PHYSICS_EVENT_COUNT;
    
    auto pRingItem = m_pFactory->makeRingItem(
        reinterpret_cast<const ::RingItem*>(&rawItem)
    );
    try {
        CPPUNIT_ASSERT_THROW(
            m_pFactory->makeScalerItem(*pRingItem),
            std::bad_cast
        );
    }
    catch (...) {
        delete pRingItem;
        throw;
    }
    delete pRingItem;
}
// Wrong size but right type throws:

void
v10factorytest::scaler_7()
{
    ::RingItemHeader hdr;
    hdr.s_size = sizeof(hdr);
    hdr.s_type = v10::INCREMENTAL_SCALERS;
    
    auto pRingItem = m_pFactory->makeRingItem(
        reinterpret_cast<const ::RingItem*>(&hdr)
    );
    try {
        CPPUNIT_ASSERT_THROW(
            m_pFactory->makeScalerItem(*pRingItem),
            std::bad_cast
        );
    }
    catch (...) {
        delete pRingItem;
        throw;
    }
    delete pRingItem;
}
void v10factorytest::scaler_8()
{
    ::RingItemHeader hdr;
    hdr.s_size = sizeof(hdr);
    hdr.s_type = v10::TIMESTAMPED_NONINCR_SCALERS;
    
    auto pRingItem = m_pFactory->makeRingItem(
        reinterpret_cast<const ::RingItem*>(&hdr)
    );
    try {
        CPPUNIT_ASSERT_THROW(
            m_pFactory->makeScalerItem(*pRingItem),
            std::bad_cast
        );
    }
    catch (...) {
        delete pRingItem;
        throw;
    }
    delete pRingItem;
}
// minimal text item creation:

void v10factorytest::text_1()
{
    time_t now = time(nullptr);
    std::vector<std::string> empty;
    auto pBase = m_pFactory->makeTextItem(v10::PACKET_TYPES, empty);
    try {
        v10::CRingTextItem* pItem = dynamic_cast<v10::CRingTextItem*>(pBase);
        ASSERT(pItem);
        EQ(v10::PACKET_TYPES, pItem->type());
        EQ(sizeof(v10::TextItem), size_t(pItem->size()));
        EQ(uint32_t(0), pItem->getTimeOffset());
        ASSERT(pItem->getTimestamp() - now <= 1);
        auto strings = pItem->getStrings();
        EQ(size_t(0), strings.size());
    }
    catch (...) {
        delete pBase;
        throw;
    }
    delete pBase;
}
// Minmal text item creation with strings:

void v10factorytest::text_2()
{
    std::vector<std::string> inputStrings = {
        "Green strings", "Blue Strings", "Old strings", "New Strings",
        "Singing their strange familiar tune"
    };
    auto pBase = m_pFactory->makeTextItem(v10::PACKET_TYPES, inputStrings);
    v10::CRingTextItem* pItem = dynamic_cast<v10::CRingTextItem*>(pBase);
    try {
        auto strings = pItem->getStrings();
        EQ(inputStrings.size(), strings.size());
        for (int i =0; i < inputStrings.size(); i++) {
            EQ(inputStrings[i], strings[i]);
        }
    }
    catch (...) {
        delete pBase;
        throw;
    }
    delete pBase;
}
// makeTextItem with full argument list:

void
v10factorytest::text_3()
{
    std::vector<std::string> inputStrings = {
        "Green strings", "Blue Strings", "Old strings", "New Strings",
        "Singing their strange familiar tune"
    };
    time_t now = time(nullptr);
    
    auto pBase = m_pFactory->makeTextItem(
        v10::MONITORED_VARIABLES,  inputStrings,
        10, now,
        2                        // Becomes 1 in the end anyway.
    );
    v10::CRingTextItem* pItem = dynamic_cast<v10::CRingTextItem*>(pBase);
   
    try {
        ASSERT(pItem);
        auto s = pItem->getStrings();
        EQ(inputStrings.size(), s.size());
        ASSERT(inputStrings == s);
        EQ(uint32_t(10), pItem->getTimeOffset());
        EQ(now, pItem->getTimestamp());
        EQ(uint32_t(1), pItem->getTimeDivisor());
    }
    catch (...) {
        delete pBase;
        throw;
    }
    delete pBase;
}
// Illegal types in creationals.

void v10factorytest::text_4()
{
    std::vector<std::string> empty;
    CPPUNIT_ASSERT_THROW(
        m_pFactory->makeTextItem(PHYSICS_EVENT, empty), std::bad_cast
    );
}
void v10factorytest::text_5()
{
    std::vector<std::string> empty;
    CPPUNIT_ASSERT_THROW(
        m_pFactory->makeTextItem(PHYSICS_EVENT, empty, 10, time(nullptr)),
        std::bad_cast
    );
}

// good constrution from existing item:

void v10factorytest::text_6()
{
    std::vector<std::string> inputStrings = {
        "Green strings", "Blue Strings", "Old strings", "New Strings",
        "Singing their strange familiar tune"
    };
    
    v10::CRingTextItem src(
        v10::MONITORED_VARIABLES,
        inputStrings
    );
    auto pBase = m_pFactory->makeTextItem(src);
    v10::CRingTextItem* pCopy = dynamic_cast<v10::CRingTextItem*>(pBase);
    try {
        ASSERT(pCopy);
        EQ(src.size(), pCopy->size());
        EQ(0, memcmp(src.getItemPointer(), pCopy->getItemPointer(), src.size()));
    }
    catch(...) {
        delete pCopy;
        throw;
    }
    delete pCopy;
}
// Construction with item that has wrong type:

void v10factorytest::text_7()
{
    std::vector<std::string> inputStrings = {
        "Green strings", "Blue Strings", "Old strings", "New Strings",
        "Singing their strange familiar tune"
    };
    
    v10::CRingTextItem src(
        v10::MONITORED_VARIABLES,
        inputStrings
    );
    src.getItemPointer()->s_header.s_type=PHYSICS_EVENT;   // bad type.
    
    CPPUNIT_ASSERT_THROW(
        m_pFactory->makeTextItem(src), std::bad_cast
    );
}
// Construction of item that has wrong size:

void v10factorytest::text_8()
{
    std::vector<std::string> inputStrings = {
        "Green strings", "Blue Strings", "Old strings", "New Strings",
        "Singing their strange familiar tune"
    };
    
    v10::CRingTextItem src(
        v10::MONITORED_VARIABLES,
        inputStrings
    );
    src.getItemPointer()->s_header.s_size--;
    CPPUNIT_ASSERT_THROW(
        m_pFactory->makeTextItem(src), std::bad_cast
    );
}
// Unknonw item types are not supported:

void v10factorytest::unknown_1()
{
    auto p = m_pFactory->makeUnknownFragment(
        12345, 1, 0, 0, nullptr
    );
    ASSERT(p == nullptr);
}
void v10factorytest::unknown_2()
{
    auto p = m_pFactory->makeRingItem(1234, 100);
    try {
        CPPUNIT_ASSERT_THROW(
            m_pFactory->makeUnknownFragment(*p),
            std::bad_cast
        );
    }
    catch (...) {
        delete p;
        throw;
    }
    delete p;
}

// Good state change: BEGIN

void v10factorytest::state_1()
{
    time_t now = time(nullptr);
    ::CRingStateChangeItem* pBase(0);
    try {
        CPPUNIT_ASSERT_NO_THROW(
            pBase= m_pFactory->makeStateChangeItem(
             v10::BEGIN_RUN, 12, 0, now, "This is a title"
            )
        );
        v10::CRingStateChangeItem* pItem =
            reinterpret_cast<v10::CRingStateChangeItem*>(pBase);
        ASSERT(pItem);
        EQ(v10::BEGIN_RUN, pItem->type());
        EQ(sizeof(v10::StateChangeItem), size_t(pItem->size()));
        EQ(uint32_t(12), pItem->getRunNumber());
        EQ(uint32_t(0), pItem->getElapsedTime());
        EQ(now, pItem->getTimestamp());
        EQ(std::string("This is a title"), pItem->getTitle());
    }
    catch (...) {
        delete pBase;
        throw;
    }
    delete pBase;
}
// good state change END
void v10factorytest::state_2()
{
    time_t now = time(nullptr);
    ::CRingStateChangeItem* pBase(0);
    try {
        CPPUNIT_ASSERT_NO_THROW(
            pBase= m_pFactory->makeStateChangeItem(
             v10::END_RUN, 12, 100, now, "This is a title"
            )
        );
        v10::CRingStateChangeItem* pItem =
            reinterpret_cast<v10::CRingStateChangeItem*>(pBase);
        ASSERT(pItem);
        EQ(v10::END_RUN, pItem->type());
        EQ(sizeof(v10::StateChangeItem), size_t(pItem->size()));
        EQ(uint32_t(12), pItem->getRunNumber());
        EQ(uint32_t(100), pItem->getElapsedTime());
        EQ(now, pItem->getTimestamp());
        EQ(std::string("This is a title"), pItem->getTitle());
    }
    catch (...) {
        delete pBase;
        throw;
    }
    delete pBase;
}
// good state change PAUSE
void v10factorytest::state_3()
{
    time_t now = time(nullptr);
    ::CRingStateChangeItem* pBase(0);
    try {
        CPPUNIT_ASSERT_NO_THROW(
            pBase= m_pFactory->makeStateChangeItem(
             v10::PAUSE_RUN, 12, 100, now, "This is a title"
            )
        );
        v10::CRingStateChangeItem* pItem =
            reinterpret_cast<v10::CRingStateChangeItem*>(pBase);
        ASSERT(pItem);
        EQ(v10::PAUSE_RUN, pItem->type());
        EQ(sizeof(v10::StateChangeItem), size_t(pItem->size()));
        EQ(uint32_t(12), pItem->getRunNumber());
        EQ(uint32_t(100), pItem->getElapsedTime());
        EQ(now, pItem->getTimestamp());
        EQ(std::string("This is a title"), pItem->getTitle());
    }
    catch (...) {
        delete pBase;
        throw;
    }
    delete pBase;
}
// good state change RESUME

void v10factorytest::state_4()
{
    time_t now = time(nullptr);
    ::CRingStateChangeItem* pBase(0);
    try {
        CPPUNIT_ASSERT_NO_THROW(
            pBase= m_pFactory->makeStateChangeItem(
             v10::RESUME_RUN, 12, 100, now, "This is a title"
            )
        );
        v10::CRingStateChangeItem* pItem =
            reinterpret_cast<v10::CRingStateChangeItem*>(pBase);
        ASSERT(pItem);
        EQ(v10::RESUME_RUN, pItem->type());
        EQ(sizeof(v10::StateChangeItem), size_t(pItem->size()));
        EQ(uint32_t(12), pItem->getRunNumber());
        EQ(uint32_t(100), pItem->getElapsedTime());
        EQ(now, pItem->getTimestamp());
        EQ(std::string("This is a title"), pItem->getTitle());
    }
    catch (...) {
        delete pBase;
        throw;
    }
    delete pBase;
}

// bad state change type

void v10factorytest::state_5()
{
    time_t now = time(nullptr);
    ::CRingStateChangeItem* pBase(0);
    CPPUNIT_ASSERT_THROW(
        m_pFactory->makeStateChangeItem(
            v10::INCREMENTAL_SCALERS, 12, 100, now, "This is a title"
        ),
        std::bad_cast
    );
}

// state change from item (good).

void v10factorytest::state_6()
{
    v10::CRingStateChangeItem item(
        v10::BEGIN_RUN, 12, 10, time_t(nullptr),
        "this is the title"
    );
    ::CRingStateChangeItem* pItem(0);
    try {
        CPPUNIT_ASSERT_NO_THROW(
            pItem = m_pFactory->makeStateChangeItem(item)
        );
        EQ(item.size(), pItem->size());
        EQ(0, memcmp(item.getItemPointer(), pItem->getItemPointer(), item.size()));
    }
    catch (...) {
        delete pItem;
        throw;
    }
    delete pItem;
}

// state chagne from invalid item type

void v10factorytest::state_7()
{
    v10::CRingStateChangeItem item(
        v10::BEGIN_RUN, 12, 10, time_t(nullptr),
        "this is the title"
    );
    auto p = item.getItemPointer();
    p->s_header.s_type = PHYSICS_EVENT;     // bad type.
    
    CPPUNIT_ASSERT_THROW(
        m_pFactory->makeStateChangeItem(item), std::bad_cast
    );
}

// state change from invalid item size.

void v10factorytest::state_8()
{
    
    v10::CRingStateChangeItem item(
        v10::BEGIN_RUN, 12, 10, time_t(nullptr),
        "this is the title"
    );
    auto p = item.getItemPointer();
    p->s_header.s_size--;             // Bad size.
    
    CPPUNIT_ASSERT_THROW(
        m_pFactory->makeStateChangeItem(item), std::bad_cast
    );
}

