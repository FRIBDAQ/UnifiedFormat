#ifndef V12_CRINGITEM_H
#define V12_ CRINGITEM_H

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

#include <CRingItem.h>    // abstract ring item.
#include <unistd.h>
#include <stdint.h>
#include <string>

namespace v12 {
/*!  
  This class is a base class for objects that encapsulate ring buffer items
  (as defined in DataFormat.h).  One interesting wrinkle is used to optimize.
  Most items will be small.  For bunches of small items, data allocation/free can
  dominate the performance over the transfer of data into smaller items.
  Therefore, each object will have a 'reasonably size' local, static storage
  for data.  If the requested body size fits in this local static storage, it will
  be used rather than doing an extra memory allocation on construction and free on
  destruction.  

  The body is meant to be filled in by getting the cursor, referencing/incrementing
  it and then storing the cursor back.

*/
class CRingItem : public ::CRingItem
{

public:
  CRingItem(
      uint16_t type,
      size_t maxBody = CRingItemStaticBufferSize - 10
  );
  CRingItem(uint16_t type, uint64_t timestamp, uint32_t sourceId,
            uint32_t barrierType = 0, size_t maxBody = CRingItemStaticBufferSize - 10);
  virtual ~CRingItem();

private:
  CRingItem(const CRingItem& rhs);
  
  
  CRingItem& operator=(const CRingItem& rhs);
  int operator==(const CRingItem& rhs) const;
  int operator!=(const CRingItem& rhs) const;



  // Selectors:

public:
  
  virtual size_t getBodySize()    const;
  virtual const void*  getBodyPointer() const;
  virtual void*  getBodyPointer();

  virtual bool mustSwap() const;
  virtual bool hasBodyHeader() const;
  virtual void* getBodyHeader() const;
  virtual uint64_t getEventTimestamp() const;
  virtual uint32_t getSourceId() const;
  virtual uint32_t getBarrierType() const;

  // Mutators:

public:
  virtual void setBodyHeader(uint64_t timestamp, uint32_t sourceId,
                     uint32_t barrierType = 0);

  
  virtual std::string typeName() const;	// Textual type of item.
  virtual std::string toString() const; // Provide string dump of the item.

private:
  std::string bodyHeaderToString() const;
  void throwIfNoBodyHeader(const char* msg) const;
  
};                   // v12::CRingItem.
}                    // namespace.
#endif
