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

#include <sstream>
#include <string.h>
#include <stdexcept>
#include "DataFormat.h"
#include <time.h>

using namespace std;
namespace ufmt {
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
    CRingItem(reason, sizeof(StateChangeItem))
  {
    init();

    // Fill in the body:

    fillStateChangeBody(0,  0, 1, time(nullptr), "", 0);
    if (!isStateChange()) {
        throw std::logic_error("Invalid ring item type for state change");
    }
    
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

    \throw std::range_error
  */
  CRingStateChangeItem::CRingStateChangeItem(uint16_t reason,
              uint32_t runNumber,
              uint32_t timeOffset,
              time_t   timestamp,
              std::string title) :
    CRingItem(reason, sizeof(StateChangeItem))

  {
    init();

    fillStateChangeBody(
        runNumber, timeOffset, 1, timestamp, title.c_str(), 0
    );
    if (!isStateChange()) {
      throw std::logic_error("Invalid ring item type for state change");
    }
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
      pStateChangeItemBody pItem =
        reinterpret_cast<pStateChangeItemBody>(getBodyPointer());

      pItem->s_runNumber = run;
  }
  /*!
    \return uint32_t
    \retval the item's current run number.
  */
  uint32_t
  CRingStateChangeItem::getRunNumber() const
  {
      const StateChangeItemBody* pItem =
          reinterpret_cast<const StateChangeItemBody*>(getBodyPointer());

      return pItem->s_runNumber;
  }

  /*!
    Set the elapsed run time field for the item.
    \param offset - seconds into the run.
  */
  void
  CRingStateChangeItem::setElapsedTime(uint32_t offset)
  {
      pStateChangeItemBody pItem =
          reinterpret_cast<pStateChangeItemBody>(getBodyPointer());

      pItem->s_timeOffset = offset;
  }
  /*!
    \return uint32_t
    \retval time offset field of the item.
  */
  uint32_t
  CRingStateChangeItem::getElapsedTime() const
  {
      const StateChangeItemBody* pItem =
          reinterpret_cast<const StateChangeItemBody*>(getBodyPointer());

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
    const StateChangeItemBody* pItem =
          reinterpret_cast<const StateChangeItemBody*>(getBodyPointer());
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
      const StateChangeItemBody* pItem =
          reinterpret_cast<const StateChangeItemBody*>(getBodyPointer());
      
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
        throw std::range_error("Title is to long for state change body");
        
      }
      pStateChangeItemBody pItem =
          reinterpret_cast<pStateChangeItemBody>(getBodyPointer());

        strcpy(pItem->s_title, title.c_str());
  }

  /*!
      \return std::string
      \retval the title string of the item.
  */
  string
  CRingStateChangeItem::getTitle() const
  {
      const StateChangeItemBody* pItem =
          reinterpret_cast<const StateChangeItemBody*>(getBodyPointer());

      return string(pItem->s_title);
  }
  /*!
    Set the timestamp
    \param stamp  - new timestamp.
  */
  void
  CRingStateChangeItem::setTimestamp(time_t stamp)
  {
      pStateChangeItemBody pItem =
          reinterpret_cast<pStateChangeItemBody>(getBodyPointer());

      pItem->s_Timestamp  = stamp;
  }
  /*!
      \return time_t
      \retval timestamp giving absolute time of the item.
  */
  time_t
  CRingStateChangeItem::getTimestamp() const
  {
      const StateChangeItemBody* pItem =
          reinterpret_cast<const StateChangeItemBody*>(getBodyPointer());

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
    const StateChangeItemBody* pItem =
          reinterpret_cast<const StateChangeItemBody*>(getBodyPointer());
    return pItem->s_originalSid;
  }

  ////////////////////////////////////////////////////////////////////////////////
  //
  // Virtual method overrides:

  /**
   * getBodyHeader
   *    @throws std::logic_error abstract items don't have body headers.
   */
  void*
  CRingStateChangeItem::getBodyHeader() const
  {
    return nullptr;
  }
  /**
   * setBodyHeader
   *    @throws std::logic_error abstract items have no body header.
   */
  void
  CRingStateChangeItem::setBodyHeader(
    uint64_t timestamp, uint32_t sourceId,
    uint32_t barrierType 
  )
  {
    
  }

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
   * toString
   *
   * Returns a string that is the ascified body of the item.
   *
   * @return std::string - ascified version of the item.
   */
  std::string
  CRingStateChangeItem::toString() const
  {
    std::ostringstream out;		//  Build up via outputting to this psuedo stream.

    uint32_t run       = getRunNumber();
    uint32_t sid       = getOriginalSourceId();
    string   title     = getTitle();
    time_t   t         = getTimestamp();
    string   timestamp = ctime(&t);

    out <<  timestamp << " : Run State change: " << typeName();
    out << " originally from source id: " <<  sid;
    out << " at " << computeElapsedTime() << " seconds into the run\n";
    out << "Title     : " << title << std::endl;
    out << "Run Number: " << run   << endl;


    return out.str();
  }
      
  ///////////////////////////////////////////////////////////////////////////////
  // Private utilities.

  /* 
  *  Initialize member data construction time.
  */
  void 
  CRingStateChangeItem::init()
  {

    uint8_t* pCursor = reinterpret_cast<uint8_t*>(getBodyPointer());
    pCursor         += sizeof(StateChangeItemBody);
    setBodyCursor(pCursor);
    updateSize();

  }

  /*
  * Evaluate wheter or the type of the current ring item is 
  * a state change or not.
  */
  bool
  CRingStateChangeItem::isStateChange()
  {
    int t = type();
    return (
      (t == BEGIN_RUN )              ||
      (t == END_RUN)                 ||
      (t == PAUSE_RUN)               ||
      (t == RESUME_RUN));
  }
  /**
   * fillStateChangeBody
   *
   *  @param run- run number.
   *  @param offset - time offset into run.
   *  @param divisor - Divisor to turn offset into real seconds.
   *  @param timestamp - clock date/time at which item was created.
   *  @param title    - Run title
   *  @param sid      - Original source id.
   */
  void
  CRingStateChangeItem::fillStateChangeBody(
    uint32_t run, uint32_t offset, uint32_t divisor, time_t timestamp,
    const char* title, uint32_t sid
  )
  {
      pRingItemHeader pH = reinterpret_cast<pRingItemHeader>(getItemPointer());
      pStateChangeItemBody pB = reinterpret_cast<pStateChangeItemBody>(pH+1);
      
      pB->s_runNumber  = run;
      pB->s_timeOffset = offset;
      pB->s_Timestamp  = timestamp;
      pB->s_offsetDivisor = divisor;
      pB->s_originalSid = sid;
      
      memset(pB->s_title, 0, TITLE_MAXSIZE+1);
      strncpy(pB->s_title, title, TITLE_MAXSIZE);
  }
}