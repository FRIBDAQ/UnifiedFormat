#ifndef CRINGPHYSICSEVENTCOUNTERITEM_H
#define CRINGPHYSICSEVENTCOUNTERITEM_H


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
#include <time.h>
#include <typeinfo>
#include <string>


/*!
   The physics event count item provides periodic informatino about how
   many events have been produced by the DAQ system.  This is provided
   so that applications that sample event data can determine what fraction
   of the data they've managed to keep up with.
   Applications can count the physics events they've seen,
   They can then divide this number by the number of events that
   have actually been generated to get an analysis fraction.

*/
class CRingPhysicsEventCountItem : public CRingItem
{

  // constructors and other canonicals:
public:
  CRingPhysicsEventCountItem();
  CRingPhysicsEventCountItem(uint64_t count,
			     uint32_t timeOffset, unsigned divisor = 1);
  CRingPhysicsEventCountItem(uint64_t count, 
			     uint32_t timeoffset, 
			     time_t stamp, unsigned divisor = 1);
  virtual ~CRingPhysicsEventCountItem();
private:
  CRingPhysicsEventCountItem(const CRingPhysicsEventCountItem& rhs);
  CRingPhysicsEventCountItem& operator=(const CRingPhysicsEventCountItem& rhs);
  int operator==(const CRingPhysicsEventCountItem& rhs) const;
  int operator!=(const CRingPhysicsEventCountItem& rhs) const;
public:


  // public interface:
public:
  uint32_t getTimeOffset() const;
  void     setTimeOffset(uint32_t offset);
  float    computeElapsedTime() const;
  uint32_t getTimeDivisor() const;

  time_t   getTimestamp() const;
  void     setTimestamp(time_t stamp);

  uint64_t getEventCount() const;
  void     setEventCount(uint64_t count);
  
  uint32_t getOriginalSourceId() const;


  // Virtual method overrides:

  virtual void setBodyHeader(uint64_t timestamp, uint32_t sourceId,
                         uint32_t barrierType = 0);
  virtual void* getBodyHeader() const; 

  virtual std::string typeName() const;	// Textual type of item.
  virtual std::string toString() const; // Provide string dump of the item.


private:
  void* fillEventCountBody(
      uint32_t tOffset, uint32_t divisor, uint32_t tStamp,
      uint64_t count, uint32_t sid
  );

};

#endif
