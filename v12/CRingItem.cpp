/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

// daqdev/NSCLDAQ#1030 changes implemented.


#include "CRingItem.h"
#include "DataFormat.h"


#include <string.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <unistd.h>

namespace v12 {

////////////////////////////////////////////////////////////////////////////////
//
// Constructors and other canonicals.
//

/*!
   Construct the ring item:
   - If the maxbody is larger than CRingItemStaticBufferSize, allocate
     a new buffer and point the item at that, otherwise point it at 
     m_staticBuffer.
   - Pointer m_cursor at the body of the ring item.
   - Calculate and fill in the storage size.
   - Set m_swapNeeded to false.
   - Set the ring item type.

   \param type - The ring item type.  This is only 16 bits wide but stored in a
                 32 bit word so receivers can determine if bytes were swapped.
   \param maxBody - Largest body we can hold

*/
CRingItem::CRingItem(uint16_t type, size_t maxBody) :
  ::CRingItem(type, maxBody)
{
  v12::pRingItemHeader pItem = reinterpret_cast<v12::pRingItemHeader>(getItemPointer());
  uint32_t* pmbz = reinterpret_cast<uint32_t*>(pItem+1);
  *pmbz++ = sizeof(uint32_t);                // v12 style.
  setBodyCursor(pmbz);
  updateSize();
}
/**
 * constructor with body header.
 * This is basically the same as the prior constructor, however a body header
 * is pre-created in the event body.  The size and cursor are updated to
 * reflect the new body start location.
 *
 * @param type        - Ring Item type.
 * @param timestamp   - Event timestamp for the body header.
 * @param sourceid    - Id of the event source.
 * @param barrierType - Type of barrier being created (0 if not a barrier)
 * @param maxbody     - Maximum body size required.
 */
CRingItem::CRingItem(uint16_t type, uint64_t timestamp, uint32_t sourceId,
                     uint32_t barrierType, size_t maxBody) :
    v12::CRingItem(type, maxBody)
{
  // We have a body header:
  
  v12::pRingItemHeader pHeader = reinterpret_cast<v12::pRingItemHeader>(getItemPointer());
  v12::pBodyHeader pBh = reinterpret_cast<v12::pBodyHeader>(pHeader+1);
  pBh->s_size = sizeof(v12::BodyHeader);
  pBh->s_timestamp = timestamp;
  pBh->s_sourceId  = sourceId;
  pBh->s_barrier   = barrierType;
  
  setBodyCursor(pBh+1);
  updateSize();

}
/*!
    Destroy the item. If the storage size was big, we need to delete the 
    storage as it was dynamically allocated.
*/
CRingItem::~CRingItem()
{
  deleteIfNecessary();
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Selectors:

/**
 * getBodySize
 *    @return size_t - number of bytes in the body.
 */
size_t
CRingItem::getBodySize() const
{
   v12::CRingItem* pThis = const_cast<v12::CRingItem*>(this);
   const uint8_t* pBegin =
    reinterpret_cast<const uint8_t*>(v12::CRingItem::getBodyPointer());
   const uint8_t* pEnd   =
    reinterpret_cast<const uint8_t*>(pThis->v12::CRingItem::getBodyCursor());
   return pEnd - pBegin;
}
/**
 * getBodyPointer
 *   @return a Pointer to the body of the item.
 */
const void*
CRingItem::getBodyPointer() const
{
    const v12::RingItem* pItem =
      reinterpret_cast<const v12::RingItem*>(getItemPointer());
    if (!v12::CRingItem::hasBodyHeader()) {
      return pItem->s_body.u_noBodyHeader.s_body;
    } else {
      // We don't assume the body header has the 'right' shape:
      
      uint32_t bhdrSize = pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_size;
      const uint8_t* p =
        reinterpret_cast<const uint8_t*>(&(pItem->s_body.u_hasBodyHeader.s_bodyHeader));
      return p + bhdrSize;
    }
}
void*
CRingItem::getBodyPointer()
{
    v12::RingItem* pItem =
      reinterpret_cast<v12::RingItem*>(getItemPointer());
    if (!v12::CRingItem::hasBodyHeader()) {
      return pItem->s_body.u_noBodyHeader.s_body;
    } else {
      // We don't assume the body header has the 'right' shape:
      
      uint32_t bhdrSize = pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_size;
      uint8_t* p = reinterpret_cast<uint8_t*>(
            &(pItem->s_body.u_hasBodyHeader.s_bodyHeader)
      );
      return p + bhdrSize;
    }
}
/**
 * mustSwap
 *    little endian wins:
 * @return false;
 */
bool
CRingItem::mustSwap() const {
  return false;
}
/**
 * hasBodyHeader
 *    @return bool -true if there's a body header.
 */
bool
CRingItem::hasBodyHeader() const {
  const v12::RingItem* pItem =
      reinterpret_cast<const v12::RingItem*>(getItemPointer());
   return (pItem->s_body.u_noBodyHeader.s_empty > sizeof(uint32_t));
}
/**
 * getBodyHeader
 *    @return void*  - pointer to the item's body header.
 *    @retval nullptr - there's no body header.
 */
void*
CRingItem::getBodyHeader() const
{
     if(v12::CRingItem::hasBodyHeader()) {
        const v12::RingItem* pItem =
            reinterpret_cast<const v12::RingItem*>(getItemPointer());
        return const_cast<v12::BodyHeader*>(&(pItem->s_body.u_hasBodyHeader.s_bodyHeader));
     } else {
        return nullptr;
     }
}
/**
 * getEventTimstamp
 *    @return uint64_t body header timestamp.
 *    @throws std::logic_error -there's no body header.
 *    @note this method assumes the body header is standard v12 format.
 */
uint64_t
CRingItem::getEventTimestamp() const
{
    throwIfNoBodyHeader("getEventTimestamp called on item without body header");
    const v12::BodyHeader* p =
      reinterpret_cast<v12::BodyHeader*>(getBodyHeader());
    return p->s_timestamp;
}
/**
 * getSourceId
 *   @return uint32_t - the sourceid from the body header.
 *   @throws std::logic_error -there's no body header.
 *    @note this method assumes the body header is standard v12 format.
 */
uint32_t
CRingItem::getSourceId() const
{
    throwIfNoBodyHeader("getEventTimestamp called on item without body header");
    const v12::BodyHeader* p =
      reinterpret_cast<v12::BodyHeader*>(getBodyHeader());
    return p->s_sourceId;
}
/**
 * getBarrierType
 *    @return uint32_t - the barrier type code from the body header.
 *    @throws std::logic_error -there's no body header.
 *    @note this method assumes the body header is standard v12 format.
 */
uint32_t
CRingItem::getBarrierType() const
{
    throwIfNoBodyHeader("getEventTimestamp called on item without body header");
    const v12::BodyHeader* p =
      reinterpret_cast<v12::BodyHeader*>(getBodyHeader());
    return p->s_barrier;
}
///////////////////////////////////////////////////////////////////

/**
 * setBodyheader
 *   - If there's no body header, space is created for one by
 *     sliding the data down sizeof(v12::BodyHeader) bytes.
 *   - The contents of the body header are set:
 * @param timestamp - the timestamp to put in the body header.
 * @param sourceId  - The source id ""
 * @param barrierType - the barrier type code ""
 */
void
CRingItem::setBodyHeader(
    uint64_t timestamp, uint32_t sourceId, uint32_t barrierType
)
{
   v12::pBodyHeader pBh;
   if (!v12::CRingItem::hasBodyHeader()) {
       // Create an empty body header:
       
       uint8_t* src = reinterpret_cast<uint8_t*>(v12::CRingItem::getBodyPointer());
       uint8_t* dest = src + sizeof(v12::BodyHeader) - sizeof(uint32_t);
       size_t nBytes= v12::CRingItem::getBodySize();
       memmove(dest, src, nBytes);
       v12::pRingItem pItem = reinterpret_cast<v12::pRingItem>(getItemPointer());
       pBh = &(pItem->s_body.u_hasBodyHeader.s_bodyHeader);
       pBh->s_size= sizeof(v12::BodyHeader);
       
       // Set new body cursor and size:
       
       uint8_t* p = reinterpret_cast<uint8_t*>(v12::CRingItem::getBodyCursor());
       p += sizeof(v12::BodyHeader) - sizeof(uint32_t);
       v12::CRingItem::setBodyCursor(p);
       v12::CRingItem::updateSize();
       
   } else {
    
       pBh = reinterpret_cast<v12::pBodyHeader>(v12::CRingItem::getBodyHeader());
   }
   // We leave the size alone in case an existing body header had an
   // extension.
   
   pBh->s_timestamp = timestamp;
   pBh->s_sourceId  = sourceId;
   pBh->s_barrier   = barrierType;
}

//////////////////////////////////////////////////////////////////////////////////////
//
// Default implementations of virtual methods:
//

/**
 * typeName
 *
 *  Return an std::string that contains the name of the ring item type
 *  (e.g. "Physics data").  The default produces:
 *  "Unknown (0xnn) where 0xnn is the hexadecimal ring item type.
 *
 * @return std::string containing the type as described above.
 */

std::string
CRingItem::typeName() const
{
  std::stringstream typeStr;
  typeStr << "Unknown (" << std::hex << type() << ")"; 
  return typeStr.str();
}   
/**
 * toString
 *
 * Return an std::string that contains a formatted dump of the ring item
 * body. Default implementation just produces a hex-dump of the data
 * the dump has 8 elements per line with spaces between each element
 * and the format of each element is %02x.
 *
 * @return std::string - the dump described above.
 */
std::string
CRingItem::toString() const
{
  std::stringstream  dump;
  const uint8_t*      p     = reinterpret_cast<const uint8_t*>(getBodyPointer());
  size_t              n     = getBodySize(); 
  int                 nPerLine(8);

  dump << bodyHeaderToString();
  
  dump << std::hex << std::setfill('0');

  for (int i = 0; i < n; i++) {
    if ( ((i % nPerLine) == 0)) {
      dump << std::endl;
    }
    dump   << std::setw(2)   << static_cast<unsigned int>(*p++) << " ";

  }
  // If there's no trailing endl put one in.

  if (n % nPerLine) {
    dump << std::endl;
  }
  

  return dump.str();
}





/**
 * bodyHeaderToString
 *
 * return a string representation of the body header.  If the body header
 * does not exist in this ring item returnes "No body header\n"
 *
 * @return std::string
 */
std::string
CRingItem::bodyHeaderToString() const
{
    std::stringstream result;
    
    if (v12::CRingItem::hasBodyHeader()) {
        pBodyHeader pHeader = reinterpret_cast<pBodyHeader>(getBodyHeader());
        result << "Body Header:\n";
        result << "Timestamp:    " << pHeader->s_timestamp << std::endl;
        result << "SourceID:     " << pHeader->s_sourceId  << std::endl;
        result << "Barrier Type: " << pHeader->s_barrier << std::endl;
        
        // 11.4 can have additional body header words.  If those are present
        // they will just be dumped as hex.  The assumption in this
        // implementation is that there won't be many of them:
        
        if (pHeader->s_size > sizeof(BodyHeader)) {
          result << "Additional body header words\n";
          uint16_t* pAdditional = reinterpret_cast<uint16_t*>(pHeader+1);
          size_t nWords = (pHeader->s_size - sizeof(BodyHeader)) / sizeof(uint16_t);
          
          result << std::hex;
          for (int i =0; i < nWords; i++) {
            result << *pAdditional++ << ' ';
          }
          result << std::dec << std::endl;
        }
        
    } else {
        result << "No body header\n";
    }
    return result.str();
}
/**
 * throwIfNoBodyHeader
 *
 * Throw an exception if the event does not have a body header.
 *
 * @param msg - The message to throw.
 */
void
CRingItem::throwIfNoBodyHeader(const char*msg) const
{
    if (!v12::CRingItem::hasBodyHeader()) {
        throw std::logic_error(msg);
    }
}
}                         // v12 namespace