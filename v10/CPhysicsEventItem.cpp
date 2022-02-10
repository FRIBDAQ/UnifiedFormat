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

#include <sstream>
#include <stdio.h>

namespace v10 {
/*
 * All the canonical methods just delegate to the base class
 */


CPhysicsEventItem::CPhysicsEventItem(size_t maxBody) :
  ::CPhysicsEventItem(maxBody) {}


CPhysicsEventItem::~CPhysicsEventItem() {}


/*--------------------------------------------------
 *
 * Virtual method overrides.
 */

/**
 *   getBodySize.
 *     sadly the hierarchy does not allow me to delegate as we descend from
 *     ::CPhysicsEventItem not from v10::CRingItem so:
 * @return number of bytes in the body
 */
size_t
CPhysicsEventItem::getBodySize() const
{
  auto cThis = const_cast<CPhysicsEventItem*>(this);
  const uint8_t* pB = reinterpret_cast<const uint8_t*>(getBodyPointer());
  const uint8_t* pC = reinterpret_cast<const uint8_t*>(cThis->getBodyCursor());
  
  return pC - pB;
}

/**
 * getBodyPointer
 *     Return a pointer to the body.
 *     sadly the hierarchy does not allow me to delegate as we descend from
 *     ::CPhysicsEventItem not from v10::CRingItem so:
 
 *  @return void*
 */
void*
CPhysicsEventItem::getBodyPointer()
{
  RingItemHeader* pH = reinterpret_cast<RingItemHeader*>(getItemPointer());
  return pH+1;
}
// same as above but const:

const void*
CPhysicsEventItem::getBodyPointer() const
{
  const RingItemHeader* pH = reinterpret_cast<const RingItemHeader*>(getItemPointer());
  return pH+1;   
}
/**
 * getBodyHeader - returns nullptr:
 *
 */
void*
CPhysicsEventItem::getBodyHeader() {
  return nullptr;
}
/**
 *  setBodyHeader
 *     no-op
 */
void
CPhysicsEventItem::setBodyHeader(
  uint64_t timestamp, uint32_t sid, uint32_t barrier
)
{}

/**
 * typeName
 *    Returns the type name associated with the item.
 * 
 * @return std::string  - "Event ".
 */
std::string
CPhysicsEventItem::typeName() const
{
  return "Event ";
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

}