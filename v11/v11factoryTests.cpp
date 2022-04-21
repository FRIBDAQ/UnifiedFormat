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

/** @file:  v11factoryTests.cpp
 *  @brief: Test for the V11 ring item factory.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "RingItemFactory.h"

#include "DataFormat.h"

#include <CRingItem.h>
#include "CRingItem.h"  // v11
#include <CAbnormalEndItem.h>
#include <CDataFormatItem.h>
#include <CGlomParameters.h>
#include <CPhysicsEventItem.h>
#include <CRingFragmentItem.h>

#include <string.h>
#include <CRingBuffer.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <sstream>
#include <memory>


const char* ringbuffer="v11factoryring";

/*
 * Note in the implementation
 * the try/catch blocks use that assertion failures are signaled
 * via exception throws the catch delete rethrow pattern
 * ensures there will be no memory leaks in the presence of
 * test failures.
 */

class v11facttest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(v11facttest);
    CPPUNIT_TEST(base_1);
    CPPUNIT_TEST(base_2);
    CPPUNIT_TEST(base_3);
    CPPUNIT_TEST(base_4);
    CPPUNIT_TEST(get_1);
    CPPUNIT_TEST(get_2);
    CPPUNIT_TEST(get_3);
    
    CPPUNIT_TEST(put_1);
    CPPUNIT_TEST(put_2);
    CPPUNIT_TEST(put_3);
    
    CPPUNIT_TEST(abnormal_1);
    CPPUNIT_TEST(abnormal_2);
    CPPUNIT_TEST(abnormal_3);
    
    CPPUNIT_TEST(format_1);
    CPPUNIT_TEST(format_2);
    CPPUNIT_TEST(format_3);

    CPPUNIT_TEST(glom_1);
    CPPUNIT_TEST(glom_2);
    CPPUNIT_TEST(glom_3);
    
    CPPUNIT_TEST(phys_1);
    CPPUNIT_TEST(phys_2);
    CPPUNIT_TEST(phys_3);
    CPPUNIT_TEST(phys_4);
    
    CPPUNIT_TEST(frag_1);
    CPPUNIT_TEST(frag_2);
    CPPUNIT_TEST(frag_3);
    CPPUNIT_TEST(frag_4);
    CPPUNIT_TEST_SUITE_END();
    
private:
    v11::RingItemFactory* m_pFactory;
public:
    void setUp() {
        m_pFactory = new v11::RingItemFactory;
    }
    void tearDown() {
        delete m_pFactory;
    }
protected:
    void base_1();
    void base_2();
    void base_3();
    void base_4();
    
    void get_1();
    void get_2();
    void get_3();
    
    void put_1();
    void put_2();
    void put_3();
    
    void abnormal_1();
    void abnormal_2();
    void abnormal_3();
    
    void format_1();
    void format_2();
    void format_3();
    
    void glom_1();
    void glom_2();
    void glom_3();
    
    void phys_1();
    void phys_2();
    void phys_3();
    void phys_4();
    
    void frag_1();
    void frag_2();
    void frag_3();
    void frag_4();
};

CPPUNIT_TEST_SUITE_REGISTRATION(v11facttest);

