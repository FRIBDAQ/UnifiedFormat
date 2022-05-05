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
#include "DataFormat.h"
#include "CRingItem.h"
#include <sstream>
#include <time.h>


using namespace std;

namespace v12 {

///////////////////////////////////////////////////////////////////////////////////
//
// Constructors and other canonicals.

/*!
   Default constructor has a timestamp of now, a time offset and
   event count of 0.
*/
CRingPhysicsEventCountItem::CRingPhysicsEventCountItem() :
  ::CRingPhysicsEventCountItem()
{
  v12::pPhysicsEventCountItem pItem =
    reinterpret_cast<v12::pPhysicsEventCountItem>(getItemPointer());
  
  pItem->s_header.s_type = PHYSICS_EVENT_COUNT;
  pItem->s_body.u_noBodyHeader.s_empty = sizeof(uint32_t);
  
  v12::pPhysicsEventCountItemBody  pBody = &(pItem->s_body.u_noBodyHeader.s_body);
  pBody->s_timeOffset =0;
  pBody->s_offsetDivisor = 1;
  pBody->s_timestamp = time(nullptr);
  pBody->s_originalSid = 0;
  pBody->s_eventCount = 0;
 
  setBodyCursor(pBody+1);
  updateSize();                // Sets pItem->s_header.s_size.
}
/*!
   Creates an event count item timestamped to now with a specified
   number of events and run offset.

   \param count       - number of events.
   \param timeOffset  - Seconds into the active run time at which this item
                        was produced.
    @param divisor    - Offset divisor (defaults to 1).
*/
CRingPhysicsEventCountItem::CRingPhysicsEventCountItem(uint64_t count,
						       uint32_t timeOffset, unsigned divisor) :
  ::CRingPhysicsEventCountItem()
{

  v12::pPhysicsEventCountItem pItem =
    reinterpret_cast<v12::pPhysicsEventCountItem>(getItemPointer());
  
  pItem->s_header.s_type = PHYSICS_EVENT_COUNT;
  pItem->s_body.u_noBodyHeader.s_empty = sizeof(uint32_t);
  
  v12::pPhysicsEventCountItemBody  pBody = &(pItem->s_body.u_noBodyHeader.s_body);
  pBody->s_timeOffset = timeOffset;
  pBody->s_offsetDivisor = divisor;
  pBody->s_timestamp = time(nullptr);
  pBody->s_originalSid = 0;
  pBody->s_eventCount = 0;
 
  setBodyCursor(pBody+1);
  updateSize();                // Sets pItem->s_header.s_size.
  
}
/*!
  Creates an event count item that is fully specified.
   \param count       - number of events.
   \param timeOffset  - Seconds into the active run time at which this item
   \param stamp       - Timestamp at which the event was produced.
   @param sid         - original source id to drop into the body.
   @param divisor     - Time offset divisor.
   @note - the resulting item has  a body header with the sid filled in from
           sid and all other fields 0. One can imagine setBodyHeader being called
           later to fill in the remaining fields.
           
*/
CRingPhysicsEventCountItem::CRingPhysicsEventCountItem(uint64_t count,
						       uint32_t timeOffset,
						       time_t   stamp, uint32_t sid, unsigned divisor) :
  ::CRingPhysicsEventCountItem()
{
  v12::pPhysicsEventCountItem pItem =
    reinterpret_cast<v12::pPhysicsEventCountItem>(getItemPointer());
  
  pItem->s_header.s_type = PHYSICS_EVENT_COUNT;
  pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_size = sizeof(v12::BodyHeader);
  pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_sourceId = sid;
  pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_timestamp = 0;
  pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_barrier = 0;
  
  v12::pPhysicsEventCountItemBody  pBody = &(pItem->s_body.u_hasBodyHeader.s_body);
  pBody->s_timeOffset = timeOffset;
  pBody->s_offsetDivisor = divisor;
  pBody->s_timestamp = stamp;
  pBody->s_originalSid = sid;
  pBody->s_eventCount = count;
 
  setBodyCursor(pBody+1);
  updateSize();                
  
}

/**
 * construtor
 *
 * Construct an event count item that includes a body header.
 *
 * @param timestamp - Event timestamp value.
 * @param source    - Id of the data source.
 * @param barrier   - Barrier type or 0 if not a barrier.
 * @param count     - Number of physics events.
 * @param timeoffset  - How long into the run we are.
 * @param stamp      - Unix timestamp.
 * @param divisor    - timeoffset/divisor = seconds.
 */
CRingPhysicsEventCountItem::CRingPhysicsEventCountItem(
    uint64_t timestamp, uint32_t source, uint32_t barrier,
    uint64_t count, uint32_t timeoffset, time_t stamp,
    int divisor) :
  ::CRingPhysicsEventCountItem()
{
  v12::pPhysicsEventCountItem pItem =
    reinterpret_cast<v12::pPhysicsEventCountItem>(getItemPointer());
  
  pItem->s_header.s_type = PHYSICS_EVENT_COUNT;
  pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_size = sizeof(v12::BodyHeader);
  pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_sourceId = source;
  pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_timestamp = timestamp;
  pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_barrier = barrier;
  
  v12::pPhysicsEventCountItemBody  pBody = &(pItem->s_body.u_hasBodyHeader.s_body);
  pBody->s_timeOffset = timeoffset;
  pBody->s_offsetDivisor = divisor;
  pBody->s_timestamp = stamp;
  pBody->s_originalSid = source;
  pBody->s_eventCount = count;
 
  setBodyCursor(pBody+1);
  updateSize();                
}

// destructor

CRingPhysicsEventCountItem::~CRingPhysicsEventCountItem() {}

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
  const v12::PhysicsEventCountItemBody* pItem =
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
 *   @return uint32_t - the item's original source id the body header
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

