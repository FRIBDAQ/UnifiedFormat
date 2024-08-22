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



#include "CRingStateChangeItem.h"
#include "DataFormat.h"
#include "CRingItem.h"
#include <sstream>
#include <string.h>
#include <stdexcept>


using namespace std;

namespace ufmt {
  namespace v12 {
  ////////////////////////////////////////////////////////////////////////////
  //
  // Constructors and canonicals.
  //

  /*!
    Construct a bare bone state change buffer.  The run number, time offset
    are both set to zero.  The timestamp is set to 'now'.
    The title is set to an emtpy string.
    \param reason - Reason for the state change.  This defaults to BEGIN_RUN.
  */
  CRingStateChangeItem::CRingStateChangeItem(uint16_t reason) :
    ::ufmt::CRingStateChangeItem(reason)
  {
    if (!isStateChange(reason)) {
        throw std::logic_error("Invalid ring item type for state change");
    }

    v12::pStateChangeItem pItem =
      reinterpret_cast<v12::pStateChangeItem>(getItemPointer());
    pItem->s_header.s_type = reason;
    pItem->s_body.u_noBodyHeader.s_empty = sizeof(uint32_t);
    
    v12::pStateChangeItemBody pBody = &(pItem->s_body.u_noBodyHeader.s_body);
    fillStateChangeBody(pBody, 0,  0, 1, time(nullptr), "", 0);
    
    setBodyCursor(pBody+1);
    updateSize();
    
  }
  /*!
    Fully specified construction the initial values of the various
    fields are specified by the constructor parameters. 

    \param reason     - Why the state change buffer is being emitted (the item type).
    \param runNumber  - Number of the run that is undegoing transitino.
    \param timeOffset - Number of seconds into the run at which this is being emitted.
    \param timestamp  - Absolute time to be recorded in the buffer.. tyically
                        this should be time(NULL).
    \param title      - Title string.  The length of this string must be at most
                        TITLE_MAXSIZE.

    \throw CRangeError - If the title string can't fit in s_title.
  */
  CRingStateChangeItem::CRingStateChangeItem(uint16_t reason,
              uint32_t runNumber,
              uint32_t timeOffset,
              time_t   timestamp,
              std::string title) :
    ::ufmt::CRingStateChangeItem(reason)

  {
    if (!isStateChange(reason)) {
        throw std::logic_error("Invalid ring item type for state change");
    }
    
    v12::pStateChangeItem pItem =
      reinterpret_cast<v12::pStateChangeItem>(getItemPointer());
    pItem->s_header.s_type = reason;
    pItem->s_body.u_noBodyHeader.s_empty = sizeof(uint32_t);
    
    v12::pStateChangeItemBody pBody = &(pItem->s_body.u_noBodyHeader.s_body);
    fillStateChangeBody(pBody, runNumber, timeOffset, 1, timestamp, title.c_str(), 0);
    
    setBodyCursor(pBody+1);
    updateSize();
    
  }
  /**
   * constructor - for timetamped item.
   *
   * @param eventTimestamp  - Event timestamp
   * @param sourceId   - Source id of the event.
   * @param barrierType - Type of barrier if barrier.
     \param reason     - Why the state change buffer is being emitted (the item type).
    \param runNumber  - Number of the run that is undegoing transitino.
    \param timeOffset - Number of seconds into the run at which this is being emitted.
    \param timestamp  - Absolute time to be recorded in the buffer.. tyically
                        this should be time(NULL).
    \param title      - Title string.  The length of this string must be at most
                        TITLE_MAXSIZE.
    @param offsetDivisor - What timeOffset needs to be divided by to get seconds.

    \throw CRangeError - If the title string can't fit in s_title.
  */
  CRingStateChangeItem::CRingStateChangeItem(
      uint64_t eventTimestamp, uint32_t sourceId, uint32_t barrierType, uint16_t reason,
      uint32_t runNumber, uint32_t timeOffset, time_t   timestamp,
      std::string title, uint32_t offsetDivisor) :
    CRingStateChangeItem(reason)
    {
      
        if (!isStateChange(reason)) {
            throw std::logic_error("Invalid ring item type for state change");
        }
        
        v12::pStateChangeItem pItem =
          reinterpret_cast<v12::pStateChangeItem>(getItemPointer());
        pItem->s_header.s_type = reason;
        
        v12::pBodyHeader pBh = &(pItem->s_body.u_hasBodyHeader.s_bodyHeader);
        pBh->s_size = sizeof(v12::BodyHeader);
        pBh->s_timestamp = eventTimestamp;
        pBh->s_sourceId   = sourceId;
        pBh->s_barrier    = barrierType;
        
        v12::pStateChangeItemBody pBody = &(pItem->s_body.u_hasBodyHeader.s_body);
        fillStateChangeBody(
          pBody, runNumber, timeOffset, offsetDivisor, timestamp, title.c_str(),
          sourceId
        );
        
        setBodyCursor(pBody+1);
        updateSize();
  }
  /*!
    Destruction just chains to the base class.

  */
  CRingStateChangeItem::~CRingStateChangeItem()
  {
  }
  ///////////////////////////////////////////////////////////////////////////////
  //
  // Accessor methods.

