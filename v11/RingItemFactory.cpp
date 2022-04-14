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

/** @file:  RingItemFactory.cpp (v11)
 *  @brief: Implement the V11 ring item factory.
 */
#include "RingItemFactory.h"
#include <DataFormat.h>
#include "DataFormat.h"
#include "CRingItem.h"
#include "CAbnormalEndItem.h"
#include "CDataFormatItem.h"
#include "CGlomParameters.h"

#include <string.h>
#include <CRingBuffer.h>
#include <iostream>
#include <unistd.h>
#include <io.h>
#include <stdexcept>
#include <typeinfo>

namespace v11 {
/**
 * makeRingItem
 *    Create an arbitrary ring item:
 *  @param type - ring item type code.
 *  @param maxbody - maximum size of the body.
 *  @return CRingItem* pointer to dynamically allocated ring item.
 */
::CRingItem*
RingItemFactory::makeRingItem(uint16_t type, size_t maxBody)
{
    return new v11::CRingItem(type, maxBody);
}
/**
 * makeRingItem
 *   @param type - item type.
 *   @param timestamp - body header event timestamp.
 *   @param sourceId  - source id for body header.
 *   @param maxbody   - maximum swize.
 *   @param barrierType -body header barrier type.,
 *   @return ::CRingItem* pointer to dynamically allocated ring item.
 */
::CRingItem*
RingItemFactory::makeRingItem(
    uint16_t type, uint64_t timestamp, uint32_t sourceId,
    size_t maxBody, uint32_t barrierType
)
{
    return new v11::CRingItem(type, timestamp, sourceId, barrierType, maxBody);
}
/**
 * makeRingItem
 *  Create a ring item from another ring item.
 *   @param rhs - ring item we're copying.
 *   @note one use for this is as a down cast from a specific to a
 *         generic ring item.
 *   @note there is an assumption that the ring item header
 *        matches the format of the v11::RingItemHeader.
 */
::CRingItem*
RingItemFactory::makeRingItem(const ::CRingItem& rhs)
{
    v11::CRingItem* pItem =  new v11::CRingItem(rhs.type(), rhs.size());
    memcpy(pItem->getItemPointer(), rhs.getItemPointer(), rhs.size());
    
    return pItem;
}
/**
 * makeRingItem
 *    Make a ring item from a raw ring item.
 *  @param pRawRing - raw ring item pointer.
*/
::CRingItem*
RingItemFactory::makeRingItem(const ::RingItem* pRawRing)
{
    v11::CRingItem* pItem = new v11::CRingItem(
        pRawRing->s_header.s_type, pRawRing->s_header.s_size
    );
    memcpy(pItem->getItemPointer(), pRawRing, pRawRing->s_header.s_size);
    return pItem;
}
/**
 * getRingItem
 *     Get a ring item from a ringbuffer (we must be attached as
 *     a consumer).
 *  @param ringbuf - reference to the ring buffer.
 *  @return ::CRingItem* newly allocated ring item.
 *  @note we will block as long as needed.
 */
::CRingItem*
RingItemFactory::getRingItem(CRingBuffer& ringbuf)
{
    v11::RingItemHeader hdr;
    ringbuf.get(&hdr, sizeof(hdr));
    v11::CRingItem* pItem = new v11::CRingItem(hdr.s_type, hdr.s_size);
    size_t remaining = hdr.s_size = sizeof(v11::RingItemHeader);
    v11::pRingItem pItemStorage =
        reinterpret_cast<v11::pRingItem>(pItem->getItemPointer());
    uint8_t* p = reinterpret_cast<uint8_t*>(&(pItemStorage->s_body));
    ringbuf.get(p, remaining);
    p += remaining;
    pItem->setBodyCursor(p);
    pItem->updateSize();
    return pItem;
}
/**
 * getRingItem
 *   @param fd - file descriptor open on the source of ring items.
 *   @return ::CRingItem* points to a dynamically allocated v11 ring item.
 *   @retval nullptr - eof.
 */
::CRingItem*
RingItemFactory::getRingItem(int fd)
{
    v11::RingItemHeader hdr;
    if (io::readData(fd, &hdr, sizeof(hdr)) < sizeof(hdr)) {
        return nullptr;
    }
    v11::CRingItem* pResult = new v11::CRingItem(hdr.s_type, hdr.s_size);
    
    v11::pRingItem pRawItem =
        reinterpret_cast<v11::pRingItem>(pResult->getItemPointer());
    size_t remainingSize = hdr.s_size - sizeof(v11::RingItemHeader);
    uint8_t* p = reinterpret_cast<uint8_t*>(&(pRawItem->s_body));
    if (io::readData(fd, p, remainingSize) != remainingSize) {
        delete p;
        return nullptr;                 // EOF.
    }
    p += remainingSize;
    pResult->setBodyCursor(p);
    pResult->updateSize();
    
    return pResult;
}
/**
 * getRingItem
 *   @param in  - std::istream& open on the ring item data source.
 *   @return ::CRingITem* points to a dynamically allocated v11::CRingItem.
 *   @retval 
 */
::CRingItem*
RingItemFactory::getRingItem(std::istream& in)
{
    v11::RingItemHeader hdr;
    in.read(reinterpret_cast<char*>(&hdr), sizeof(hdr));
    if (!in) {
        return nullptr;            
    }
    v11::CRingItem* pResult = new v11::CRingItem(hdr.s_type, hdr.s_size);
    size_t remaining = hdr.s_size - sizeof(v11::RingItemHeader);
    v11::pRingItem pRawItem =
        reinterpret_cast<v11::pRingItem>(pResult->getItemPointer());
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
 *     Put a ring item into a stream.  This blocks, if necessary
 *     until space is available.
 *  @param pItem - pointer to CRingItem.
 *  @param out   - References the std::ostream into which we put.
 *  @return std::ostream&  stream that was passed in to support chaining.
 *  @note output failures are reflected in the usual manner for
 *        std::ostream objects.
 */
std::ostream&
RingItemFactory::putRingItem(const ::CRingItem* pItem, std::ostream& out)
{
    const void* pData = pItem->getItemPointer();
    size_t bytes      = pItem->size();
    
    return out.write(reinterpret_cast<const char*>(pData), bytes);
    
}
/**
 * putRingItem
 *    Put  a ring item in to a file
 * @param pItem - Item to put.
 * @param fd - file descriptor;
 */
void
RingItemFactory::putRingItem(const ::CRingItem* pItem, int fd)
{
    const void* pData = pItem->getItemPointer();
    size_t bytes      = pItem->size();
    io::writeData(fd, pData, bytes);
}
/**
 * putRingItem
 *    Put a ring item to a ringbuffer.  BLocks until the item is fully
 *    put.
 * @param pItem - Item to put.
 * @param rbuf  - Ring buffer reference.
 */
void
RingItemFactory::putRingItem(const ::CRingItem* pItem, CRingBuffer& rbuf)
{
    rbuf.put(pItem->getItemPointer(), pItem->size());
}
/**
 * makeAbnormalEndItem.
 *   Creates a v11::CAbnormalEndItem and returns it as a base class
 *   pointer
 *  @return ::CAbnormalEndItem*
 */
::CAbnormalEndItem*
RingItemFactory::makeAbnormalEndItem()
{
    return new v11::CAbnormalEndItem;
}
/**
 * makeAbnormalEndITem
 *    Given an arbitrary ring item reference, if the type is
 *    that of an abnormal end item, return an abnormal end item
 *    generated from it.
 *  @param rhs - input ring item.
 *  @return CAbnormalEndItem*
 *  @throws std::bad_cast -if rhs is not an abnormal end item.
 */
::CAbnormalEndItem*
RingItemFactory::makeAbnormalEndItem(const ::CRingItem& rhs)
{
    if (rhs.type() == v11::ABNORMAL_ENDRUN) {
        // there are no contents to speak of so:
        
        return new v11::CAbnormalEndItem;
    } else {
        throw std::bad_cast();
    }
}
/**
 *  makeDataFormatItem.
 *    @return ::CDataFormatItem*  - actually points to a V11::CDataFormatItem.
 */
::CDataFormatItem*
RingItemFactory::makeDataFormatItem()
{
    return new CDataFormatItem;           // Has right versions.
}
/**
 * makeDataFormatItem.
 *    @param rhs - item to turn into a v11 data format item.
 *    @throws std::bad_cast if rhs is not a data format item.
 *    @return ::CDataFormatItem*
 */
::CDataFormatItem*
RingItemFactory::makeDataFormatItem(const ::CRingItem& rhs)
{
    // Require it be a data format item and of our format:
    
    if (rhs.type() == v11::RING_FORMAT) {
        const ::CDataFormatItem& fmt =
            dynamic_cast<const::CDataFormatItem&>(rhs);
        if (fmt.getMajor() != v11::FORMAT_MAJOR) {
            throw std::bad_cast();
        } else {
            return new v11::CDataFormatItem;
        }
    } else {
        throw std::bad_cast();
    }
    
}
/**
 * makeGlomParameters
 *    @param  interval - the build interval
 *    @param  isBuliding - true if building was enabled.
 *    @param  policy     - Event building timestap policy
 *                         determines how the timstamp of the final
 *                         built events are computed.
 *    @return ::CGlomParameters* - actually a pointer to v11::GlomParameters
 */
::CGlomParameters*
RingItemFactory::makeGlomParameters(
    uint64_t interval, bool isBuilding, uint16_t policy
)
{
    ::CGlomParameters::TimestampPolicy ePolicy =
        static_cast<::CGlomParameters::TimestampPolicy>(policy);
    return new CGlomParameters(interval, isBuilding, ePolicy);
}
/**
 * makeGlomParameters
 *    Given a ring item that is alleged to be a glom parameters
 *    produces a new ring item that's a v11::CGlomPolicies item
 *    and hands back a pointer to it:
 * @param rhs - the policy item.
 * @return ::CGlomParameters*
 */
::CGlomParameters*
RingItemFactory::makeGlomParameters(const ::CRingItem& rhs)
{
    if (rhs.type() == v11::EVB_GLOM_INFO) {
        const ::CGlomParameters& glom =
            dynamic_cast<const ::CGlomParameters&>(rhs);
        return new v11::CGlomParameters(
            glom.coincidenceTicks(), glom.isBuilding(), glom.timestampPolicy()
        );
    } else {
        throw std::bad_cast();
    }
}

}