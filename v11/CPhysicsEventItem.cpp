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

#include "CPhysicsEventItem.h"
#include "CRingItem.h"
#include "DataFormat.h"
#include <sstream>
#include <stdio.h>
#include <iostream>

namespace ufmt {

  namespace v11 {
  /**
   * @file CPhysicsEventItem.cpp
   * @brief  wrapping of CRingItem - just needed to get the toString method.
   * @author Ron Fox <fox@nscl.msu.edu>
   */

  /*
  * All the canonical methods just delegate to the base class
  */


  /**
   * constructor
   *    Just create a minimal, body-headerless physics event item.
   */
  CPhysicsEventItem::CPhysicsEventItem(size_t maxBody) : ::ufmt::CPhysicsEventItem(maxBody)
  {
      v11::pPhysicsEventItem pItem = reinterpret_cast<v11::pPhysicsEventItem>(getItemPointer());
      pItem->s_header.s_type = v11::PHYSICS_EVENT;
      
      uint32_t* p = reinterpret_cast<uint32_t*>(getBodyCursor());
      *p++ = 0;                                   // No body header. (v11 style).
      setBodyCursor(p);
      updateSize();
  }
  /**
   * constructor
   *     Create a minimal physics item with a body header.
   * @param timestamp - event timestamp to put in the body header.
   * @param source   - source id to put in the body header.
   * @param barrier  - Barrier type to put in the body header.
   * @param maxBody  - Largest body size.
   */
  CPhysicsEventItem::CPhysicsEventItem(
      uint64_t timestamp, uint32_t source, uint32_t barrier, size_t maxBody) :
      ::ufmt::CPhysicsEventItem(maxBody)
  {
      v11::pPhysicsEventItem pItem = reinterpret_cast<v11::pPhysicsEventItem>(getItemPointer());
      
      pItem->s_header.s_type = v11::PHYSICS_EVENT;
      v11::pBodyHeader pHeader = reinterpret_cast<v11::pBodyHeader>(&(pItem->s_body));
      pHeader->s_size = sizeof(v11::BodyHeader);
      pHeader->s_timestamp = timestamp;
      pHeader->s_sourceId  = source;
      pHeader->s_barrier   = barrier;
      
      setBodyCursor(pHeader+1);
      updateSize();
      
  }

  CPhysicsEventItem::~CPhysicsEventItem() {}


  /*--------------------------------------------------
  *
  * Virtual method overrides... in many cases these delegate to v11::CRingItem.
  */

  
  /**
    * getBodySize
    *   @return size_t size of the body (exclusive of any body header).
    */
  size_t
  CPhysicsEventItem::getBodySize() const
  {
        const v11::CRingItem* pItem = reinterpret_cast<const v11::CRingItem*>(this);
        return pItem->CRingItem::getBodySize();
  }
  /**
    * getBodyPointer
    *    Get a pointer to the body -- regardless of the presence or absence
    *    of a body header.
    * @return const void* Body pointer
    */
  const void*
  CPhysicsEventItem::getBodyPointer() const
  {
      const v11::CRingItem* pItem = reinterpret_cast<const v11::CRingItem*>(this);
      
      return pItem->CRingItem::getBodyPointer();
  }
  void*
  CPhysicsEventItem::getBodyPointer()
  {
    v11::CRingItem* pItem = reinterpret_cast<v11::CRingItem*>(this);
    return pItem->CRingItem::getBodyPointer();
    
  }
  /**
    * hasBodyHeader
    *    @return bool -true if the item does have a body header.
    *
    */
  bool
  CPhysicsEventItem::hasBodyHeader() const
  {
        const v11::CRingItem* pItem = reinterpret_cast<const v11::CRingItem*>(this);
      
      return pItem->CRingItem::hasBodyHeader();
  }
  /**
   * getEventTimestamp
   *    @return uint64_t - the timestamp else throws.
   */
  uint64_t
  CPhysicsEventItem::getEventTimestamp() const
  {
      const v11::CRingItem* pItem = reinterpret_cast<const v11::CRingItem*>(this);
      
      return pItem->CRingItem::getEventTimestamp();
  }
  /**
   * getSourceId
   *    @return uint32_t  - the source id or throws if no body header.
   */
  uint32_t
  CPhysicsEventItem::getSourceId() const
  {
      const v11::CRingItem* pItem = reinterpret_cast<const v11::CRingItem*>(this);
      
      return pItem->CRingItem::getSourceId();
  }
  /**
   * getBarrierType
   *    @return uint32_  - the barrier type or throws if no body header.
   */
  uint32_t
  CPhysicsEventItem::getBarrierType() const
  {
      const v11::CRingItem* pItem = reinterpret_cast<const v11::CRingItem*>(this);
      
      return pItem->CRingItem::getBarrierType();
  }
  /**
   *
   * typeName
   *    Returns the type name associated with the item.
   * 
   * @return std::string  - "Event".
   */
  std::string
  CPhysicsEventItem::typeName() const
  {
    return "Event";
  }

  /**
   * bodyToString
   *
   *  Convert the event body to a string.
   *
   * @return std::string - stringified versino of the event.
   */
  std::string
  CPhysicsEventItem::bodyToString() const
  {
    std::ostringstream out;
    uint32_t  bytes = getBodySize();
    uint32_t  words = bytes/sizeof(uint16_t);
    const uint16_t* body  = reinterpret_cast<const uint16_t*>((const_cast<CPhysicsEventItem*>(this))->getBodyPointer());

    
    const v11::CRingItem* pItem = reinterpret_cast<const v11::CRingItem*>(this);
    out << pItem->bodyHeaderToString();

    int  w = out.width();
    char f = out.fill();

    
    for (int i =1; i <= words; i++) {
      char number[32];
      sprintf(number, "%04x ", *body++);
      out << number;
      if ( (i%8) == 0) {
        out << std::endl;
      }
    }
    out << std::endl;
    
    
    return out.str();

  }
  /**
   * getBodyHeader
   *  @return void*  Return a pointer to the body header or nullptr if there is none
   */
  void*
  CPhysicsEventItem::getBodyHeader() const
  {
    const v11::CRingItem* pItem = reinterpret_cast<const v11::CRingItem*>(this);
      
      return pItem->CRingItem::getBodyHeader();
  }

  /**
   * setBodyHeader
   *     Sets the body header contents.  If a body header exists its contents
   *     are replaced in place.  If not the item's contents are moved to make
   *     space for a new body header and that's filled in.
   * @param timestamp - event timestamp for the body header.
   * @param sourceId  - Source id for the body header.
   * @param barrierType - Barrier type.
   * 
   */
  void
  CPhysicsEventItem::setBodyHeader(
        uint64_t timestamp, uint32_t sourceId,
        uint32_t barrierType
  )
  {
    v11::CRingItem* pItem = reinterpret_cast<v11::CRingItem*>(this);
    pItem->CRingItem::setBodyHeader(timestamp, sourceId, barrierType);
  }

  }                                    // v11 namespace
}