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
#include "CRingPhysicsEventCountItem.h"
#include <sstream>
#include <time.h>
#include <stdexcept>

using namespace std;

///////////////////////////////////////////////////////////////////////////////////
//
// Constructors and other canonicals.

/*!
   Default constructor has a timestamp of now, a time offset and
   event count of 0.
*/
CRingPhysicsEventCountItem::CRingPhysicsEventCountItem() :
  CRingItem(PHYSICS_EVENT_COUNT)
{
  
  void* p = fillEventCountBody( 0, 1, time(nullptr), 0, 0); 
  
  setBodyCursor(p);
  updateSize();
}
/*!
   Creates an event count item timestamped to now with a specified
   number of events and run offset.

   \param count       - number of events.
   \param timeOffset  - Seconds into the active run time at which this item
                        was produced.
*/
CRingPhysicsEventCountItem::CRingPhysicsEventCountItem(uint64_t count,
						       uint32_t timeOffset, unsigned divisor) : 
  CRingItem(PHYSICS_EVENT_COUNT)
{

  void* p =
    fillEventCountBody(
         timeOffset, divisor, time(nullptr), count, 0
    );
  
  setBodyCursor(p);
  updateSize();
}
/*!
  Creates an event count item that is fully specified.
   \param count       - number of events.
   \param timeOffset  - Seconds into the active run time at which this item
   \param stamp       - Timestamp at which the event was produced.
   @param sid         - Sourceid.
   @param divisor     - Timestamp divisor defaults to 1
*/
CRingPhysicsEventCountItem::CRingPhysicsEventCountItem(uint64_t count,
						       uint32_t timeOffset,
						       time_t   stamp, uint32_t sid,
                   unsigned divisor) :
  CRingItem(PHYSICS_EVENT_COUNT)
{
  
  void* p =
    fillEventCountBody(timeOffset, divisor, stamp, count, sid);
  setBodyCursor(p);
  updateSize();
}

/*!
  Destructor chaining:
*/
CRingPhysicsEventCountItem::~CRingPhysicsEventCountItem()
{}

//////////////////////////////////////////////////////////////////////////////////
//
//  object interface:
//

/*!
    \return uint32_t
    \retval The current value of the time offset.
*/
uint32_t
CRingPhysicsEventCountItem::getTimeOffset() const
{
  const PhysicsEventCountItemBody* pItem =
    reinterpret_cast<const PhysicsEventCountItemBody*>(getBodyPointer());

  return pItem->s_timeOffset;
}
/**
 * computeTimeOffset
 *
 * Get the floating point seconds into the run using the time offset and
 * divisor fields:
 *
 * @return float
 */
float
CRingPhysicsEventCountItem::computeElapsedTime() const
{
    const PhysicsEventCountItemBody* pItem =
    reinterpret_cast<const PhysicsEventCountItemBody*>(getBodyPointer());
    
    float timeOffset = pItem->s_timeOffset;
    float divisor    = pItem->s_offsetDivisor;
    
    return timeOffset/divisor;
}
/**
 * getTimeDivisor
 *   Return the divisor used to compute the offset.
 *
 * @return uint32_t
 */
uint32_t
CRingPhysicsEventCountItem::getTimeDivisor() const
{
  const PhysicsEventCountItemBody* pItem =
    reinterpret_cast<const PhysicsEventCountItemBody*>(getBodyPointer());
  return pItem->s_offsetDivisor;
}
/*!
   set the time offset.
   \param offset - new value of time offset.
*/
void
CRingPhysicsEventCountItem::setTimeOffset(uint32_t offset)
{
  pPhysicsEventCountItemBody pItem =
    reinterpret_cast<pPhysicsEventCountItemBody>(getBodyPointer());

  pItem->s_timeOffset = offset;
}

/*!
  \return time_t
  \retval current value of the timestamp.
*/
time_t
CRingPhysicsEventCountItem::getTimestamp() const
{
  const PhysicsEventCountItemBody* pItem =
    reinterpret_cast<const PhysicsEventCountItemBody*>(getBodyPointer());

  return pItem->s_timestamp;
}
/*!
  \param stamp - New value for the timestamp.
*/
void
CRingPhysicsEventCountItem::setTimestamp(time_t stamp)
{
  pPhysicsEventCountItemBody pItem =
    reinterpret_cast<pPhysicsEventCountItemBody>(getBodyPointer());

  pItem->s_timestamp = stamp;
}

