#ifndef __CRINGSCALERITEM_H
#define __CRINGSCALERITEM_H
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



#include <CRingScalerItem.h>        // abstract.


#include <vector>
#include <typeinfo>
#include <string>

namespace ufmt {
/*!
   This class derived from ::CRingScalerItem and represents a set of scalers that have been 
   formatted as a ring item.  Note we need to deal with two cases:
   incremental and non-incremental scalers which, in V10 are two different
   ring item types.
*/
  namespace v10 {
  class CRingScalerItem : public ::ufmt::CRingScalerItem
  {
    // private object data:

  private:
    bool m_isIncremental; 

  public:
    CRingScalerItem(size_t numScalers);
    CRingScalerItem(uint32_t startTime,
        uint32_t stopTime,
        time_t   timestamp,
        std::vector<uint32_t> scalers,
        bool                  isIncremental = true,
        uint32_t              sid = 0,
        uint32_t              timeOffsetDivisor = 1);
    
    virtual ~CRingScalerItem();
  private:
    CRingScalerItem(const CRingItem& rhs) ;
    CRingScalerItem(const CRingScalerItem& rhs);
    CRingScalerItem& operator=(const CRingScalerItem& rhs);
    int operator==(const CRingScalerItem& rhs) const;
    int operator!=(const CRingScalerItem& rhs) const;
    
  public:
    // Accessor member functions.

    virtual void     setStartTime(uint32_t startTime);
    virtual uint32_t getStartTime() const;
    virtual float    computeStartTime() const;

    virtual void     setEndTime(uint32_t endTime);
    virtual uint32_t getEndTime() const;
    virtual float    computeEndTime() const;

    virtual uint32_t getTimeDivisor() const;


    virtual void     setTimestamp(time_t stamp);
    virtual time_t   getTimestamp() const;
    virtual bool isIncremental() const;
    
    virtual void     setScaler(uint32_t channel, uint32_t value);
    virtual uint32_t getScaler(uint32_t channel) const;
    virtual std::vector<uint32_t> getScalers() const;

    virtual uint32_t getScalerCount() const;
    virtual uint32_t getOriginalSourceId() const;

    // Virtual methods overriddent:

    virtual void* getBodyHeader() const;
    virtual void setBodyHeader(
        uint64_t timestamp, uint32_t sourceId,
        uint32_t barrierType = 0
    );
    virtual std::string typeName() const;
    

    // utility.

  private:
  
    void init(size_t n);
    void throwIfInvalidChannel(uint32_t channel, 
            const char* message) const ;
  };
    
  }
}
#endif
