#ifndef CRINGTEXTITEM_H
#define CRINGTEXTITEM_H

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
#include <stdint.h>
#include <time.h>
#include <string>
#include <vector>
#include <typeinfo>

namespace ufmt {

  /*!
    The text ring item provides a mechanism to put an item in/take an item out of 
    a ring buffer that consists of null terminated text strings.  
  */
  class CRingTextItem : public CRingItem
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
    

    virtual ~CRingTextItem();

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

    virtual void* getBodyHeader() const;
    virtual void setBodyHeader(
          uint64_t timestamp, uint32_t sid, uint32_t barrierType= 0
    );
    virtual std::string typeName() const;
    virtual std::string headerToString() const;
    virtual std::string bodyToString() const;
    

    //private utilities:
  private:
    size_t bodySize(std::vector<std::string> strings) const;
    bool   validType() const;
    
    std::vector<const char*> makeStringPointers(const std::vector<std::string>& strings);
    void* fillTextItemBody(
      uint32_t offset, uint32_t divisor, time_t stamp, uint32_t nStrings,
      const char** ppStrings, int sid
    );
  };

}
#endif
