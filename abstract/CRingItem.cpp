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

#include <CRingBuffer.h>
#include <CRingSelectionPredicate.h>
#include <string.h>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <unistd.h>

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
  m_pItem(reinterpret_cast<RingItem*>(&m_staticBuffer)),
  m_storageSize(maxBody)
{

  // If necessary, dynamically allocate (big max item).

  newIfNecessary(maxBody);
  uint32_t* pAfter = static_cast<uint32_t*>(fillRingHeader(m_pItem, 0, type)); 
  
  setBodyCursor(pAfter);
  updateSize();
  
  
}

/*!
  Copy construct.  This is actually the same as the construction above, 
  however the item contents get memcpied into our ring body. The caller has to
  ensure that m_pCursor reflects the amount of data copied into the ring body.

  \param rhs  - The source of the copy.
*/
CRingItem::CRingItem(const CRingItem& rhs) :
  m_pItem(reinterpret_cast<RingItem*>(&m_staticBuffer)) // Needed to prevent uncond new.
{
  
 
  // If the storage size is big enough, we need to dynamically allocate
  // our storage

  newIfNecessary(rhs.m_storageSize);

  copyIn(rhs);
}
/*!
    Destroy the item. If the storage size was big, we need to delete the 
    storage as it was dynamically allocated.
*/
CRingItem::~CRingItem()
{
  deleteIfNecessary();
}

/*!
   Assignment.  If necessary destroy the body.  If necessary re-create the body.
   After that it's just a copy in.

   \param rhs  - The object that we are duplicating into *this.
*/
CRingItem&
CRingItem::operator=(const CRingItem& rhs)
{
  
  if (this != &rhs) {
    deleteIfNecessary();
    newIfNecessary(rhs.m_storageSize);
    copyIn(rhs);
  }

  return *this;
}

