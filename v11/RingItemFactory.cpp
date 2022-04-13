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

#include <string.h>
#include <CRingBuffer.h>
#include <iostream>
#include <unistd.h>

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

}