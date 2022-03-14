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

#include "CRingItem.h"
#include "DataFormat.h"

#include <string.h>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
namespace v11 {
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

  // Set a null body header
  
  uint32_t* pNullHeader = reinterpret_cast<uint32_t*>(getBodyCursor());
  *pNullHeader++ = 0;                         // Strict 11.
  setBodyCursor(pNullHeader);
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
  ::CRingItem(type, maxBody)
{
  
  v11::pBodyHeader pHeader = reinterpret_cast<pBodyHeader>(getBodyCursor());
  pHeader->s_size      = sizeof(BodyHeader);
  pHeader->s_timestamp = timestamp;
  pHeader->s_sourceId  = sourceId;
  pHeader->s_barrier   = barrierType;
  
  setBodyCursor(pHeader+1);
  updateSize();
  
}
/*!
    Destroy the item. If the storage size was big, we need to delete the 
    storage as it was dynamically allocated.
*/
CRingItem::~CRingItem()
{
  
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Selectors:



/*!
   \return size_t
   \retval Amount of data in the body.  This is the difference between the 
           cursor and the start of the body.  This does not include the
           body header if one exists.
    @note if the client has put data in the item body but not yet updated the
          body cursor,  we know nothing about that and, therefore, cannot include
          it in the size.
*/
size_t
CRingItem::getBodySize() const
{
  auto pThis = const_cast<CRingItem*>(this);
  const uint8_t* pCursor = reinterpret_cast<const uint8_t*>(pThis->getBodyCursor());
  return (pCursor - reinterpret_cast<const uint8_t*>(getBodyPointer()));
}
/*!
  \return void*
  \retval Pointer to the body of the ring item. To get the next insertion point,
          see getBodyCursor.
*/
const void*
CRingItem::getBodyPointer() const
{
    // The result depends on whether or not the item has a body header:
    // daqdev/NSCLDAQ#966  - use the body header size to figure out
    // where the body  pointer is... this allows custom body headers to work
    // just fine.
    // We just treat a 0 as sizeof(uint32_t).  All of this works because
    // ring items are packed structs.
    
    
    const v11::RingItem* pItem = reinterpret_cast<const v11::RingItem*>(getItemPointer());
    
    
    // Get the body header size.  Note this computation allows for
    // a body header extension as was used in e.g. e17011.
    
    size_t bhdrSize = pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_size;
    if (bhdrSize == 0) bhdrSize = sizeof(uint32_t);
    
    const uint8_t* pBody =
      reinterpret_cast<const uint8_t*>(&pItem->s_body.u_noBodyHeader) + bhdrSize;
    return pBody;
  
  

}
/**
 * getBodyPointer
 *    Return a writable body pointer.
 */
void*
CRingItem::getBodyPointer()
{
     const CRingItem* cP = const_cast<const CRingItem*>(this); // force const call?
     const void* pResult = cP->getBodyPointer();
     return const_cast<void*>(pResult);                        // throw away the const.
}

/**
 * hasBodyHeader
 *
 * @return bool - true if the item has a body header, false otherwise.
 */
bool
CRingItem::hasBodyHeader() const
{
    const RingItem* pItem = reinterpret_cast<const RingItem*>(getItemPointer());
    return (pItem->s_body.u_noBodyHeader.s_mbz > sizeof(uint32_t));  // Thinking ahead to v12.
}
/**
 * getEventTimestamp
 *
 * @return uint64_t - returns the timestamp from the body header.
 * @throws std::string - if the item has no body header.
 */
uint64_t
CRingItem::getEventTimestamp() const
{
    throwIfNoBodyHeader(
        "Attempted to get a timestamp from an event that does not have one"
    );
    auto pThis = const_cast<CRingItem*>(this);
    const BodyHeader* pHeader = reinterpret_cast<const BodyHeader*>(pThis->getBodyHeader());
    return pHeader->s_timestamp;
}
/**
 * getSourceId
 *
 * @return uint32_t the id of the data source that contributed this event.
 *
 * @throw std::string - if this event does ot have a body header.
 */
uint32_t
CRingItem::getSourceId() const
{
    throwIfNoBodyHeader(
        "Attempted to get the source ID from an event that does not have one"
    );
    auto pThis = const_cast<CRingItem*>(this);
    const BodyHeader* pHeader = reinterpret_cast<const BodyHeader*>(pThis->getBodyHeader());
    return pHeader->s_sourceId;
}
/**
 * getBarrierType
 *
 * @return uint32_t - Barrier type in the event.
 * @throw std::string - If this event does not have an event header.
 */
uint32_t
CRingItem::getBarrierType() const
{
    throwIfNoBodyHeader(
        "Attempted to get the barrier type from an event that does not have one"
    );
    auto pThis = const_cast<CRingItem*>(this);
    const BodyHeader* pHeader = reinterpret_cast<const BodyHeader*>(pThis->getBodyHeader());
    return pHeader->s_barrier;
    
}

///////////////////////////////////////////////////////////////////////////////////////
//
// Mutators.



/**
 * setBodyHeader
 *
 * Sets a body header to the desired values.  If the event does not yet
 * have a body header, space is created for by sliding the existing data
 * down in the buffer. Clearly then, for large events, it is much quicker to
 * construct the ring item with the wrong header and then call this to get the
 * header right than it is to use this method to add a header to an event that
 * has none
 *
 * @param timestamp   - The event timestamp
 * @param sourceId    - Id of the source that contributed this item.
 * @param barrierType - Type of the item.
 * @note If the client to this object is holding a body cusror, that cursor
 *       is invalidated by this call (well strictly speaking only if theres'
 *       note yet a body header).
 */

void
CRingItem::setBodyHeader(uint64_t timestamp, uint32_t sourceId,
    uint32_t barrierType)
{
    pRingItem pItem = reinterpret_cast<pRingItem>(getItemPointer());
    if (!hasBodyHeader()) {
        // Make space for the body header.
        
        uint8_t* pBody = (pItem->s_body.u_noBodyHeader.s_body);
        size_t moveSize = sizeof(BodyHeader) - sizeof(uint32_t);
        size_t moveCount= pItem->s_header.s_size - sizeof(RingItemHeader) - sizeof(uint32_t);
        memmove(pBody + moveSize, pBody, moveCount);
        
        // Update the cursor as well
        
        uint8_t* pCursor = reinterpret_cast<uint8_t*>(getBodyCursor());
        pCursor += moveSize;
        setBodyCursor(pCursor);

    }
    pBodyHeader pHeader = &(pItem->s_body.u_hasBodyHeader.s_bodyHeader); //Getbody header won't yet work.
    pHeader->s_size = sizeof(BodyHeader);
    pHeader->s_timestamp = timestamp;
    pHeader->s_sourceId  = sourceId;
    pHeader->s_barrier   = barrierType;
    updateSize();
    
}
/**
 * getBodyHeader
 *    @return Pointer to the item's body header.
 *    @retval nullptr - the item has no body header.
 */
void*
CRingItem::getBodyHeader()
{
     if(hasBodyHeader()) {
          pRingItem pItem = reinterpret_cast<pRingItem>(getItemPointer());
          return &(pItem->s_body.u_hasBodyHeader.s_bodyHeader);
     } else {
          return nullptr;
     }
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


////////////////////////////////////////////////////////////////////////////////
// Protected methods:

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

    if (hasBodyHeader()) {
        const RingItem* pItem = reinterpret_cast<const RingItem*>(getItemPointer());
        const BodyHeader* pHeader = &(pItem->s_body.u_hasBodyHeader.s_bodyHeader);
        result << "Body Header:\n";
        result << "Timestamp:    " << pHeader->s_timestamp << std::endl;
        result << "SourceID:     " << pHeader->s_sourceId  << std::endl;
        result << "Barrier Type: " << pHeader->s_barrier << std::endl;

    } else {
        result << "No body header\n";
    }
    return result.str();
}
///////////////////////////////////////////////////////////////////////////////////////
//
// Private utilities.
//


/**
 * throwIfNoBodyHeader
 *
 * Throw an exception if the event does not have a body header.
 *
 * @param msg - The message to throw.
 */
void
CRingItem::throwIfNoBodyHeader(std::string msg) const
{
    if (!hasBodyHeader()) {
        throw msg;
    }
}




}                                 // v11 namespace