/*!
   Comparison for equality.. note that true equality may be time consuming
   to determine, as it requires the contents of the items to be equal which will
   require linear time in the size of the item.
   \param rhs  - refers to the item to be compared with *this>
   \return int
   \retval 0   - Not equal\
   \retval 1   - equal

*/
int
CRingItem::operator==(const CRingItem& rhs) const
{
  // short cut by looking at storage size and swap characteristics first:

  if (m_storageSize != rhs.m_storageSize) return 0;
  

  // Now there's nothing for it but to compare the contents:

  return (memcmp(m_pItem, rhs.m_pItem, itemSize(m_pItem)) == 0);
}
/*!
  Inequality is just the logical inverse of equality.  This can take time, see
  operator==

  \param rhs  - Reference to the object *this will be compared to.
  \return int
  \retval 0   - Objects are not unequal
  \retval 1   - Objects are unequal
*/
int
CRingItem::operator!=(const CRingItem& rhs) const
{
  return !(*this == rhs);
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Selectors:

/*!

   \return size_t
   \retval The size allocated to the ring item's body.
*/
size_t
CRingItem::getStorageSize() const
{
  return m_storageSize;
}

/*!
   \return size_t
   \retval Amount of data in the body.  This is the difference between the 
           cursor and the start of the body.  This does not include the
           body header if one exists.
*/
size_t
CRingItem::getBodySize() const
{
  return (reinterpret_cast<const uint8_t*>(m_pCursor) - reinterpret_cast<const uint8_t*>(getBodyPointer()));
}
/*!
  \return void*
  \retval Pointer to the body of the ring item. To get the next insertion point,
          see getBodyCursor.
  @note - THe base class knows nothing of body headers.  This gets overidden
          in the concrete versions of the class that have body headers:
*/
const RingItem*
CRingItem::getBodyPointer() const
{
  const RingItem* p = getItemPointer();
  p++;
  return p;
  
    
}
pRingItem
CRingItem::getBodyPointer()
{
  const RingItem* p = getBodyPointer();
  return const_cast<pRingItem>(p);
}
/*!
   \return void*
   \retval Pointer to the next insertion point of the body
*/
void*
CRingItem::getBodyCursor()
{
  return m_pCursor;
}
/*!
   \return void*
   \retval Pointer to the body.  To be usually used by derived classes but...
*/
pRingItem
CRingItem::getItemPointer()
{
  return m_pItem;
}

/*!
   \return void*
   \retval Pointer to the full ring item.  To be usually used by derived classes but...
*/
const _RingItem*
CRingItem::getItemPointer() const
{
  return m_pItem;
}
/*!
   \return uint32_t
   \retval Current type of the item.
*/
uint32_t
CRingItem::type() const
{
  return m_pItem->s_header.s_type;
}
/*!
   \return uint32_t
   \retval Current size of the item.
*/
uint32_t
CRingItem::size() const
{
  
  return m_pItem->s_header.s_size;
  
}

/**
 * hasBodyHeader
 *
 * @return bool - true if the item has a body header, false otherwise.
 */
bool
CRingItem::hasBodyHeader() const
{
    return false;
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
    return 0;            // Should not get here.
    
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
    return 0;
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
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////
//
// Mutators.

/*!
  Update the body cursor to reflect data that was put in the item.
  \param pNewCursor - New vlaue for the body cursor.
 
*/
void
CRingItem::setBodyCursor(void* pNewCursor)
{
  m_pCursor = reinterpret_cast<uint8_t*>(pNewCursor);
}

/*!
** Given the current item cursor set the size of the item.
*/
void
CRingItem::updateSize()
{
  pRingItemHeader ph = &(getItemPointer()->s_header);
  uint8_t* b       = reinterpret_cast<uint8_t*>(ph);
  uint8_t* e       = reinterpret_cast<uint8_t*>(getBodyCursor());
  ph->s_size = e-b;
  
}


///////////////////////////////////////////////////////////////////////////////////////
//
//   Object operations.

/*!
   Commit the current version of the ring data to a ring.  
   - Calculates the size field of the header
   - puts the data in the ring buffer.

   \param ring  - Reference to the ring buffer in which the item will be put.

   \note The invoking process must already be the producing process for the ring.
   \note This implementation has no mechanism to timeout the put, however that could be
         added later.
   \note This function is non-destructive. There's nothing to stop the caller from
         issuing it on several rings.
   
*/
void
CRingItem::commitToRing(CRingBuffer& ring)
{
  
    updateSize();
    ring.put(m_pItem, m_pItem->s_header.s_size);
}

/*!
   This is primarily intended for items that have been constructed via
   the getFromRing static member.
   \return bool
   \retval true - The byte order on the generating system is different from that of
                  this system.
   \retval false - The byte order on the generating system is the same as that of this
                  system.
*/
bool
CRingItem::mustSwap() const
{
  return false;
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

///////////////////////////////////////////////////////////////////////////////////////
//
//  Static class methods.



///////////////////////////////////////////////////////////////////////////////////////
//
// Private utilities.
//


/*
 * Common code for copy construction and assignment,
 * copies the contents of some source item into *this.
 * We assume that the final values for m_pCursor and m_pItem
 * have already been set.
 */
void
CRingItem::copyIn(const CRingItem& rhs)
{
  m_storageSize   = rhs.m_storageSize;
  newIfNecessary(m_storageSize);
  

  memcpy(m_pItem, rhs.m_pItem, itemSize(rhs.m_pItem));

  
  
  // where copyin is used, our cursor is already pointing at the body of the item.
  // therefore when updating it we need to allow for that in the arithmetic below.

  auto pCursor    = reinterpret_cast<uint8_t*>(m_pItem);
  pCursor   += rhs.size();
  m_pCursor = pCursor;
  
  updateSize();
}


/*
 *   If necessary, delete dynamically allocated buffer space.
 */
void 
CRingItem::deleteIfNecessary()
{
  if ((m_pItem != (pRingItem)m_staticBuffer) ) {
    delete [](reinterpret_cast<uint8_t*>(m_pItem));
    m_pItem = (pRingItem)(m_staticBuffer);   // No ned to delete now.
  }
}
/*
 *  If necessary, create dynamically allocated buffer space and point
 * m_pItem at it.
 * Note:  This must be done early in the life cycle of the ring item as
 * the contents of old storage are not copied into the new storage.
 */
void
CRingItem::newIfNecessary(uint32_t size)
{
  if (size > CRingItemStaticBufferSize) {
    deleteIfNecessary();                     // In some cases we get called more than once.
    m_pItem  = reinterpret_cast<RingItem*>(new uint8_t[size + sizeof(RingItemHeader) + 100]);
  }
  else {
    m_pItem = reinterpret_cast<RingItem*>(m_staticBuffer);
  }
  m_pCursor= reinterpret_cast<uint8_t*>(&(m_pItem->s_body));

}
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

/**
 * fillRingHeader
 *    @param p - pointer to a raw ring item.
 *    @param size - size value to fill in.
 *    @param type - ring item type to fill in
 *    @return void* points just after the header.
 */
void*
CRingItem::fillRingHeader(pRingItem p, uint32_t size, uint32_t type)
{
  p->s_header.s_size = size;
  p->s_header.s_type = type;
  pRingItemHeader ph = reinterpret_cast<pRingItemHeader>(p);
  return reinterpret_cast<void*>(ph+1);
}