  /*!
    Set the run number for the item.
    \param run - new run number.
  */
  void
  CRingStateChangeItem::setRunNumber(uint32_t run)
  {
      v12::pStateChangeItemBody pItem =
        reinterpret_cast<v12::pStateChangeItemBody>(getBodyPointer());

      pItem->s_runNumber = run;
  }
  /*!
    \return uint32_t
    \retval the item's current run number.
  */
  uint32_t
  CRingStateChangeItem::getRunNumber() const
  {
      const v12::StateChangeItemBody* pItem =
          reinterpret_cast<const v12::StateChangeItemBody*>(getBodyPointer());

      return pItem->s_runNumber;
  }

  /*!
    Set the elapsed run time field for the item.
    \param offset - seconds into the run.
  */
  void
  CRingStateChangeItem::setElapsedTime(uint32_t offset)
  {
      v12::pStateChangeItemBody pItem =
          reinterpret_cast<v12::pStateChangeItemBody>(getBodyPointer());

      pItem->s_timeOffset = offset;
  }
  /*!
    \return uint32_t
    \retval time offset field of the item.
  */
  uint32_t
  CRingStateChangeItem::getElapsedTime() const
  {
      const v12::StateChangeItemBody* pItem =
          reinterpret_cast<const v12::StateChangeItemBody*>(getBodyPointer());

      return pItem->s_timeOffset;
  }
  /**
   * getTimeDivisor
   *    Return the divisor used for timing (conversion from ticks to seconds)
   * @return uint32_t
   */
  uint32_t
  CRingStateChangeItem::getTimeDivisor() const
  {
    const v12::StateChangeItemBody* pItem =
          reinterpret_cast<const v12::StateChangeItemBody*>(getBodyPointer());
    return pItem->s_offsetDivisor;
  }
  /**
   * getElapsedTime
   *
   * @return float - Elapsed time taking into account the divisor.
   */
  float
  CRingStateChangeItem::computeElapsedTime() const
  {
      const v12::StateChangeItemBody* pItem =
          reinterpret_cast<const v12::StateChangeItemBody*>(getBodyPointer());
      
      float offset = pItem->s_timeOffset;
      float divisor = pItem->s_offsetDivisor;
      return offset/divisor;
  }
  /*!
    Set the title string.
    \param title - new title string.
    \throw CRangeError - if the title string is too long to fit.
  */
  void
  CRingStateChangeItem::setTitle(string title)
  {
      // Ensure the title is small enough.
    
      if(title.size() > TITLE_MAXSIZE) {
        throw std::length_error("Checking size of title against TITLE_MAXSIZE");
      }
      v12::pStateChangeItemBody pItem =
          reinterpret_cast<v12::pStateChangeItemBody>(getBodyPointer());

        strcpy(pItem->s_title, title.c_str());
  }

  /*!
      \return std::string
      \retval the title string of the item.
  */
  string
  CRingStateChangeItem::getTitle() const
  {
      const v12::StateChangeItemBody* pItem =
          reinterpret_cast<const v12::StateChangeItemBody*>(getBodyPointer());

      return string(pItem->s_title);
  }
  /*!
    Set the timestamp
    \param stamp  - new timestamp.
  */
  void
  CRingStateChangeItem::setTimestamp(time_t stamp)
  {
      v12::pStateChangeItemBody pItem =
          reinterpret_cast<v12::pStateChangeItemBody>(getBodyPointer());

      pItem->s_Timestamp  = stamp;
  }
  /*!
      \return time_t
      \retval timestamp giving absolute time of the item.
  */
  time_t
  CRingStateChangeItem::getTimestamp() const
  {
      const v12::StateChangeItemBody* pItem =
          reinterpret_cast<const v12::StateChangeItemBody*>(getBodyPointer());

      return pItem->s_Timestamp;
  }
  /**
   * getOriginalSourceId
   *    Return the source id with wich the item was originally constructed.
   *    The body header can be re-written by glom and therefore the original source
   *    id is squirreled away in the body itself.  This returns that
   *    squirreled value.
   *
   * @return uint32_t
   */
  uint32_t
  CRingStateChangeItem::getOriginalSourceId() const
  {
    const v12::StateChangeItemBody* pItem =
          reinterpret_cast<const v12::StateChangeItemBody*>(getBodyPointer());
    return pItem->s_originalSid;
  }

  ////////////////////////////////////////////////////////////////////////////////
  //
  // Virtual method overrides:

  /**
   * typeName
   *
   *  Stringify the type of the item.
   *
   * @return std::string - the item type
   */
  std::string
  CRingStateChangeItem::typeName() const
  {
    switch (type()) {
    case BEGIN_RUN:
      return "Begin Run";
    case END_RUN:
      return "End Run";
    case PAUSE_RUN:
      return "Pause Run";
    case RESUME_RUN:
      return "Resume Run";
    }
    return "*Invalid State Change type*";
  }
  
