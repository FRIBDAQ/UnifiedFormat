#ifndef V12_CRINGTEXTITEM_H
#define V12_CRINGTEXTITEM_H

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

#include <CRingTextItem.h>    // Abstract

namespace v12 {

/*!
  The text ring item provides a mechanism to put an item in/take an item out of 
  a ring buffer that consists of null terminated text strings.  
*/
class CRingTextItem : public ::CRingTextItem
{
  // Private data:


public:
  // Constructors and other canonicals:

  CRingTextItem(uint16_t type, size_t maxsize);
  CRingTextItem(uint16_t type,
		std::vector<std::string> theStrings);
  CRingTextItem(uint16_t type,
		std::vector<std::string> theStrings,
		uint32_t                 offsetTime,
		time_t                   timestamp, uint32_t divisor=1) ;
  CRingTextItem(
    uint16_t type, uint64_t eventTimestamp, uint32_t source, uint32_t barrier,
    std::vector<std::string> theStrings, uint32_t offsetTime, time_t timestamp,
    int offsetDivisor = 1
  );
  virtual ~CRingTextItem();
private:  
  CRingTextItem(const CRingItem& rhs) ;
  CRingTextItem(const CRingTextItem& rhs);

  

  CRingTextItem& operator=(const CRingTextItem& rhs);
  int operator==(const CRingTextItem& rhs) const;
  int operator!=(const CRingTextItem& rhs) const;

  // Public interface:
public:
  virtual std::vector<std::string>  getStrings() const;

  virtual void     setTimeOffset(uint32_t offset);
  virtual uint32_t getTimeOffset() const;
  virtual float    computeElapsedTime() const;
  virtual uint32_t getTimeDivisor() const;

  virtual void     setTimestamp(time_t stamp);
  virtual time_t   getTimestamp() const;
  virtual uint32_t getOriginalSourceId() const;
  
  // Virtual methods all ring overrides.

  virtual std::string typeName() const;
  virtual std::string toString() const;
  
  virtual void* getBodyHeader() const;
  virtual void setBodyHeader(
        uint64_t timestamp, uint32_t sid, uint32_t barrierType= 0
  );
  virtual size_t getBodySize()    const;
  virtual const void*  getBodyPointer() const;
  virtual void* getBodyPointer();
  virtual bool hasBodyHeader() const;
  uint64_t getEventTimestamp() const;
  uint32_t getSourceId() const;
  uint32_t getBarrierType() const;


  //private utilities:
private:
  size_t bodySize(std::vector<std::string> strings) const;
  bool   validType(uint16_t type) const;
  size_t itemSize(const std::vector<std::string>& strings) const;
  void* fillBody(
        void* pBody, unsigned offset, time_t stamp, unsigned divisor,
        unsigned osid, const std::vector<std::string>& strings
  );
};

}                                     // v12 namespace.
#endif
