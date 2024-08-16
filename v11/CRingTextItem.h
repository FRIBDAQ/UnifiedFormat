#ifndef V11_CRINGTEXTITEM_H
#define V11_CRINGTEXTITEM_H

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

#include <CRingTextItem.h>

#include <time.h>
#include <string>
#include <vector>

namespace ufmt {
  namespace v11 {
  /*!
    The text ring item provides a mechanism to put an item in/take an item out of 
    a ring buffer that consists of null terminated text strings.  
  */
  class CRingTextItem : public ::ufmt::CRingTextItem
  {
    // Private data:


  public:
    // Constructors and other canonicals:

    
    CRingTextItem(uint16_t type,
      std::vector<std::string> theStrings,
      uint32_t                 offsetTime,
      time_t                   timestamp,
      uint32_t                 divisor = 1) ;
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
    std::vector<std::string>  getStrings() const;

    void     setTimeOffset(uint32_t offset);
    uint32_t getTimeOffset() const;
    float    computeElapsedTime() const;
    uint32_t getTimeDivisor() const;

    virtual void     setTimestamp(time_t stamp);
    virtual time_t   getTimestamp() const;
    virtual uint32_t getOriginalSourceId() const;

    
    
    // Virtual methods all ring overrides.
    
    
    virtual void* getBodyPointer();
    virtual const void* getBodyPointer() const;
    virtual bool hasBodyHeader() const;
    virtual void* getBodyHeader() const;
    virtual void setBodyHeader(
          uint64_t timestamp, uint32_t sid, uint32_t barrierType= 0
    );


    virtual std::string typeName() const;
    virtual std::string toString() const;

    //private utilities:
  private:
    size_t sizeStrings(const std::vector<std::string>& strings) const;
    bool   validType() const;
    void   copyStrings(void* destination, const std::vector<std::string>& strings);
    
  };

  }                                     // v11
}
#endif
