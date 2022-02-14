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
#include <sstream>

using namespace std;

namespace v10 {
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
  init();
  v10::pPhysicsEventCountItem p =
      reinterpret_cast<v10::pPhysicsEventCountItem>(getItemPointer());
  p->s_timeOffset = 0;
  p->s_timestamp = static_cast<uint32_t>(time(NULL));
  p->s_eventCount = 0;
}
/*!
   Creates an event count item timestamped to now with a specified
   number of events and run offset.

   \param count       - number of events.
   \param timeOffset  - Seconds into the active run time at which this item
                        was produced.
*/
CRingPhysicsEventCountItem::CRingPhysicsEventCountItem(uint64_t count,
						       uint32_t timeOffset) :
  ::CRingPhysicsEventCountItem()
{
  init();
  v10::pPhysicsEventCountItem p =
      reinterpret_cast<v10::pPhysicsEventCountItem>(getItemPointer());
  p->s_timeOffset = timeOffset;
  p->s_timestamp = static_cast<uint32_t>(time(NULL));
  p->s_eventCount = count;
}
/*!
  Creates an event count item that is fully specified.
   \param count       - number of events.
   \param timeOffset  - Seconds into the active run time at which this item
   \param stamp       - Timestamp at which the event was produced.
*/
CRingPhysicsEventCountItem::CRingPhysicsEventCountItem(uint64_t count,
						       uint32_t timeOffset,
						       time_t   stamp) :
  ::CRingPhysicsEventCountItem()
{
  init();
  v10::pPhysicsEventCountItem p =
      reinterpret_cast<v10::pPhysicsEventCountItem>(getItemPointer());
  
  p->s_timeOffset  = timeOffset;
  p->s_timestamp  = stamp;
  p->s_eventCount = count;
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
  const PhysicsEventCountItem* p =
    reinterpret_cast<const PhysicsEventCountItem*>(getItemPointer());
  return p->s_timeOffset;
}
/*!
   set the time offset.
   \param offset - new value of time offset.
*/
void
CRingPhysicsEventCountItem::setTimeOffset(uint32_t offset)
{
  pPhysicsEventCountItem p =
    reinterpret_cast<pPhysicsEventCountItem>(getItemPointer());
  p->s_timeOffset = offset;
}
/**
 * computeElapsedTime
 *     This is the same as getTimeOffst as in v10 there's no time divisor.
 * @return float
 */
float
CRingPhysicsEventCountItem::computeElapsedTime() const
{
  return getTimeOffset();
}
/**
 * getTimeDivisor
 *    always 1.
 */
uint32_t
CRingPhysicsEventCountItem::getTimeDivisor() const
{
  return 1;
}



/*!
  \return time_t
  \retval current value of the timestamp.
*/
time_t
CRingPhysicsEventCountItem::getTimestamp() const
{
  const  PhysicsEventCountItem* p =
    reinterpret_cast<const PhysicsEventCountItem*>(getItemPointer());
  return p->s_timestamp;
}
/*!
  \param stamp - New value for the timestamp.
*/
void
CRingPhysicsEventCountItem::setTimestamp(time_t stamp)
{
  pPhysicsEventCountItem p =
    reinterpret_cast<pPhysicsEventCountItem>(getItemPointer());
  p->s_timestamp = stamp;
}

/*!
  \return uint64_t
  \retval number of events submitted  to the ring this run.
*/
uint64_t
CRingPhysicsEventCountItem::getEventCount() const
{
  const PhysicsEventCountItem* p =
    reinterpret_cast<const PhysicsEventCountItem*>(getItemPointer());
  return p->s_eventCount;
}
/*!
  \param count - new value for number of events submitted.
*/
void
CRingPhysicsEventCountItem::setEventCount(uint64_t count)
{
  pPhysicsEventCountItem p =
    reinterpret_cast<pPhysicsEventCountItem>(getItemPointer());
  p->s_eventCount = count;
}
//////////////////////////////////////////////////////////
//
// Virtual method overrides.

/**
 * getBodyHeader
 *   @return nullptr - no body header in v10.
 */
void*
CRingPhysicsEventCountItem::getBodyHeader() const
{
  return nullptr;
}
/**
 * setBodyHeader
 *     no-op.
 */
void
CRingPhysicsEventCountItem::setBodyHeader(uint64_t timestamp, uint32_t sourceId,
                         uint32_t barrierType
)
{}
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
  uint64_t events = getEventCount();


  out << time << " : " << events << " Triggers accepted as of " 
      << offset << " seconds into the run\n";
  out << " Average accepted trigger rate: " 
      <<  (static_cast<double>(events)/static_cast<double>(offset))
      << " events/second \n";

  return out.str();
}

/////////////////////////////////////////////////////////////////////////////////
//
// Private utilities.
//


/*
** initialize by setting the item pointer and body cursor
** appropriatly.
*/
void
CRingPhysicsEventCountItem::init()
{
  

  uint8_t* pCursor = reinterpret_cast<uint8_t*>(getItemPointer());
  pCursor         += sizeof(v10::PhysicsEventCountItem);
  setBodyCursor(pCursor);
  updateSize();

}
}