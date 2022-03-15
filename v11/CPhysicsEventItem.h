#ifndef V11_CPHYSICSEVENTITEM_H
#define V11_CPHYSICSEVENTITEM_H
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

#include <CPhysicsEventItem.h>

namespace v11 {

/**
 *  This class is a wrapper for physics events.
 *  It's mainly provided so that textual dumps
 *  can be performed as typeName and toString
 *  are the only substantive methods...everything
 *  else just delegates to the base class.
 */

class CPhysicsEventItem : public ::CPhysicsEventItem
{
public:
  CPhysicsEventItem(size_t maxBody=8192);
  CPhysicsEventItem(                                 // Our factory can use this.
       uint64_t timestamp, uint32_t source, uint32_t barrier,
       size_t maxBody=8192
   );

  virtual ~CPhysicsEventItem();
private:

  CPhysicsEventItem(const CRingItem& rhs) ;
  CPhysicsEventItem(const CPhysicsEventItem& rhs);


  CPhysicsEventItem& operator=(const CPhysicsEventItem& rhs);
  int operator==(const CPhysicsEventItem& rhs) const;
  int operator!=(const CPhysicsEventItem& rhs) const;
public:
  // We need to overrride these in ::CRingItem our ultimate
  // base class.  We'll delegate them to v11::CRingItem.
  //
  virtual size_t getBodySize()    const;
  virtual const void*  getBodyPointer() const;
  virtual void* getBodyPointer();
  virtual bool hasBodyHeader() const;
  virtual uint64_t getEventTimestamp() const;
  virtual uint32_t getSourceId() const;
  virtual uint32_t getBarrierType() const;

  
  
  // Virtual methods that all ring items must provide:

  virtual std::string typeName() const;	// Textual type of item.
  virtual std::string toString() const; // Provide string dump of the item.

  virtual void* getBodyHeader() const;
  virtual void setBodyHeader(
        uint64_t timestamp, uint32_t sourceId,
        uint32_t barrierType = 0
  );

 
 
};
}

#endif