  time_t t = getTimestamp();
  string   time   = ctime(&t);
  uint32_t offset = getTimeOffset();
  uint32_t divisor= getTimeDivisor();
  uint64_t events = getEventCount();
  uint32_t sid    = getOriginalSourceId();
  
  float fOffset = static_cast<float>(offset)/static_cast<float>(divisor);
  
  const v12::CRingItem* This = reinterpret_cast<const v12::CRingItem*>(this);
  out << This->v12::CRingItem::bodyHeaderToString();
  out << time << " : " << events << " Triggers accepted as of " 
      << fOffset << " seconds into the run\n";
  out << " Average accepted trigger rate: " 
      <<  static_cast<double>(events)/fOffset
      << " events/second originally from sid: " << sid << std::endl;

  return out.str();
}


////////////////////////////////////////////////////////////////////////////
// Overrides that delegate to v12::CRingItem.
//  These require a fancy sideways cast and force as we're in a parallel hierarchy
//  in order to let the abstract hierarchy designate our interface.
//

/**
 * getBodySize
 *   @return size_t the size of the body of the item:
 */
size_t
CRingPhysicsEventCountItem::getBodySize() const
{
  const v12::CRingItem* pThis = reinterpret_cast<const v12::CRingItem*>(this);
  return pThis->v12::CRingItem::getBodySize();
}
/**
 * getBodyPointer
 *    @return void* [const] - return a pointer to the item body.
 */
const void*
CRingPhysicsEventCountItem::getBodyPointer() const
{
  const v12::CRingItem* pThis = reinterpret_cast<const v12::CRingItem*>(this);
  return pThis->v12::CRingItem::getBodyPointer();
}
void*
CRingPhysicsEventCountItem::getBodyPointer()
{
  v12::CRingItem* pThis = reinterpret_cast<v12::CRingItem*>(this);
  return pThis->v12::CRingItem::getBodyPointer();

}
/**
 * hasBodyHeader
 *    @return bool - true if the item has a body header.
 */
bool
CRingPhysicsEventCountItem::hasBodyHeader() const
{
  const v12::CRingItem* pThis = reinterpret_cast<const v12::CRingItem*>(this);
  return pThis->v12::CRingItem::hasBodyHeader();
}
/**
 * getEventTimestamp
 *    @return uint64_t - timestamp from body header.
 *    @throw std::logic_error - item has no body header.
 */
uint64_t
CRingPhysicsEventCountItem::getEventTimestamp() const
{
  const v12::CRingItem* pThis = reinterpret_cast<const v12::CRingItem*>(this);
  return pThis->v12::CRingItem::getEventTimestamp();
}
/**
 * getSourceId
 *    @return uint32_t - source id from the body header
 *    @throw std::logic_error - item has no body header.
 */
uint32_t
CRingPhysicsEventCountItem::getSourceId() const
{
  const v12::CRingItem* pThis = reinterpret_cast<const v12::CRingItem*>(this);
  return pThis->v12::CRingItem::getSourceId();
}
/**
 * getBarrierType
 *    @return uint32_t - barrier type from the body header.
 *    @throw std::logic_error - item has no body header.
 */
uint32_t
CRingPhysicsEventCountItem::getBarrierType() const
{
  const v12::CRingItem* pThis = reinterpret_cast<const v12::CRingItem*>(this);
  return pThis->v12::CRingItem::getBarrierType();
}
/**
 * getBodyHeader
 *   @return void*  - pointer to the item's body header.
 *   @retval nullptr - item has no body header.
 */
void*
CRingPhysicsEventCountItem::getBodyHeader() const
{
  const v12::CRingItem* pThis = reinterpret_cast<const v12::CRingItem*>(this);
  return pThis->v12::CRingItem::getBodyHeader();
}
/**
 * setBodyHeader
 *    If there's no body header the body is moved to accommodate one.
 *    The body header (new or old) is filled in with the values of the parameters.
 *    Note that an existing body header will retain the value of its'
 *    s_size field allowing for the preservation of body header extensions.
 *  @param timestamp - timestamp value for the body header.
 *  @param sourceId - source id value for the body header.
 *  @param barrierType - barrier type for the body header (defaults to 0).
 */
void
CRingPhysicsEventCountItem::setBodyHeader(
  uint64_t timestamp, uint32_t sourceId, uint32_t barrierType
)
{
  v12::CRingItem* pThis = reinterpret_cast<v12::CRingItem*>(this);
  pThis->v12::CRingItem::setBodyHeader(timestamp, sourceId, barrierType);
}


}                            // v12 namespace./