// Make a ring item without a body header.
void v11facttest::base_1()
{
    ::CRingItem* pItem = m_pFactory->makeRingItem(v11::PHYSICS_EVENT, 100);
    try {
        const v11::RingItem* p =
            reinterpret_cast<const v11::RingItem*>(pItem->getItemPointer());
        EQ(sizeof(v11::RingItemHeader) + sizeof(uint32_t), size_t(p->s_header.s_size));
        EQ(v11::PHYSICS_EVENT, p->s_header.s_type);
        EQ(uint32_t(0), p->s_body.u_noBodyHeader.s_mbz);
        
        // Ensure that body cursor and size() are consistent with
        // contents of the ring item:
        
        EQ(pItem->size(), (p->s_header.s_size));
        const uint8_t* beg = reinterpret_cast<const uint8_t*>(p);
        const uint8_t* end = reinterpret_cast<const uint8_t*>(pItem->getBodyCursor());
        EQ(ptrdiff_t(pItem->size()), end - beg);
    }
    catch (...) {
        delete pItem;
        throw;
    }
    delete pItem;
}
// Make a ring item with a body header:
void v11facttest::base_2()
{
    ::CRingItem* pItem = m_pFactory->makeRingItem(
        v11::PHYSICS_EVENT, 0x1234567890, 2, 100, 1
    );
    try {
        const v11::RingItem* p =
            reinterpret_cast<const v11::RingItem*>(pItem->getItemPointer());
       EQ(
            sizeof(v11::RingItemHeader) + sizeof(v11::BodyHeader),
            size_t(p->s_header.s_size)
        );
        EQ(v11::PHYSICS_EVENT, p->s_header.s_type);
        EQ(
           sizeof(v11::BodyHeader),
           size_t(p->s_body.u_hasBodyHeader.s_bodyHeader.s_size)
        );
        EQ(
            uint64_t(0x1234567890),
            p->s_body.u_hasBodyHeader.s_bodyHeader.s_timestamp
        );
        EQ(
            uint32_t(2),
            p->s_body.u_hasBodyHeader.s_bodyHeader.s_sourceId
        );
        EQ(
            uint32_t(1),
            p->s_body.u_hasBodyHeader.s_bodyHeader.s_barrier
        );
        
        EQ(pItem->size(), (p->s_header.s_size));
        const uint8_t* beg = reinterpret_cast<const uint8_t*>(p);
        const uint8_t* end = reinterpret_cast<const uint8_t*>(pItem->getBodyCursor());
        EQ(ptrdiff_t(pItem->size()), end - beg);
    }
    catch (...) {
        delete pItem;
        throw;
    }
    delete pItem;
}
// Make duplicate from a base class item:
void v11facttest::base_3()
{
    ::CRingItem* pItem1 = m_pFactory->makeRingItem(
        v11::PHYSICS_EVENT, 100
    );
    uint16_t* p = reinterpret_cast<uint16_t*>(pItem1->getBodyCursor());
    for (int i =0; i < 10; i++) {
        *p++ = i;
    }
    pItem1->setBodyCursor(p);
    pItem1->updateSize();
    
    ::CRingItem* pItem2= m_pFactory->makeRingItem(*pItem1);
    
    try {
        const v11::RingItem* p1 =
            reinterpret_cast<v11::RingItem*>(pItem1->getItemPointer());
        const v11::RingItem* p2 =
            reinterpret_cast<v11::RingItem*>(pItem2->getItemPointer());
        EQ(p1->s_header.s_size, p2->s_header.s_size);
        EQ(0, memcmp(p1, p2, p1->s_header.s_size));
        
        EQ(pItem2->size(), (p2->s_header.s_size));
        const uint8_t* beg = reinterpret_cast<const uint8_t*>(p2);
        const uint8_t* end = reinterpret_cast<const uint8_t*>(pItem2->getBodyCursor());
        EQ(ptrdiff_t(pItem2->size()), end - beg);
    }
    catch (...) {
        delete pItem1;
        delete pItem2;
        
        throw;
    }
    delete pItem1;
    delete pItem2;
}
// DUplicate from raw

