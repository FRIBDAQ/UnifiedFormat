#ifndef __CRINGSTATECHANGEITEM_H
#define __CRINGSTATECHANGEITEM_H
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
/**
 * @file CRingStateChangeItem.h (v11).
 * @brief NSCLDAQ version 11 state change items.
 */

#include <CRingStateChangeItem.h>   // abstract - base class.
#include <string>

namespace ufmt {
  namespace v11 {
  /*!
    This class represents a state change item.
    State change items are items in the buffer that indicate a change in the state of
    data taking.  The set of state changes recognized by this class are:

    - BEGIN_RUN  - the run has become active after being stopped.
    - END_RUN    - The run has become inactive after being ended.
    - PAUSE_RUN  - The run has become inactive after being temporarily paused.
    - RESUME_RUN - The run has become active after being resumed while paused.

    This object is suitable for use by both producers (who must create state change
    items), and consumers (who must inspect the contents of such items after getting
    them from a ring).


  */
  class CRingStateChangeItem : public ::ufmt::CRingStateChangeItem
  {

    // construction and other canonicals
  public:
    CRingStateChangeItem(uint16_t reason = BEGIN_RUN);
    CRingStateChangeItem(uint16_t reason,
            uint32_t runNumber,
            uint32_t timeOffset,
            time_t   timestamp,
            std::string title) ;
    CRingStateChangeItem(uint64_t eventTimestamp, uint32_t sourceId, uint32_t barrierType,
                        uint16_t reason,
            uint32_t runNumber,
            uint32_t timeOffset,
            time_t   timestamp,
            std::string title,
                        uint32_t offsetDivisor = 1);
    virtual ~CRingStateChangeItem();

  private:  
    CRingStateChangeItem(const CRingItem& item) ;
    CRingStateChangeItem(const CRingStateChangeItem& rhs);
    

    CRingStateChangeItem& operator=(const CRingStateChangeItem& rhs);
    int operator==(const CRingStateChangeItem& rhs) const;
    int operator!=(const CRingStateChangeItem& rhs) const;
    
  public:
    // Accessors for elements of the item (selectors and mutators both).

    virtual void setRunNumber(uint32_t run);
    virtual uint32_t getRunNumber() const;

    virtual void setElapsedTime(uint32_t offset);
    virtual uint32_t getElapsedTime() const;
    virtual float    computeElapsedTime() const;

    virtual void setTitle(std::string title) ;
    virtual std::string getTitle() const;

    virtual void setTimestamp(time_t stamp);
    virtual time_t getTimestamp() const;
    
    virtual uint32_t getOriginalSourceId() const;
    
    // Body methods:
    
    virtual const void*  getBodyPointer() const;
    virtual void*        getBodyPointer();
    
    // Body header methods:
    
    virtual bool hasBodyHeader() const;
    virtual void* getBodyHeader() const;
    virtual void setBodyHeader(uint64_t timestamp, uint32_t sourceId,
                          uint32_t barrierType = 0);

    // Virtual method overrides.

    virtual std::string typeName() const;
    

    // Utitlity functions..

  private:
    bool isStateChange();
    
  };
  }
}
#endif