  /**
   * getBodySize
   *    Delegated to v12::CRingItem which works since the underlying stored
   *    data are v12::CRingItem compatible.
   *    @return size_t
   */
  size_t
  CRingStateChangeItem::getBodySize()    const
  {
      const v12::CRingItem* pThis = reinterpret_cast<const v12::CRingItem*>(this);
      return pThis->v12::CRingItem::getBodySize();
  }

  /**
   * getBodyPointer
   *   @return [const] void* - pointer to the item's body
   */
  const void*
  CRingStateChangeItem::getBodyPointer() const
  {
    const v12::CRingItem* pThis = reinterpret_cast<const v12::CRingItem*>(this);
    return pThis->v12::CRingItem::getBodyPointer();
  }

  void*
  CRingStateChangeItem::getBodyPointer()
  {
    v12::CRingItem* pThis = reinterpret_cast<v12::CRingItem*>(this);
    return pThis->v12::CRingItem::getBodyPointer();
  }
  /**
   * hasBodyHeader
   *    @return bool - true if the item has a body header:
   */
  bool
  CRingStateChangeItem::hasBodyHeader() const
  {
      const v12::CRingItem* pThis = reinterpret_cast<const v12::CRingItem*>(this);
      return pThis->v12::CRingItem::hasBodyHeader();
  }
  /**
   * getBodyHeader
   *    Return a pointer to the item's body header or nullptr if there isn't
   *    one.
   * @return void*
   */
  void*
  CRingStateChangeItem::getBodyHeader() const
  {
    const v12::CRingItem* pThis = reinterpret_cast<const v12::CRingItem*>(this);
    return pThis->v12::CRingItem::getBodyHeader();
  }
  /**
   * setBodyHeader
   *   Set the contents of the item's body header.   If it does not yet have
   *   one, one is created by sliding the data down as needed:
   * @param timestamp - Event fragment timestamp.
   * @param sourceId  - Event fragment source id.
   * @param barrierType - Event fragment barrier type.
   */
  void
  CRingStateChangeItem::setBodyHeader(
      uint64_t timestamp, uint32_t sourceId, uint32_t barrierType
  )
  {
    v12::CRingItem* pThis = reinterpret_cast<v12::CRingItem*>(this);
    pThis->v12::CRingItem::setBodyHeader(timestamp, sourceId, barrierType);
  }
  /**
   * getEventTimestamp
   *   @return uint64_t - timestamp from body header.
   *   @throw std::logic_error if there's no body header.
   */
  uint64_t
  CRingStateChangeItem::getEventTimestamp() const
  {
    const v12::CRingItem* pThis = reinterpret_cast<const v12::CRingItem*>(this);
    return pThis->v12::CRingItem::getEventTimestamp();
  }
  /**
   * getSourceId
   *
   *    @return uint32_t - the source id from the body header
   *    @throw std::logic_error if there's no body header.
   */
  uint32_t
  CRingStateChangeItem::getSourceId() const
  {
    const v12::CRingItem* pThis = reinterpret_cast<const v12::CRingItem*>(this);
    return pThis->v12::CRingItem::getSourceId();
  }
  /**
   * getBarrierType
   *    @return uint32_t the barrier id from the body header.
   *    @throw std::logic_error if there's no body header.
   */
  uint32_t
  CRingStateChangeItem::getBarrierType() const
  {
    const v12::CRingItem* pThis = reinterpret_cast<const v12::CRingItem*>(this);
    return pThis->v12::CRingItem::getBarrierType();
  }


  ///////////////////////////////////////////////////////////////////////////////
  // Private utilities.



  /*
  * Evaluate whether or the type of the current ring item is 
  * a state change or not.
  */
  bool
  CRingStateChangeItem::isStateChange(unsigned t)
  {

    return (
      (t == v12::BEGIN_RUN )              ||
      (t == v12::END_RUN)                 ||
      (t == v12::PAUSE_RUN)               ||
      (t == v12::RESUME_RUN));
  }
  /**
   * fillStateChangeBody
   *    Utility to fill the state change body with stuff.
   * @param p - pointer to the item body.
   * @param run   - run number.
   * @paraqm offset - time offset.
   * @param divisor - time offset divisor.
   * @param clocktime - wall time (time_t).
   * @param title   - title string.
   * @param osid    - original source id
   */
  void
  CRingStateChangeItem::fillStateChangeBody(
    void* p, unsigned run, unsigned offset, unsigned divisor,
    time_t clocktime, const char* title, unsigned osid
  )
  {
    v12::StateChangeItemBody*  pBody = reinterpret_cast<v12::StateChangeItemBody*>(p);
    pBody->s_runNumber = run;
    pBody->s_timeOffset = offset;
    pBody->s_Timestamp  = clocktime;
    pBody->s_offsetDivisor = divisor;
    pBody->s_originalSid = osid;
    memset(pBody->s_title, 0, TITLE_MAXSIZE);
    strncpy(pBody->s_title, title, TITLE_MAXSIZE-1);
  }
  }                                   // v12 namespace.
}