void v11facttest::base_4()
{
    // Make a raw ring item. might as well put a body header in it:
    
    uint8_t buffer[200];
    v11::pRingItem pRaw = reinterpret_cast<v11::pRingItem>(buffer);
    pRaw->s_header.s_type = v11::PHYSICS_EVENT;
    pRaw->s_header.s_size =
        sizeof(v11::RingItemHeader) + sizeof(v11::BodyHeader)
        +  sizeof(uint16_t)*10;   // 10 16 bit payload words.
    pRaw->s_body.u_hasBodyHeader.s_bodyHeader.s_timestamp = 0x1234567890;
    pRaw->s_body.u_hasBodyHeader.s_bodyHeader.s_sourceId  = 3;
    pRaw->s_body.u_hasBodyHeader.s_bodyHeader.s_barrier   = 3;
    pRaw->s_body.u_hasBodyHeader.s_bodyHeader.s_size = sizeof(v11::BodyHeader);
    
    uint16_t*  pContents =
        reinterpret_cast<uint16_t*>(pRaw->s_body.u_hasBodyHeader.s_body);
    for (int i =0; i < 10; i++) {
        *pContents++ = i;
    }
    ::CRingItem* pItem = m_pFactory->makeRingItem(
        reinterpret_cast<::pRingItem>(pRaw)
    );
    try {
        EQ(pItem->size(), (pRaw->s_header.s_size));
        const v11::pRingItem p =
            reinterpret_cast<const v11::pRingItem>(pItem->getItemPointer());
        EQ(0, memcmp(p, pRaw, pRaw->s_header.s_size));
        const uint8_t* beg = reinterpret_cast<const uint8_t*>(p);
        const uint8_t* end = reinterpret_cast<const uint8_t*>(pItem->getBodyCursor());
        EQ(ptrdiff_t(pItem->size()), end - beg);
    }
    catch(...) {
        delete pItem;
        throw;
    }
    
    delete pItem;
}
// Get an item from a ring buffer
// This test requires the DAQ be running.
void v11facttest::get_1()
{
    CRingBuffer* pProducer(0);
    CRingBuffer* pConsumer(0);
    ::CRingItem* pGotten(0);
    try {
        pProducer = CRingBuffer::createAndProduce(ringbuffer);
        pConsumer = new CRingBuffer(ringbuffer, CRingBuffer::consumer);
        v11::CRingItem  item(v11::PHYSICS_EVENT, 200);
        uint16_t* p = reinterpret_cast<uint16_t*>(item.getBodyCursor());
        for (int i =0; i < 20; i++) {
            *p++ = i;            
        }
        item.setBodyCursor(p);
        item.updateSize();
        pProducer->put(item.getItemPointer(), item.size());
        
        pGotten = m_pFactory->getRingItem(*pConsumer);
        ASSERT(!(pGotten == nullptr));
        EQ(item.size(), pGotten->size());
        EQ(0, memcmp(item.getItemPointer(), pGotten->getItemPointer(), item.size()));
        
        // Check pGotten's cursor.
        
        const uint8_t* pBeg = reinterpret_cast<const uint8_t*>(pGotten->getItemPointer());
        const uint8_t* pEnd = reinterpret_cast<const uint8_t*>(pGotten->getBodyCursor());
        EQ(ptrdiff_t(pGotten->size()), pEnd - pBeg);
    }
    catch (...) {
        delete pGotten;
        delete pProducer;
        delete pConsumer;
        CRingBuffer::remove(ringbuffer);
        throw;
    }
    delete pGotten;
    delete pProducer;
    delete pConsumer;
    CRingBuffer::remove(ringbuffer);
}
/* get from file descriptor. */
void v11facttest::get_2()
{
    int fd = memfd_create("factory-test", 0);
    ::CRingItem* pGotten(0);
    
    try {
        v11::CRingItem  item(v11::PHYSICS_EVENT, 200);
        uint16_t* p = reinterpret_cast<uint16_t*>(item.getBodyCursor());
        for (int i =0; i < 20; i++) {
            *p++ = i;            
        }
        item.setBodyCursor(p);
        item.updateSize();
        write(fd, item.getItemPointer(), item.size());
        lseek(fd, 0, SEEK_SET);   // rewind the memory file.
        
        pGotten = m_pFactory->getRingItem(fd);
        ASSERT(pGotten != nullptr);
        EQ(item.size(), pGotten->size());
        EQ(0, memcmp(item.getItemPointer(), pGotten->getItemPointer(), item.size()));
        
        // Check pGotten's cursor.
        
        const uint8_t* pBeg = reinterpret_cast<const uint8_t*>(pGotten->getItemPointer());
        const uint8_t* pEnd = reinterpret_cast<const uint8_t*>(pGotten->getBodyCursor());
        EQ(ptrdiff_t(pGotten->size()), pEnd - pBeg);
    }
    catch (...) {
        close(fd);
        delete pGotten;
        throw;
    }
    close(fd);
    delete pGotten;
    
}
// get item from std::istream:

void v11facttest::get_3()
{
    std::stringstream s;
    ::CRingItem* pGotten(0);
    try {
        v11::CRingItem  item(v11::PHYSICS_EVENT, 200);
        uint16_t* p = reinterpret_cast<uint16_t*>(item.getBodyCursor());
        for (int i =0; i < 20; i++) {
            *p++ = i;            
        }
        item.setBodyCursor(p);
        item.updateSize();
        s.write(
            reinterpret_cast<const char*>(item.getItemPointer()),
            item.size()
        );
        s.seekg(0);    // rewind.
        
        pGotten = m_pFactory->getRingItem(s);
        ASSERT(pGotten != nullptr);
        EQ(item.size(), pGotten->size());
        EQ(0, memcmp(item.getItemPointer(), pGotten->getItemPointer(), item.size()));
        
        // Check pGotten's cursor.
        
        const uint8_t* pBeg = reinterpret_cast<const uint8_t*>(pGotten->getItemPointer());
        const uint8_t* pEnd = reinterpret_cast<const uint8_t*>(pGotten->getBodyCursor());
        EQ(ptrdiff_t(pGotten->size()), pEnd - pBeg);
    }
    catch (...) {
        delete pGotten;
        throw;
    }
    delete pGotten;
}
// put to std::ostream:

void v11facttest::put_1()
{
    std::stringstream s;
    ::CRingItem* pGotten(0);
    try {
        v11::CRingItem  item(v11::PHYSICS_EVENT, 200);
        uint16_t* p = reinterpret_cast<uint16_t*>(item.getBodyCursor());
        for (int i =0; i < 20; i++) {
            *p++ = i;            
        }
        item.setBodyCursor(p);
        item.updateSize();
        m_pFactory->putRingItem(&item, s);
        s.seekg(0);
        
        pGotten = m_pFactory->getRingItem(s);
        ASSERT(pGotten != nullptr);
        EQ(item.size(), pGotten->size());
        EQ(0, memcmp(item.getItemPointer(), pGotten->getItemPointer(), item.size()));
        
        // Check pGotten's cursor.
        
        const uint8_t* pBeg = reinterpret_cast<const uint8_t*>(pGotten->getItemPointer());
        const uint8_t* pEnd = reinterpret_cast<const uint8_t*>(pGotten->getBodyCursor());
        EQ(ptrdiff_t(pGotten->size()), pEnd - pBeg);
    }
    catch(...) {
        delete pGotten;
        throw;
    }
    delete pGotten;
}
void v11facttest::put_2()
{
    int fd = memfd_create("factory-test", 0);
    ::CRingItem* pGotten(0);
    try {
        v11::CRingItem  item(v11::PHYSICS_EVENT, 200);
        uint16_t* p = reinterpret_cast<uint16_t*>(item.getBodyCursor());
        for (int i =0; i < 20; i++) {
            *p++ = i;            
        }
        item.setBodyCursor(p);
        item.updateSize();
        
        m_pFactory->putRingItem(&item, fd);
        lseek(fd, 0, SEEK_SET);
        pGotten = m_pFactory->getRingItem(fd);
        ASSERT(pGotten != nullptr);
        EQ(item.size(), pGotten->size());
        EQ(0, memcmp(item.getItemPointer(), pGotten->getItemPointer(), item.size()));
        
        // Check pGotten's cursor.
        
        const uint8_t* pBeg = reinterpret_cast<const uint8_t*>(pGotten->getItemPointer());
        const uint8_t* pEnd = reinterpret_cast<const uint8_t*>(pGotten->getBodyCursor());
        EQ(ptrdiff_t(pGotten->size()), pEnd - pBeg);
    }
    catch (...) {
        close(fd);
        delete pGotten;
        throw;
    }
    close(fd);
    delete pGotten;
}
// put into ringbuffer.