/*!
  \return uint64_t
  \retval number of events submitted  to the ring this run.
*/
uint64_t
CRingPhysicsEventCountItem::getEventCount() const
{
  const PhysicsEventCountItemBody* pItem =
    reinterpret_cast<const PhysicsEventCountItemBody*>(getBodyPointer());

  return pItem->s_eventCount;
}
/*!
  \param count - new value for number of events submitted.
*/
void
CRingPhysicsEventCountItem::setEventCount(uint64_t count)
{
  pPhysicsEventCountItemBody pItem =
    reinterpret_cast<pPhysicsEventCountItemBody>(getBodyPointer());

  pItem->s_eventCount = count;
}
/**
 * getOriginalSourceId
 *   @return uint32_t - the item's o96riginal source id the body header
 *           can get re-written by Glom.  This preserves the id of the
 *           original data source.
 */
uint32_t
CRingPhysicsEventCountItem::getOriginalSourceId() const
{
  const PhysicsEventCountItemBody* pItem =
    reinterpret_cast<const PhysicsEventCountItemBody*>(getBodyPointer());
  return pItem->s_originalSid;
}
//////////////////////////////////////////////////////////
//
// Virtual method overrides.

/**
 * setBodyHeader
 *    @throws std::Logic_error as we don't have a body header in
 *            abstract.
 * @param timestamp - timestamp for event.
 * @param sourceid  - Source id of event.
 * @param barrierType - Barrier event type.
 */
void
CRingPhysicsEventCountItem::setBodyHeader(
    uint64_t timestamp, uint32_t sourceId, uint32_t barrierType 
)
{
  
}
/**
 * getBodyHeader
 
 */
void*
CRingPhysicsEventCountItem::getBodyHeader() const
{
  return nullptr;
}

/**
 * typeName
 * 
 *   Provides the type name for this ring item.
 *
 * @return std::string type name.
 */
std::string
CRingPhysicsEventCountItem::typeName() const
{
  return std::string("Trigger count");
}
/**
 * toString
 *
 * Returns a stringified version of the item.
 *
 * @return item - the string.
 */
std::string
CRingPhysicsEventCountItem::toString() const
{
  std::ostringstream out;
  time_t tstamp = getTimestamp();
  string   time   = ctime(&tstamp);
  time.erase(time.size()-1);          // Get rid of trailing \n
  uint32_t offset = getTimeOffset();
  uint32_t divisor= getTimeDivisor();
  uint64_t events = getEventCount();
  uint32_t sid    = getOriginalSourceId();
  
  float fOffset = static_cast<float>(offset)/static_cast<float>(divisor);
  
  out << time << " : " << events << " Triggers accepted as of " 
      << fOffset << " seconds into the run\n";
  out << " Average accepted trigger rate: " 
      <<  static_cast<double>(events)/fOffset
      << " events/second originally from sid: " << sid << std::endl;

  return out.str();
}

/////////////////////////////////////////////////////////////////////////////////
//
// Private utilities.
//

/**
 * fillEventCountBody
 *     - get a pointer to our body.
 *     - Fill it with the event count data.
 * @param tOffset - offset into the run.
 * @param divisor - Offset divisor to support fractional secionds.
 * @param tSTamp - actually a time_t for the clock time.
 * @param triggers - Number of triggers.
 * @param sid    - Source id.
 * @return void* - points just past the body.
 */
void*
CRingPhysicsEventCountItem::fillEventCountBody(
      uint32_t tOffset, uint32_t divisor, uint32_t tStamp,
      uint64_t triggers, uint32_t sid
)
{
  pPhysicsEventCountItemBody   pBody =
    reinterpret_cast<pPhysicsEventCountItemBody>(getBodyPointer());
  pBody->s_timeOffset = tOffset;
  pBody->s_offsetDivisor = divisor;
  pBody->s_timestamp = tStamp;
  pBody->s_originalSid = sid;
  pBody->s_eventCount = triggers;
  
  return pBody+1;
  
}