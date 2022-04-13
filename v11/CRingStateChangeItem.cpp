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
#include "CRingItem.h"
#include "DataFormat.h"
#include <stdexcept>
#include <sstream>
#include <string.h>
#include <time.h>


using namespace std;
namespace v11 {
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
  ::CRingStateChangeItem(reason)
{

  v11::pStateChangeItem pItem =
    reinterpret_cast<v11::pStateChangeItem>(getItemPointer());
  pItem->s_header.s_size =
    sizeof(v11::RingItemHeader) + sizeof(uint32_t) +
    sizeof(v11::StateChangeItemBody);
  pItem->s_header.s_type = reason;
  if(!isStateChange()) {
    throw std::invalid_argument(
        "'reason' parameter is not a state change item type"
    );
  }
  // No body header:
  
  pItem->s_body.u_noBodyHeader.s_mbz = 0;
  


  pStateChangeItemBody pBody = &(pItem->s_body.u_noBodyHeader.s_body);
  
  pBody->s_runNumber    = 0;
  pBody->s_timeOffset   = 0;
  pBody->s_Timestamp = static_cast<uint32_t>(time(NULL));
  memset(pBody->s_title, 0, TITLE_MAXSIZE+1);
  pBody->s_offsetDivisor = 1;
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
  v11::CRingStateChangeItem(reason)

{
  

  // Everything should work just fine now:

  v11::pStateChangeItemBody pItem =
    reinterpret_cast<v11::pStateChangeItemBody>(getBodyPointer());

  pItem->s_runNumber = runNumber;
  pItem->s_timeOffset= timeOffset;
  pItem->s_Timestamp = timestamp;
  setTitle(title);		// takes care of the exception.
  pItem->s_offsetDivisor = 1;

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
    uint64_t eventTimestamp, uint32_t sourceId, uint32_t barrierType,
    uint16_t reason,
    uint32_t runNumber, uint32_t timeOffset, time_t   timestamp,
    std::string title, uint32_t offsetDivisor) :
    ::CRingStateChangeItem(reason)
    
{
    v11::pStateChangeItem pItem =
      reinterpret_cast<v11::pStateChangeItem>(getItemPointer());
    pItem->s_header.s_size =
      sizeof(v11::RingItemHeader) +
      sizeof(v11::BodyHeader)     +
      sizeof(v11::StateChangeItemBody);
    pItem->s_header.s_type = reason;
    
    if(!isStateChange()) {
      throw std::invalid_argument(
          "reason is not a valid state change item type"
      );
    }
    
    v11::pBodyHeader pBh = &(pItem->s_body.u_hasBodyHeader.s_bodyHeader);
    pBh->s_size = sizeof(v11::BodyHeader);
    pBh->s_timestamp = eventTimestamp;
    pBh->s_sourceId  = sourceId;
    pBh->s_barrier   = barrierType;
    
    
    // Everything should work just fine now:
  
    pStateChangeItemBody pBody =
        reinterpret_cast<pStateChangeItemBody>(pBh+1);

    pBody->s_runNumber = runNumber;
    pBody->s_timeOffset= timeOffset;
    pBody->s_Timestamp = timestamp;
    setTitle(title);		// takes care of the exception.
    pBody->s_offsetDivisor = offsetDivisor;    
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
    v11::pStateChangeItemBody pItem =
      reinterpret_cast<v11::pStateChangeItemBody>(getBodyPointer());

    pItem->s_runNumber = run;
}
/*!
  \return uint32_t
  \retval the item's current run number.
*/
uint32_t
CRingStateChangeItem::getRunNumber() const
{
    const v11::StateChangeItemBody* pItem =
        reinterpret_cast<const v11::StateChangeItemBody*>(getBodyPointer());

    return pItem->s_runNumber;
}

/*!
   Set the elapsed run time field for the item.
   \param offset - seconds into the run.
*/
void
CRingStateChangeItem::setElapsedTime(uint32_t offset)
{
    v11::pStateChangeItemBody pItem =
        reinterpret_cast<v11::pStateChangeItemBody>(getBodyPointer());

    pItem->s_timeOffset = offset;
}
/*!
  \return uint32_t
  \retval time offset field of the item.
*/
uint32_t
CRingStateChangeItem::getElapsedTime() const
{
    const v11::StateChangeItemBody* pItem =
        reinterpret_cast<const v11::StateChangeItemBody*>(getBodyPointer());

    return pItem->s_timeOffset;
}
/**
 * getElapsedTime
 *
 * @return float - Elapsed time taking into account the divisor.
 */
float
CRingStateChangeItem::computeElapsedTime() const
{
    const v11::StateChangeItemBody* pItem =
        reinterpret_cast<const v11::StateChangeItemBody*>(getBodyPointer());
    
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
      throw std::length_error(
          "Checking size of title against TITLE_MAXSIZE"
      );
    }
    v11::pStateChangeItemBody pItem =
        reinterpret_cast<v11::pStateChangeItemBody>(getBodyPointer());

      strncpy(pItem->s_title, title.c_str(), TITLE_MAXSIZE+1);
}

/*!
    \return std::string
    \retval the title string of the item.
*/
string
CRingStateChangeItem::getTitle() const
{
    const v11::StateChangeItemBody* pItem =
        reinterpret_cast<const v11::StateChangeItemBody*>(getBodyPointer());

    return string(pItem->s_title);
}
/*!
   Set the timestamp
   \param stamp  - new timestamp.
*/
void
CRingStateChangeItem::setTimestamp(time_t stamp)
{
    v11::pStateChangeItemBody pItem =
        reinterpret_cast<v11::pStateChangeItemBody>(getBodyPointer());

    pItem->s_Timestamp  = stamp;
}
/*!
    \return time_t
    \retval timestamp giving absolute time of the item.
*/
time_t
CRingStateChangeItem::getTimestamp() const
{
    const v11::StateChangeItemBody* pItem =
        reinterpret_cast<const v11::StateChangeItemBody*>(getBodyPointer());

    return pItem->s_Timestamp;
}
/**
 * getOriginalSourceId
 *     We have not field to store the original source id.
 *    - If there's a body header we just give the source id from that
 *    - If not return 0.
 * @return uint32_t
 */
uint32_t
CRingStateChangeItem::getOriginalSourceId() const
{
    const v11::BodyHeader* p =
      reinterpret_cast<const v11::BodyHeader*>(getBodyHeader());
    if (p) {
      return p->s_sourceId;  
    } else {
      return 0;
    }
}
/**
 * getBodyPointer
 *   @return const void* - pointer to the item's body.
 */
const void*
CRingStateChangeItem::getBodyPointer() const
{
  const v11::CRingItem* pThis =
      reinterpret_cast<const v11::CRingItem*>(this);
    return pThis->v11::CRingItem::getBodyPointer();
}
/**
 * getBodyPointer
 *   @return void* - pointer to the item's body.
 */
void*
CRingStateChangeItem::getBodyPointer()
{
    v11::CRingItem* pThis = reinterpret_cast<v11::CRingItem*>(this);
    return pThis->v11::CRingItem::getBodyPointer();
}

bool
CRingStateChangeItem::hasBodyHeader() const
{
  return getBodyHeader() != nullptr;
}
/**
 * getBodyHeader
 *   Delegates to v11::CRingItem with a fancy bit of artifice.
 * @return void*
 */
void*
CRingStateChangeItem::getBodyHeader() const
{
    const v11::CRingItem* pThis =
      reinterpret_cast<const v11::CRingItem*>(this);
    return pThis->v11::CRingItem::getBodyHeader();
}

/**
 * setBodyHeader
 *    delegates to v11::CRingItem
 * @param timestamp - event timestamp.
 * @param sourceId  - Event fragment source id.
 * @param barrierType - event fragment barrier type code.
 */
void
CRingStateChangeItem::setBodyHeader(
  uint64_t timestamp, uint32_t sourceId,
  uint32_t barrierType
)
{
  v11::CRingItem* pThis =
      reinterpret_cast<v11::CRingItem*>(this);
  pThis->v11::CRingItem::setBodyHeader(timestamp, sourceId, barrierType);
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
    default:
      break;
  }
  return "Unknown item type";
  
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
  
  string   title     = getTitle();
  time_t t = getTimestamp();
  string   timestamp = ctime(&t);

  out <<  timestamp << " : Run State change : " << typeName();
  out << " at " << computeElapsedTime() << " seconds into the run\n";
  const v11::CRingItem* pThis = reinterpret_cast<const v11::CRingItem*>(this);
  out << pThis->v11::CRingItem::bodyHeaderToString();
  out << "Title     : " << title << std::endl;
  out << "Run Number: " << run   << endl;


  return out.str();
}
    
///////////////////////////////////////////////////////////////////////////////
// Private utilities.


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
}                                // namespace v11