void v11facttest::put_3()
{
    CRingBuffer* pProducer(0);
    CRingBuffer* pConsumer(0);
    ::CRingItem* pGotten(0);
    try {
        pProducer = CRingBuffer::createAndProduce(ringbuffer);
        pConsumer = new CRingBuffer(ringbuffer, CRingBuffer::consumer);
        
        v11::CRingItem  item(v11::PHYSICS_EVENT, 200);
        uint16_t* p = reinterpret_cast<uint16_t*>(item.getBodyCursor());
        for (int i =0; i < 20; i++) {
            *p++ = i;            
        }
        item.setBodyCursor(p);
        item.updateSize();
        
        m_pFactory->putRingItem(&item, *pProducer);
        
        pGotten = m_pFactory->getRingItem(*pConsumer);
        ASSERT(pGotten != nullptr);
        EQ(item.size(), pGotten->size());
        EQ(0, memcmp(item.getItemPointer(), pGotten->getItemPointer(), item.size()));
        
        // Check pGotten's cursor.
        
        const uint8_t* pBeg = reinterpret_cast<const uint8_t*>(pGotten->getItemPointer());
        const uint8_t* pEnd = reinterpret_cast<const uint8_t*>(pGotten->getBodyCursor());
        EQ(ptrdiff_t(pGotten->size()), pEnd - pBeg);
    }
    catch (...) {
        delete pGotten;
        delete pProducer;
        delete pConsumer;
        CRingBuffer::remove(ringbuffer);
        throw;
    }
    delete pGotten;
    delete pProducer;
    delete pConsumer;
    CRingBuffer::remove(ringbuffer);
    
}
// Create from nothing.
void v11facttest::abnormal_1()
{
    ::CAbnormalEndItem* pItem = m_pFactory->makeAbnormalEndItem();
    try
    {
        EQ(sizeof(v11::AbnormalEndItem), size_t(pItem->size()));
        EQ(v11::ABNORMAL_ENDRUN, pItem->type());
        const v11::AbnormalEndItem* p =
            reinterpret_cast<const v11::AbnormalEndItem*>(pItem->getItemPointer());
        EQ(sizeof(v11::AbnormalEndItem), size_t(p->s_header.s_size));
        EQ(v11::ABNORMAL_ENDRUN, p->s_header.s_type);
        EQ(uint32_t(0), p->s_mbz);
    }
    catch (...) {
        delete pItem;
        throw;
    }
    delete pItem;
    
}
// Create from appropriate ring item (copy construction).
void v11facttest::abnormal_2()
{
    ::CAbnormalEndItem* pSrc = m_pFactory->makeAbnormalEndItem();
    ::CAbnormalEndItem* pCopy(0);
    try {
        CPPUNIT_ASSERT_NO_THROW(pCopy = m_pFactory->makeAbnormalEndItem(*pSrc));
        EQ(sizeof(v11::AbnormalEndItem), size_t(pCopy->size()));
        EQ(v11::ABNORMAL_ENDRUN, pCopy->type());
        const void* p1  = pSrc->getItemPointer();
        const void* p2  = pCopy->getItemPointer();
        EQ(0, memcmp(p1,p2, pSrc->size()));
    }
    catch (...) {
        delete pSrc;
        delete pCopy;
        throw;
    }
    delete pSrc;
    delete pCopy;
}

// Create from inappropriate item throws.
void v11facttest::abnormal_3()
{
    v11::CRingItem item(v11::PHYSICS_EVENT, 40);
    CPPUNIT_ASSERT_THROW(
        m_pFactory->makeAbnormalEndItem(item),
        std::bad_cast
    );
}
// Create format item from nothing.

void v11facttest::format_1()
{
    ::CDataFormatItem* pItem = m_pFactory->makeDataFormatItem();
    try {
        EQ(uint16_t(11), pItem->getMajor());
        EQ(uint16_t(0), pItem->getMinor());
        EQ(v11::RING_FORMAT, pItem->type());
        EQ(sizeof(v11::DataFormat), size_t(pItem->size()));
        
        const v11::DataFormat* p =
            reinterpret_cast<v11::DataFormat*>(pItem->getItemPointer());
        EQ(v11::RING_FORMAT, p->s_header.s_type);
        EQ(
           sizeof(RingItemHeader) + sizeof(uint32_t) + 2*sizeof(uint16_t),
           size_t(p->s_header.s_size)
        );
        EQ(uint32_t(0), p->s_mbz);
        EQ(uint16_t(11), p->s_majorVersion);
        EQ(uint16_t(0), p->s_minorVersion);
        
    }
    catch(...) {
        delete pItem;
        throw;
    }
    delete pItem;
}
// create format item from an appropriate other item.
void v11facttest::format_2()
{
    ::CDataFormatItem* pItem = m_pFactory->makeDataFormatItem();
    ::CDataFormatItem* pCopy(0);
    try {
        CPPUNIT_ASSERT_NO_THROW(pCopy = m_pFactory->makeDataFormatItem(*pItem));
        EQ(uint16_t(11), pItem->getMajor());
        EQ(uint16_t(0), pItem->getMinor());
        EQ(v11::RING_FORMAT, pItem->type());
        EQ(sizeof(v11::DataFormat), size_t(pItem->size()));
        
        const v11::DataFormat* p =
            reinterpret_cast<v11::DataFormat*>(pItem->getItemPointer());
        EQ(v11::RING_FORMAT, p->s_header.s_type);
        EQ(
           sizeof(RingItemHeader) + sizeof(uint32_t) + 2*sizeof(uint16_t),
           size_t(p->s_header.s_size)
        );
        EQ(uint32_t(0), p->s_mbz);
        EQ(uint16_t(11), p->s_majorVersion);
        EQ(uint16_t(0), p->s_minorVersion);
    }
    catch(...) {
        delete pItem;
        delete pCopy;
        throw;
    }
    delete pItem;
    delete pCopy;

    
}
// create format item from inappropriate item.
void v11facttest::format_3()
{
    v11::CRingItem item(v11::PHYSICS_EVENT, 100);
    CPPUNIT_ASSERT_THROW(
        m_pFactory->makeDataFormatItem(item),
        std::bad_cast
    );
}
//make glom from parameters.

