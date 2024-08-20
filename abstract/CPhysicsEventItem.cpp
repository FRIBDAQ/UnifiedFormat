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

#include <iostream>
namespace ufmt {
  /**
   * @file CPhysicsEventItem.cpp
   * @brief  wrapping of CRingItem - just needed to get the toString method.
   * @author Ron Fox <fox@nscl.msu.edu>
   */

  /*
  * All the canonical methods just delegate to the base class
  */


  CPhysicsEventItem::CPhysicsEventItem(size_t maxBody) :
    CRingItem(PHYSICS_EVENT, maxBody) {}
    


  CPhysicsEventItem::CPhysicsEventItem(const CRingItem& rhs) 
    : CRingItem(rhs)
  {
    if (type() != PHYSICS_EVENT) {
      throw std::bad_cast();
    }
  }


  CPhysicsEventItem::CPhysicsEventItem(const CPhysicsEventItem& rhs) :
    CRingItem(rhs) {}

  CPhysicsEventItem::~CPhysicsEventItem() {}

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
   * headerToString
   *    STringify the header of the item.
   * 
   *  @return std::string
   */
  std::string
  CPhysicsEventItem::headerToString() const {
    std::ostringstream out;
    uint32_t  bytes = getBodySize();
    out << "Event " << bytes << " bytes long\n";

    auto result = out.str();
    return result;
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
    
    auto result = out.str();
    return result;

  }
  // toString is adequately handled by the base class.
  
  // In the abstract case, there's no body header so these two required
  // virtual methods throw std::logic_error

  void*
  CPhysicsEventItem::getBodyHeader() const
  {
    throw std::logic_error("Abstract CPhysicsEventItem objects have no body header");
  }

  void
  CPhysicsEventItem::setBodyHeader(
      uint64_t timestamp, uint32_t sourceId,
      uint32_t barrierType)
  {
    throw std::logic_error("Abstract CPhysicsEventItem objects have no body header");
  }
}