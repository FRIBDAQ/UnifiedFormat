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

/** @file:  RingItemFactory.cpp (v12)
 *  @brief: Implement v12::RingItemFactory.
 */
#include "RingItemFactory.h"

#include "CRingItem.h"
#include "CAbnormalEndItem.h"
#include "CGlomParameters.h"
#include "CDataFormatItem.h"
#include "CPhysicsEventItem.h"
#include "CRingFragmentItem.h"
#include "CRingPhysicsEventCountItem.h"
#include "CRingScalerItem.h"
#include "CRingTextItem.h"
#include "CUnknownFragment.h"
#include "CRingStateChangeItem.h"
#include <DataFormat.h>
#include "DataFormat.h"

#include <string.h>
#include <CRingBuffer.h>
#include <io.h>
#include <stdexcept>

namespace v12 {

/**
 * makeRingItem.
 *    Create a v12:: ring item:
 *  @param type  - type of item.
 *  @param maxBody - maximum body size.
 *  @return ::CRingItem* - pointer to the newly created ring item.
 */
::CRingItem*
RingItemFactory::makeRingItem(uint16_t type, size_t maxbody)
{
    return new v12::CRingItem(type, maxbody);
}
/**
 * makeRingItem
 *    Make  a ring item with a body header.
 * @param type - item type.
 * @param timestamp - body header timestamp
 * @param sourceId - body header source id.
 * @param maxbody  - bound on size (capacity).
 * @param barrierType - body header barrier type.
 *  @return ::CRingItem* - pointer to the newly created ring item.
 */
::CRingItem*
RingItemFactory::makeRingItem(
    uint16_t type, uint64_t timestamp, uint32_t sourceId,
    size_t maxBody, uint32_t barrierType
)
{
    return new v12::CRingItem(type, timestamp, sourceId, maxBody, barrierType);    
}
/**
 * makeRingItem
 *    Like a copy constructor.
 * @param rhs - ring item we're copying.
 *  @return ::CRingItem* - pointer to the newly created ring item.
 */
::CRingItem*
RingItemFactory::makeRingItem(
    const ::CRingItem& rhs
)
{
    v12::CRingItem* pResult = new v12::CRingItem(rhs.type(), rhs.size());
    memcpy(pResult->getItemPointer(), rhs.getItemPointer(), rhs.size());
    return pResult;
}
/**
 * makeRingItem
 *   In this case from a raw ring item.
 * @param rhs - ring item we're copying.
 * @return ::CRingItem*
 */
::CRingItem*
RingItemFactory::makeRingItem(
    const ::RingItem* rhs
)
{
    v12::CRingItem* pResult = new v12::CRingItem(
        rhs->s_header.s_type, rhs->s_header.s_size
    );
    memcpy(pResult->getItemPointer(), rhs, rhs->s_header.s_size);
    return pResult;
}
/**
 * getRingItem
 *    Get a ring item from a ring buffer.
 *  @param ringbuf -references a ring buffer from which the ring item will be gotten.
 *  @return ::CRingItem* - pointer to the dynamically created gotten item.
 */
::CRingItem*
RingItemFactory::getRingItem(CRingBuffer& ringbuf)
{
    v12::RingItemHeader hdr;
    ringbuf.get(&hdr, sizeof(hdr), sizeof(hdr));
    v12::CRingItem* pResult = new CRingItem(hdr.s_type, hdr.s_size);
    
    // Read the remainder of the item:
    
    uint32_t remaining = hdr.s_size = sizeof(hdr);
    uint8_t* p = reinterpret_cast<uint8_t*>(pResult->getItemPointer());
    p += sizeof(hdr);
    
    if (remaining) {
        ringbuf.get(p, remaining, remaining);
        p += remaining;
    }
    pResult->setBodyCursor(p);
    pResult->updateSize();
    return pResult;
}
/**
 * getRingItem
 *    Read a ring item from file open on a file descriptor.,
 *  @param fd - file descriptor opened on the file.
 *  @return ::CRingItem* - pointer to the ring item that was dynamically made.
 *  @retval nullptr - eof.
 */
::CRingItem*
RingItemFactory::getRingItem(int fd)
{
    v12::RingItemHeader hdr;
    if (io::readData(fd, &hdr, sizeof(hdr)) < sizeof(hdr)) {
        return nullptr;
    }
    
    v12::CRingItem* pResult = new v12::CRingItem(hdr.s_type, hdr.s_size);
    
    uint8_t* p = reinterpret_cast<uint8_t*>(pResult->getItemPointer());
    p += sizeof(hdr);
    uint32_t remaining = hdr.s_size - sizeof(hdr);
    
    if (remaining) {
        if (io::readData(fd, p, remaining) < remaining) {
            delete pResult;
            return nullptr;
        }
    }
    
    p += remaining;
    pResult->setBodyCursor(p);
    pResult->updateSize();
    return pResult;
}
/**
 * getRingItem
 *     Get a ring item from an std::istream
 *  @param in - the stream from which we'll consume the item.
 *  @return ::CRingItem*  - pointer to the new ring item.
 */
::CRingItem*
RingItemFactory::getRingItem(std::istream& in)
{
    v12::RingItemHeader hdr;
    in.read(reinterpret_cast<char*>(&hdr), sizeof(hdr));
    if (!in) {
        return nullptr;            
    }
    v12::CRingItem* pResult = new v12::CRingItem(hdr.s_type, hdr.s_size);
    size_t remaining = hdr.s_size - sizeof(v12::RingItemHeader);
    v12::pRingItem pRawItem =
        reinterpret_cast<v12::pRingItem>(pResult->getItemPointer());
    uint8_t* pCursor = reinterpret_cast<uint8_t*>(&(pRawItem->s_body));
    in.read(reinterpret_cast<char*>(pCursor), remaining);
    if (!in) {
        delete pResult;
        return nullptr;
    }
    pResult->setBodyCursor(pCursor + remaining);
    pResult->updateSize();
    
    return pResult;
}
/**
 * putRingItem
 *    Put a ring item to an std::ostream.
 * @param pItem  - pointer to the item to put.
 * @paraqm out - reference to the ostream
 * @return std::ostream&  - reference to the output stream that we put the item to
 *               errors must be checked by the caller.
 */
std::ostream&
RingItemFactory::putRingItem(const ::CRingItem* pItem, std::ostream& out)
{
    size_t n = pItem->size();
    const void* p = pItem->getItemPointer();
    out.write(reinterpret_cast<const char*>(p), n);
    return out;
}
/**
 * putRingItem
 *   Put a ring item to a file open on an fd:
 *
 *   @param pItem - pointer to the item t put.
 *   @param fd    - File descriptor open on the file.
 *   @throw errors from io::writeData are thrown as exceptions.
 */
void
RingItemFactory::putRingItem(const ::CRingItem* pItem, int fd)
{
    size_t n = pItem->size();
    const void* p = pItem->getItemPointer();
    io::writeData(fd, p, n);
}
/**
 * putRingItem
 *    Put a ring item to a ring buffer.
 *
 * @param pItem - pointer to the itme.
 * @param ring  - reference the ring buffer object.
 */
void
RingItemFactory::putRingItem(const ::CRingItem* pItem, CRingBuffer& ringbuf)
{
    const void* p = pItem->getItemPointer();
    size_t      n = pItem->size();
    
    ringbuf.put(p, n);
}
/**
 *  makeAbnormalEndItem
 *     Create an abnormal end run item:
 *  @return - pointer to a new abnormal end item.
 */
::CAbnormalEndItem*
RingItemFactory::makeAbnormalEndItem()
{
    return new v12::CAbnormalEndItem();
}
/**
 * makeAbnormalEndItem
 *    Create an abnormal end item from an undifferentiated ring item object.
 *    The If the ring item type is not v12::ABNORMAL_ENDRUN
 *    std::bad_cast is thrown.
 * @param rhs - const reference to the ring item we're 'casting'.
 * @return ::CAbnormalEndItem* - pointer to new abnormal end item.
 * @throw std::bad_cast.
 */
::CAbnormalEndItem*
RingItemFactory::makeAbnormalEndItem(const ::CRingItem& rhs)
{
    if (rhs.type() != v12::ABNORMAL_ENDRUN) {
        throw std::bad_cast();
    }
    return new v12::CAbnormalEndItem();   // All look the same.
}
/**
 * makeDataFormatItem.
 *    @return ::CDataFormatItem.
 *    @note   We return one for v12.0.
 */
::CDataFormatItem*
RingItemFactory::makeDataFormatItem()
{
    return new v12::CDataFormatItem();
}
/**
 * makeDataFormatItem
 *    'cast' a ring item to a data format item.
 *    -  The ring item must have type v12::RING_FORMAT
 *    -  The item must also, therefore have it's major version as
 *       v12::FORMAT_MAJOR
 *   @param rhs -The item we're making a differentiated ring item from.
 *   @return CDataFormatItem* - pointer to the newly created item.
 *   @throw std::bad_cast - if one of the tests above fails.
 */
::CDataFormatItem*
RingItemFactory::makeDataFormatItem(const ::CRingItem& rhs)
{
    if (rhs.type() != v12::RING_FORMAT) {
        throw std::bad_cast();
    }
    // This can throw as well:
    
    const ::CDataFormatItem& original(dynamic_cast<const ::CDataFormatItem&>(rhs));
    if (original.getMajor() != v12::FORMAT_MAJOR) {
        throw std::bad_cast();
    }
    return new v12::CDataFormatItem();        // No actual differentiation.
}
/**
 * makeGlomParameters
 *    Make a glom parameters ring item from the actual parameters.
 * @param interval - build interval.
 * @param isBuilding - True if glom is building events
 * @param policy    - Timestamp policy which is one of
 *      *  v12::GLOM_TIMESTAMP_FIRST  output timestamp from first fragment
 *      *  v12::GLOM_TIMESTAMP_LAST   Output timestamp  from last fragment.
 *      *  v12::GLOM_TIMESTAMP_AVERAGE Output timestamp average of all fragments.
 * @return ::CGlomParameters* - pointer to a newly created glom parameters item.
 */
::CGlomParameters*
RingItemFactory::makeGlomParameters(
    uint64_t interval, bool isBuilding, uint16_t policy
)
{
    // Validate the policy:
    
    ::CGlomParameters::TimestampPolicy policySelector;
    switch (policy) {
        case v12::GLOM_TIMESTAMP_FIRST:
            policySelector = ::CGlomParameters::first;
            break;
        case v12::GLOM_TIMESTAMP_LAST:
            policySelector = ::CGlomParameters::last;
            break;
        case v12::GLOM_TIMESTAMP_AVERAGE:
            policySelector = ::CGlomParameters::average;
            break;
        default:
            throw std::invalid_argument("Invalid timestamp policy value");
    }
    return new v12::CGlomParameters(interval, isBuilding, policySelector);
}
/**
 * makeGlomParameters
 *    Create a CGlomParameters object from some undifferentiated ring item.
 * @param rhs - reference to the source item.
 * @throw std::bad_cast if: rhs is not a v12::EVB_GLOM_INFO type item.
 *             or if the size of the item is not sizeof(v12::GlomParametrs).
 */
::CGlomParameters*
RingItemFactory::makeGlomParameters(const ::CRingItem& rhs)
{
    if (rhs.type() != EVB_GLOM_INFO) {
        throw std::bad_cast();
    }
    if (rhs.size() != sizeof(v12::GlomParameters)) {
        throw std::bad_cast();
    }
    
    // this too could throw:
    
    const ::CGlomParameters& item(dynamic_cast<const ::CGlomParameters&>(rhs));
    
    // Make the new item:
    
    return new v12::CGlomParameters(
        item.coincidenceTicks(), item.isBuilding(), item.timestampPolicy()
    );
}

}