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

////////////////////////////////////////////////////////////////////////////////
//
// Constructors and other canonicals.
//
namespace ufmt {
  namespace v10 {
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
    ::ufmt::CRingItem(type, maxBody)
    
  {
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


  /*!
    \return size_t
    \retval Amount of data in the body.  This is the difference between the 
            cursor and the start of the body.
  */
  size_t
  CRingItem::getBodySize() const
  {
    v10::pRingItem pItem = reinterpret_cast<v10::pRingItem>(m_pItem);
    uint8_t* pc = reinterpret_cast<uint8_t*>(m_pCursor);
    return (pc - pItem->s_body);
  }
  /*!
    \return void*
    \retval Pointer to the body of the ring item. To get the next insertion point,
            see getBodyCursor.
  */
  void*
  CRingItem::getBodyPointer() 
  {
    v10::pRingItemHeader ph = reinterpret_cast<v10::pRingItemHeader>(m_pItem);
    return ph+1;
  }
  const void*
  CRingItem::getBodyPointer() const
  { 
    const v10::RingItemHeader* ph = reinterpret_cast<const v10::RingItemHeader*>(m_pItem);
    return ph+1;
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //
  //   Object operations.


  //////////////////////////////////////////////////////////////////////////////////////
  //
  // Default implementations of virtual methods:
  //

  /**
   * getoBodyHeader
   *    No such thing - in this case return a void*
   */
  void*
  CRingItem::getBodyHeader() const {
    return nullptr;
  }
  /**
   * setBodyHeader
   *    no op since we have no body headers in v 10.
   */
  void
  CRingItem::setBodyHeader(
    uint64_t timestamp, uint32_t sourceId, uint32_t barrierType
  )
  {}

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
   
  }              // v10 namespace
}                // ufmt namespace.