void v11facttest::glom_1()
{
    auto pItem = m_pFactory->makeGlomParameters(
        100, true, v11::GLOM_TIMESTAMP_LAST
    );
    try {
        EQ(v11::EVB_GLOM_INFO, pItem->type());
        EQ(sizeof(v11::GlomParameters), size_t(pItem->size()));
        EQ(uint64_t(100), pItem->coincidenceTicks());
        ASSERT(pItem->isBuilding());
        EQ(::CGlomParameters::last, pItem->timestampPolicy());
        
        const v11::GlomParameters* p =
            reinterpret_cast<v11::GlomParameters*>(pItem->getItemPointer());
        EQ(v11::EVB_GLOM_INFO, p->s_header.s_type);
        EQ(sizeof(v11::GlomParameters), size_t(p->s_header.s_size));
        EQ(uint32_t(0), p->s_mbz);
        EQ(uint64_t(100), p->s_coincidenceTicks);
        ASSERT(p->s_isBuilding);
        EQ(uint16_t(v11::GLOM_TIMESTAMP_LAST), p->s_timestampPolicy); 
    }
    catch (...) {
        delete pItem;
        throw;
    }
    delete pItem;
}
// Make glom via copy construction

void v11facttest::glom_2()
{
    auto pItem = m_pFactory->makeGlomParameters(
        100, true, v11::GLOM_TIMESTAMP_LAST
    );
    ::CGlomParameters* pCopy(0);
    try {
        CPPUNIT_ASSERT_NO_THROW(pCopy = m_pFactory->makeGlomParameters(*pItem));
        EQ(v11::EVB_GLOM_INFO, pCopy->type());
        EQ(sizeof(v11::GlomParameters), size_t(pCopy->size()));
        EQ(uint64_t(100), pCopy->coincidenceTicks());
        ASSERT(pCopy->isBuilding());
        EQ(::CGlomParameters::last, pCopy->timestampPolicy());
        
        const v11::GlomParameters* p =
            reinterpret_cast<v11::GlomParameters*>(pCopy->getItemPointer());
        EQ(v11::EVB_GLOM_INFO, p->s_header.s_type);
        EQ(sizeof(v11::GlomParameters), size_t(p->s_header.s_size));
        EQ(uint32_t(0), p->s_mbz);
        EQ(uint64_t(100), p->s_coincidenceTicks);
        ASSERT( p->s_isBuilding);
        EQ(uint16_t(v11::GLOM_TIMESTAMP_LAST), p->s_timestampPolicy); 
    }
    catch (...) {
        delete pItem;
        delete pCopy;
        throw;
    }
    delete pItem;
    delete pCopy;
}
// Make glom via inappropriate source item -throws.
void v11facttest::glom_3()
{
    v11::CRingItem item(v11::PHYSICS_EVENT, 100);
    CPPUNIT_ASSERT_THROW(
        m_pFactory->makeGlomParameters(item),
        std::bad_cast
    );
}
// physics item with no body header.

