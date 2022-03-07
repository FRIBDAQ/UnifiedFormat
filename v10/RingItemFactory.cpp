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

/** @file:  RingItemFactory.cpp
 *  @brief: Implement the v10 ring item factory.
 */

#include "RingItemFactory.h"
#include "DataFormat.h"
#include "CRingItem.h"
#include <CRingBuffer.h>
#include <io.h>

#include <string.h>
#include <stdint.h>
#include <iostream>
#include <stdexcept>

namespace v10 {
/////////////////////////////////////////////////////////////////////////////
// Factory methods for raw ring items:



/**
 * makeRingItem
 *     Overloaded factory methods to make ring items:
 *
 *  @param type    - Type of ring item to create.
 *  @param maxBody - Largest allowed body.
 *  @param timestamp - event timestamp (not used in v10)
 *  @param sourceId - Data source id (not used in v10).
 *  @param barrierType - Type of barrier (not used in v10).
 *  @param rhs      - Existing CRing item of some sort.
 *  @param pRawRing - Existing ring item that we will copy/wrap.
 *  @return ::CRingItem* - pointer to a ring item that was created.
 *                    this must eventually be destroyed via delete.
 */
// From type and max size.

 ::CRingItem*
 RingItemFactory::makeRingItem(uint16_t type, size_t maxBody)
 {
    return new v10::CRingItem(type, maxBody);
 }
 
 //  from event building parameters
 
 ::CRingItem*
 RingItemFactory::makeRingItem(
    uint16_t type, uint64_t timestamp, uint32_t sourceId,
    size_t maxBody, uint32_t barrierType 
)
 {
    return new v10::CRingItem(type, timestamp);
 }
 
 // From an existing item -
 
 ::CRingItem*
 RingItemFactory::makeRingItem(const ::CRingItem& rhs)
 {
    const ::RingItem* pItem =
        reinterpret_cast<const ::RingItem*>(rhs.getItemPointer());
    return makeRingItem(pItem);

    
    
 }
 // From an existing raw item:
 
 ::CRingItem*
 RingItemFactory::makeRingItem(const ::RingItem* pRawItem)
 {
    const v10::RingItemHeader* pHeader =
        reinterpret_cast<const v10::RingItemHeader*>(&(pRawItem->s_header));
       
    auto result = makeRingItem(pHeader->s_type, pHeader->s_size);
    const void* pBody  = reinterpret_cast<const void*>(pHeader+1);
    uint8_t* p = reinterpret_cast<uint8_t*>(result->getBodyCursor());
    uint32_t bodySize = pHeader->s_size - sizeof(RingItemHeader);
    memcpy(p, pBody, bodySize);
    p += bodySize;
    result->setBodyCursor(p);
    
    return result;    
 }
 // Get ring items from various sources:
 
 /**
  * getRingItem (from CRingBuffer)
  *   @param ringbuf - references a ring buffer that must have been
  *                    created as a consumer.
  *   @return ::CRingItem* pointer to a newly created ring item that must
  *                   be deleted at some point.
  */
 ::CRingItem*
 RingItemFactory::getRingItem(CRingBuffer& ringbuf)
 {
    // Read the header, use it to create a v10 ring item
    // then read the body into it:
    v10::RingItemHeader hdr;
    ringbuf.get(&hdr, sizeof(hdr));
    
    auto result = makeRingItem(hdr.s_type, hdr.s_size);
    uint8_t* p  = reinterpret_cast<uint8_t*>(result->getBodyCursor());
    size_t bodySize = hdr.s_size - sizeof(v10::RingItemHeader);
    ringbuf.get(p, bodySize);
    p += bodySize;
    result->setBodyCursor(p);
    
    return result;
 }
 /**
  * getRingItem (from file descriptor)
  *    @param fd - file descriptor.
  *    @return ::CRingItem* pointer to new ring itemthat must be
  *              destroyed via delete.
  */
 ::CRingItem*
 RingItemFactory::getRingItem(int fd)
 {
    // Read the header then the body:
    
    v10::RingItemHeader hdr;
    io::readData(fd, &hdr, sizeof(hdr));
    
    auto result = makeRingItem(hdr.s_type, hdr.s_size);
    
    uint8_t* p  = reinterpret_cast<uint8_t*>(result->getBodyCursor());
    size_t bodySize = hdr.s_size - sizeof(v10::RingItemHeader);
    io::readData(fd, p, bodySize);
    p += bodySize;
    result->setBodyCursor(p);
    
    return result;
 }
 /**
  * getRingItem (from ios::istream)
  *    @param in - input stream  (note that this must be opened with
  *                io::binary set)
  *     @return ::CRingItem* pointer to new ring itemthat must be
  *              destroyed via delete.
  *     @retval nullptr - if the read could not be done;
  *              the stream will have the reason for that.
  */               
 ::CRingItem*
 RingItemFactory::getRingItem(std::istream& in)
 {
    v10::RingItemHeader hdr;
    in.read(reinterpret_cast<char*>(&hdr), sizeof(hdr));
    
    if(!in) {                         // header read failed.
        // Failed in some way.
        
        return nullptr;
    }
    
    auto result = makeRingItem(hdr.s_type, hdr.s_size);
    char* p  = reinterpret_cast<char*>(result->getBodyCursor());
    size_t bodySize = hdr.s_size - sizeof(hdr);
    
    in.read(p, bodySize);
    if (!in) {                  // body read failed.
        delete result;
        return nullptr;
    }
    p += bodySize;
    result->setBodyCursor(p);
    return result;
    
 }
 // Put ring items to various data sinks.
 
 /**
  *  putRingItem (to stream)
  *     Put a ring item to an std::ostream.
  *   @param pItem - pointer to the item.
  *   @param out   - Output stream (reference)
  *   @return std::ostream& - out after the write(attempt).
  *                 Errors etc. will be in the stream.
  */
 std::ostream&
 RingItemFactory::putRingItem(const ::CRingItem* pItem, std::ostream& out)
 {
    const ::v10::RingItemHeader* hdr =
        reinterpret_cast<const ::v10::RingItemHeader*>(pItem->getItemPointer());
    out.write(reinterpret_cast<const char*>(hdr), hdr->s_size);
    
    return out;
 }
 /**
  * putRingItem (to file descriptor)
  *
  *  @param pItem - pointer to the item.
  *  @param fd    - file descriptor.
  *  @throw errors are reported by the NSCLDAQ io::writeData function
  *         via exceptions.
  */
 void
 RingItemFactory::putRingItem(const ::CRingItem* pItem, int fd)
 {
    const ::v10::RingItemHeader* hdr =
        reinterpret_cast<const ::v10::RingItemHeader*>(pItem->getItemPointer());
    io::writeData(fd, hdr, hdr->s_size);
    
 }
 /**
  * putRingItem (to ring).
  *   @param pItem - pointer to he item.
  *   @param ringbuf - reference to the ring buffer.
  *   @throw exceptions from CRingBUfer::put
  *   @note ringbuf must be opened as a producer.
  */
 void
 RingItemFactory::putRingItem(const ::CRingItem* pItem, CRingBuffer& ringbuf)
 {
    const ::v10::RingItemHeader* hdr =
        reinterpret_cast<const ::v10::RingItemHeader*>(pItem->getItemPointer());
    ringbuf.put(hdr, hdr->s_size);
 }

}                          // v10 namespace.