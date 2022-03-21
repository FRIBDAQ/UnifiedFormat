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

using namespace std;
namespace v11 {
///////////////////////////////////////////////////////////////////////////////////
//
// Constructors and other canonicals.

/*!
   Default constructor has a timestamp of now, a time offset and
   event count of 0.  This version will not have a body header.
*/
CRingPhysicsEventCountItem::CRingPhysicsEventCountItem() :
  ::CRingPhysicsEventCountItem()
{
  
  v11::pPhysicsEventCountItem pItem =
    reinterpret_cast<v11::pPhysicsEventCountItem>(getItemPointer());
  
  pItem->s_header.s_type = v11::PHYSICS_EVENT_COUNT;
  pItem->s_body.u_noBodyHeader.s_mbz = 0;
  
  v11::pPhysicsEventCountItemBody pBody = &pItem->s_body.u_noBodyHeader.s_body;
      
    
  pBody->s_timeOffset = 0;
  pBody->s_timestamp = static_cast<uint32_t>(time(NULL));
  pBody->s_eventCount = 0;
  pBody->s_offsetDivisor = 1;
  setBodyCursor(pBody+1);
  updateSize();
}
/*!
   Creates an event count item timestamped to now with a specified
   number of events and run offset.

   \param count       - number of events.
   \param timeOffset  - Seconds into the active run time at which this item
                        was produced.
   @param divisor (optional defaults to 1) divisor to turn timeOffset into seconds.
*/
CRingPhysicsEventCountItem::CRingPhysicsEventCountItem(uint64_t count,
			     uint32_t timeOffset, unsigned divisor) :
  ::CRingPhysicsEventCountItem()
{

  v11::pPhysicsEventCountItem pItem =
    reinterpret_cast<v11::pPhysicsEventCountItem>(getItemPointer());
  
  pItem->s_header.s_type = v11::PHYSICS_EVENT_COUNT;
  pItem->s_body.u_noBodyHeader.s_mbz = 0;
  
  v11::pPhysicsEventCountItemBody pBody = &pItem->s_body.u_noBodyHeader.s_body;
      
    
  pBody->s_timeOffset = timeOffset;
  pBody->s_timestamp = static_cast<uint32_t>(time(NULL));
  pBody->s_eventCount = count;
  pBody->s_offsetDivisor = divisor;
  
  setBodyCursor(pBody+1);
  updateSize();
}
/*!
  Creates an event count item that is fully specified.
   \param count       - number of events.
   \param timeOffset  - Seconds into the active run time at which this item
   \param stamp       - Timestamp at which the event was produced.
   @param divisor     - time offset divisor 
*/
CRingPhysicsEventCountItem::CRingPhysicsEventCountItem(uint64_t count,
						       uint32_t timeOffset,
						       time_t   stamp, unsigned divisor) :
  ::CRingPhysicsEventCountItem()
{
  v11::pPhysicsEventCountItem pItem =
    reinterpret_cast<v11::pPhysicsEventCountItem>(getItemPointer());
  
  pItem->s_header.s_type = v11::PHYSICS_EVENT_COUNT;
  pItem->s_body.u_noBodyHeader.s_mbz = 0;
  
  v11::pPhysicsEventCountItemBody pBody = &pItem->s_body.u_noBodyHeader.s_body;
      
    
  pBody->s_timeOffset = timeOffset;
  pBody->s_timestamp = stamp;
  pBody->s_eventCount = count;
  pBody->s_offsetDivisor = divisor;
  
  setBodyCursor(pBody+1);
  updateSize();
}
/**
 *   constructor with a non empty 

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
 * @param barrier    - any barrier type.
 */
CRingPhysicsEventCountItem::CRingPhysicsEventCountItem(
           uint64_t count, 
			     uint32_t timeoffset, 
			     time_t stamp, uint64_t timestamp,  uint32_t sid, unsigned divisor,
           uint32_t barrier) :
  ::CRingPhysicsEventCountItem()
{
  v11::pPhysicsEventCountItem pItem =
    reinterpret_cast<v11::pPhysicsEventCountItem>(getItemPointer());
  
  pItem->s_header.s_type = v11::PHYSICS_EVENT_COUNT;
  
  v11::pBodyHeader pBh = &(pItem->s_body.u_hasBodyHeader.s_bodyHeader);
  
  pBh->s_size = sizeof(v11::BodyHeader);
  pBh->s_timestamp = timestamp;
  pBh->s_sourceId  = sid;
  pBh->s_barrier   = barrier;
  
  v11::pPhysicsEventCountItemBody pBody = &pItem->s_body.u_hasBodyHeader.s_body;
      
    
  pBody->s_timeOffset = timeoffset;
  pBody->s_timestamp = stamp;
  pBody->s_eventCount = count;
  pBody->s_offsetDivisor = divisor;
  
  setBodyCursor(pBody+1);
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
    \retval The curren value of the time offset.
*/
uint32_t
CRingPhysicsEventCountItem::getTimeOffset() const
{
  const PhysicsEventCountItemBody* pItem =
    reinterpret_cast<const PhysicsEventCountItemBody*>(getBodyPointer());

  return pItem->s_timeOffset;
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
  \return time_t
  \retval current value of the timestamp.
*/
time_t
CRingPhysicsEventCountItem::getTimestamp() const
{
  const  PhysicsEventCountItemBody* pItem =
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
 *    v11 items don't have an original source id field in them so
 * @return uint32_t - if there's a body header return the sid from there.
 * @retval 0xffffffff - If there's no body header.
 */
uint32_t
CRingPhysicsEventCountItem::getOriginalSourceId() const
{
  const v11::BodyHeader* pB =
    reinterpret_cast<const v11::BodyHeader*>(getBodyHeader());
  if (pB) {
    return pB->s_sourceId;
  } else {
    return 0xffffffff;
  }
}

/**
 * getBodyPointer
 *    This can be satisifed by v11::CRingItem.
 *  @return void*
 */
void*
CRingPhysicsEventCountItem::getBodyPointer()
{
    v11::CRingItem* pThis = reinterpret_cast<v11::CRingItem*>(this);
    return pThis->v11::CRingItem::getBodyPointer();
}
const void*
CRingPhysicsEventCountItem::getBodyPointer() const
{
    const v11::CRingItem* pThis = reinterpret_cast<const v11::CRingItem*>(this);
    return pThis->v11::CRingItem::getBodyPointer();
  
}
/**
 * getBodySize
 *    @return sizeof(PhysicsEventCountItemBody)
 */
size_t
CRingPhysicsEventCountItem::getBodySize() const
{
  return sizeof(PhysicsEventCountItemBody);
}

/**
 * setBodyHeader
 *    delegates to v11::CRingItem::setBodyHeader
 *@param timestamp - event timestamp.
 *@param sourceId  - Source id of fragment.
 *@param barrierType - Barrier type code
 */
void
CRingPhysicsEventCountItem::setBodyHeader(uint64_t timestamp, uint32_t sourceId,
                         uint32_t barrierType)
{
    v11::CRingItem* pThis = reinterpret_cast<v11::CRingItem*>(this);
    pThis->v11::CRingItem::setBodyHeader(timestamp, sourceId, barrierType);
}
/**
 * getBodyHeader
 *   @return void*
 */
void*
CRingPhysicsEventCountItem::getBodyHeader() const
{
      const v11::CRingItem* pThis = reinterpret_cast<const v11::CRingItem*>(this);
      return pThis->v11::CRingItem::getBodyHeader();
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
  
  float fOffset = static_cast<float>(offset)/static_cast<float>(divisor);
  
  const v11::CRingItem*  pThis = reinterpret_cast<const v11::CRingItem*>(this);
  out << pThis->v11::CRingItem::bodyHeaderToString();
  out << time << " : " << events << " Triggers accepted as of " 
      << fOffset << " seconds into the run\n";
  out << " Average accepted trigger rate: " 
      <<  (static_cast<double>(events)/static_cast<double>(offset))
      << " events/second \n";

  return out.str();
}

/////////////////////////////////////////////////////////////////////////////////
//
// Private utilities.
//



}                                 // v11 namespace