void v11facttest::phys_1()
{
    // Create an item with stuff but no body header.
    auto pItem = m_pFactory->makePhysicsEventItem(200);
    uint16_t* pCursor = reinterpret_cast<uint16_t*>(pItem->getBodyCursor());
    for (int  i =0; i < 20; i++) {
        *pCursor++ = i;
    }
    pItem->setBodyCursor(pCursor);
    pItem->updateSize();
    
    // Check out the item:
    
    try {
        EQ(v11::PHYSICS_EVENT, pItem->type());
        EQ(
            sizeof(v11::RingItemHeader) + sizeof(uint32_t)
            + sizeof(uint16_t)*20,
            size_t(pItem->size())
        );
        // nitty gritty:
        
        const v11::PhysicsEventItem* p =
            reinterpret_cast<const v11::PhysicsEventItem*>(pItem->getItemPointer());
        EQ(v11::PHYSICS_EVENT, p->s_header.s_type);
        EQ(
            sizeof(v11::RingItemHeader) + sizeof(uint32_t)
            + sizeof(uint16_t)*20,
            size_t(p->s_header.s_size)
        );
        EQ(uint32_t(0), p->s_body.u_noBodyHeader.s_mbz);
        const uint16_t* payload = reinterpret_cast<const uint16_t*>(pItem->getBodyPointer());
        for (uint16_t i = 0; i < 20; i++) {
            EQ(i, *payload);
            payload++;
        }
    }
    catch (...) {
        delete pItem;
        throw;
    }
    delete pItem;
}
// Physics item with body header

void v11facttest::phys_2()
{
    // Make the item:
    
    auto pItem = m_pFactory->makePhysicsEventItem(
            0x1234567890, 2, 1, 200
    );
    uint16_t* pCursor = reinterpret_cast<uint16_t*>(pItem->getBodyCursor());
    for (int  i =0; i < 20; i++) {
        *pCursor++ = i;
    }
    pItem->setBodyCursor(pCursor);
    pItem->updateSize();
    
    // test the item:
    
    try {
        EQ(v11::PHYSICS_EVENT, pItem->type());
        EQ(
            sizeof(v11::RingItemHeader) + sizeof(v11::BodyHeader)
            + sizeof(uint16_t)*20,
            size_t(pItem->size())
        );
        // nitty gritty:
        
        const v11::PhysicsEventItem* p =
            reinterpret_cast<const v11::PhysicsEventItem*>(pItem->getItemPointer());
        EQ(v11::PHYSICS_EVENT, p->s_header.s_type);
        EQ(
            sizeof(v11::RingItemHeader) + sizeof(v11::BodyHeader)
            + sizeof(uint16_t)*20,
            size_t(p->s_header.s_size)
        );
        EQ(
           uint32_t(sizeof(v11::BodyHeader)),
           p->s_body.u_hasBodyHeader.s_bodyHeader.s_size
        );
        EQ(uint64_t(0x1234567890), p->s_body.u_hasBodyHeader.s_bodyHeader.s_timestamp);
        EQ(uint32_t(2), p->s_body.u_hasBodyHeader.s_bodyHeader.s_sourceId);
        EQ(uint32_t(1), p->s_body.u_hasBodyHeader.s_bodyHeader.s_barrier);
        const uint16_t* payload = reinterpret_cast<const uint16_t*>(pItem->getBodyPointer());
        for (uint16_t i = 0; i < 20; i++) {
            EQ(i, *payload);
            payload++;
        }
    } catch (...) {
        delete pItem;
        throw;
    }
    delete pItem;
    
}
// physics item from another physics item.
void v11facttest::phys_3()
{
    std::unique_ptr<::CPhysicsEventItem> pSource(
       m_pFactory->makePhysicsEventItem(
            0x1234567890, 2, 1, 200
    ));
    uint16_t* pCursor = reinterpret_cast<uint16_t*>(pSource->getBodyCursor());
    for (int  i =0; i < 20; i++) {
        *pCursor++ = i;
    }
    pSource->setBodyCursor(pCursor);
    pSource->updateSize();
    
    std::unique_ptr<::CPhysicsEventItem> pItem;
    CPPUNIT_ASSERT_NO_THROW(
        pItem.reset(m_pFactory->makePhysicsEventItem(*pSource))
    );
    EQ(v11::PHYSICS_EVENT, pItem->type());
    EQ(
        sizeof(v11::RingItemHeader) + sizeof(v11::BodyHeader)
        + sizeof(uint16_t)*20,
        size_t(pItem->size())
    );
    // nitty gritty:
    
    const v11::PhysicsEventItem* p =
        reinterpret_cast<const v11::PhysicsEventItem*>(pItem->getItemPointer());
    EQ(v11::PHYSICS_EVENT, p->s_header.s_type);
    EQ(
        sizeof(v11::RingItemHeader) + sizeof(v11::BodyHeader)
        + sizeof(uint16_t)*20,
        size_t(p->s_header.s_size)
    );
    EQ(
       uint32_t(sizeof(v11::BodyHeader)),
       p->s_body.u_hasBodyHeader.s_bodyHeader.s_size
    );
    EQ(uint64_t(0x1234567890), p->s_body.u_hasBodyHeader.s_bodyHeader.s_timestamp);
    EQ(uint32_t(2), p->s_body.u_hasBodyHeader.s_bodyHeader.s_sourceId);
    EQ(uint32_t(1), p->s_body.u_hasBodyHeader.s_bodyHeader.s_barrier);
    const uint16_t* payload = reinterpret_cast<const uint16_t*>(pItem->getBodyPointer());
    for (uint16_t i = 0; i < 20; i++) {
        EQ(i, *payload);
        payload++;
    }
    
}
// physics item from non physics item throw bad_cast
void v11facttest::phys_4()
{
    std::unique_ptr<::CDataFormatItem> pItem(m_pFactory->makeDataFormatItem());
    CPPUNIT_ASSERT_THROW(
        m_pFactory->makePhysicsEventItem(*pItem),
        std::bad_cast
    );
}

