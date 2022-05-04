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
#include "DataFormat.h"
#include "CRingItem.h"
#include <sstream>
#include <stdio.h>

#include <iostream>
/**
 * @file CPhysicsEventItem.cpp
 * @brief  wrapping of CRingItem - just needed to get the toString method.
 * @author Ron Fox <fox@nscl.msu.edu>
 */

namespace v12 {

/**
 *  construct with no body header:
 *
 *    @param maxBody - approximate maximum body size.
 */

CPhysicsEventItem::CPhysicsEventItem(size_t maxBody) :
  ::CPhysicsEventItem(maxBody)
{
  v12::pPhysicsEventItem pItem =
    reinterpret_cast<v12::pPhysicsEventItem>(getItemPointer());
  
  pItem->s_header.s_type = v12::PHYSICS_EVENT;
  pItem->s_body.u_noBodyHeader.s_empty = sizeof(uint32_t);
  
  setBodyCursor(pItem->s_body.u_noBodyHeader.s_body);
  updateSize();
}
  
CPhysicsEventItem::CPhysicsEventItem(
    uint64_t timestamp, uint32_t source, uint32_t barrier, size_t maxBody) :
    ::CPhysicsEventItem(maxBody)
{
    v12::pPhysicsEventItem pItem =
      reinterpret_cast<v12::pPhysicsEventItem>(getItemPointer());
    
    pItem->s_header.s_type = v12::PHYSICS_EVENT;
    pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_size = sizeof(v12::BodyHeader);
    pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_timestamp = timestamp;
    pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_sourceId  = source;
    pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_barrier   = barrier;
    
    setBodyCursor(pItem->s_body.u_hasBodyHeader.s_body);
    updateSize();
}

// destructor

CPhysicsEventItem::~CPhysicsEventItem()
{}

/*--------------------------------------------------
 *
 * Virtual method overrides.
 */

/**
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
 * toString
 *
 *  Convert the event to a string.
 *
 * @return std::string - stringified versino of the event.
 */
std::string
CPhysicsEventItem::toString() const
{
  std::ostringstream out;
  uint32_t  bytes = getBodySize();
  uint32_t  words = bytes/sizeof(uint16_t);
  const uint16_t* body  = reinterpret_cast<const uint16_t*>((const_cast<CPhysicsEventItem*>(this))->getBodyPointer());

  out << "Event " << bytes << " bytes long\n";
  const v12::CRingItem* This = reinterpret_cast<const v12::CRingItem*>(this);
  out << This->v12::CRingItem::bodyHeaderToString();

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
// These do a sort of sideways cast to v12::CRingItem to delegate to it.

/**
 * hasBodyHeader
 *   @return bool - true if the item has a body header.
 */
bool
CPhysicsEventItem::hasBodyHeader() const
{
  const v12::CRingItem* This =
    reinterpret_cast<const v12::CRingItem*>(this);
  
  return This->v12::CRingItem::hasBodyHeader();
}
/**
 * getBodyHeader
 *    @return void* - pointer to the item's body header.
 *    @retval nullptr - if the body has no body header.
 */
void*
CPhysicsEventItem::getBodyHeader() const
{
  const v12::CRingItem* This =
    reinterpret_cast<const v12::CRingItem*>(this);
    
  return This->v12::CRingItem::getBodyHeader();
}
/**
 * setBodyHeader
 *    Modifies an existing or, if necessary, inserts a new body header
 * @param tstamp - timestamp
 * @param sourceid - fragment source id.
 * @param barrierType - type of barrier.
 */
void
CPhysicsEventItem::setBodyHeader(
   uint64_t tstamp, uint32_t sourceId, uint32_t barrierType
)

{
  v12::CRingItem* pThis = reinterpret_cast<::v12::CRingItem*>(this);
  pThis->v12::CRingItem::setBodyHeader(tstamp, sourceId, barrierType);
}
}                     // V12 namespace.