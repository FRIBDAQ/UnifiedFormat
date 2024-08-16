#ifndef V10_CPHYSICSEVENTITEM_H
#define V10_CPHYSICSEVENTITEM_H
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


#include <CPhysicsEventItem.h>		/* Base class ::ufmt::CPhysicsEventItem..*/
#include <unistd.h>
#include <stdint.h>
#include <string>

namespace ufmt {
  namespace v10 {
  /**
   *  This class is a wrapper for physics events.
   *  It's mainly provided so that textual dumps
   *  can be performed as typeName and toString
   *  are the only substantive methods...everything
   *  else just delegates to the base class.
   */

  class CPhysicsEventItem : public ::ufmt::CPhysicsEventItem
  {
  public:
    CPhysicsEventItem(size_t maxBody=8192);
    virtual ~CPhysicsEventItem();

  private:
    CPhysicsEventItem& operator=(const CPhysicsEventItem& rhs);
    int operator==(const CPhysicsEventItem& rhs) const;
    int operator!=(const CPhysicsEventItem& rhs) const;
  public:
    
    // Virtual methods that all ring items must provide:

    virtual size_t getBodySize() const;
    virtual void*  getBodyPointer();
    virtual const void*  getBodyPointer() const;
    
    virtual void* getBodyHeader();
    void setBodyHeader(
      uint64_t timestamp, uint32_t sid, uint32_t barrierType = 0
    );
    
    virtual std::string typeName() const;	// Textual type of item.
    virtual std::string toString() const; // Provide string dump of the item.

  
    
  };
  }
}
#endif