// Construct parameterized fragment item with empty payload
void v11facttest::frag_1()
{
    std::unique_ptr<::CRingFragmentItem> pItem(
        m_pFactory->makeRingFragmentItem(
            0x1234567890, 2, 0, nullptr, 1
        )
    );
    EQ(
        sizeof(v11::RingItemHeader) + sizeof(v11::BodyHeader),
        size_t(pItem->size())
    );
    EQ(v11::EVB_FRAGMENT, pItem->type());
    EQ(uint64_t(0x1234567890), pItem->timestamp());
    EQ(uint32_t(2), pItem->source());
    EQ(uint32_t(1), pItem->barrierType());
    EQ(size_t(0), pItem->payloadSize());
    
    
}
// construct parameterized fragment with non-empty payload
void v11facttest::frag_2()
{
    uint16_t payload[100];
    for (int i =0;i < 100; i++) {
        payload[i] = i;
    }
    std::unique_ptr<::CRingFragmentItem> pItem(
        m_pFactory->makeRingFragmentItem(
            0x1234567890, 2, sizeof(payload), payload, 1
        )
    );
    EQ(
        sizeof(v11::RingItemHeader) + sizeof(v11::BodyHeader)
        + sizeof(payload),
        size_t(pItem->size())
    );
    EQ(v11::EVB_FRAGMENT, pItem->type());
    EQ(uint64_t(0x1234567890), pItem->timestamp());
    EQ(uint32_t(2), pItem->source());
    EQ(uint32_t(1), pItem->barrierType());
    EQ(sizeof(payload), pItem->payloadSize());
    const void* p = pItem->payloadPointer();
    EQ(0, memcmp(payload, p, sizeof(payload)));
}
// construct copy of fragment
void v11facttest::frag_3()
{
    uint16_t payload[100];
    for (int i =0;i < 100; i++) {
        payload[i] = i;
    }
    std::unique_ptr<::CRingFragmentItem> pOriginal(
        m_pFactory->makeRingFragmentItem(
            0x1234567890, 2, sizeof(payload), payload, 1
        )
    );
    std::unique_ptr<::CRingFragmentItem> pItem;
    CPPUNIT_ASSERT_NO_THROW(
        pItem.reset(
            m_pFactory->makeRingFragmentItem(*pOriginal)
        )
    );
    EQ(
        sizeof(v11::RingItemHeader) + sizeof(v11::BodyHeader)
        + sizeof(payload),
        size_t(pItem->size())
    );
    EQ(v11::EVB_FRAGMENT, pItem->type());
    EQ(uint64_t(0x1234567890), pItem->timestamp());
    EQ(uint32_t(2), pItem->source());
    EQ(uint32_t(1), pItem->barrierType());
    EQ(sizeof(payload), pItem->payloadSize());
    const void* p = pItem->payloadPointer();
    EQ(0, memcmp(payload, p, sizeof(payload)));
}
// copy construction with illegal source
void v11facttest::frag_4()
{
    std::unique_ptr<::CAbnormalEndItem> pBad(
        m_pFactory->makeAbnormalEndItem()
    );
    CPPUNIT_ASSERT_THROW(
        m_pFactory->makeRingFragmentItem(*pBad),
        std::bad_cast
    );
}