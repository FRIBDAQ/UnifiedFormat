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

#include "CRingScalerItem.h"
#include "DataFormat.h"
#include <time.h>
#include <string.h>
#include <sstream>
#include <stdio.h>
#include <stdexcept>

using namespace std;

namespace v10 {

///////////////////////////////////////////////////////////////////////////////////////
//
// Constructors and other canonicals.
//

/*!
  Construct a scaler item with the scalers all zeroed and the times not set...
  except for the absolute timestamp which is set to now.
  \param numScalers - Number of scalers that will be put in the item.
  This unconditionally constructs an incremental scaler item.
*/
CRingScalerItem::CRingScalerItem(size_t numScalers) :
 ::CRingScalerItem(numScalers),
  m_isIncremental(true)
  
{
  init(numScalers);                   // Set the size and type
  v10::pScalerItem p = reinterpret_cast<v10::pScalerItem>(getItemPointer());
  
  p->s_intervalStartOffset = 0;
  p->s_intervalEndOffset   = 0;
  p->s_timestamp = static_cast<uint32_t>(time(NULL));
  p->s_scalerCount = numScalers;
  memset(p->s_scalers, 0, numScalers*sizeof(uint32_t));
}
/*!
  Construct a scaler item with all the info specified.
  \param startTime - incremental scaler interval start time.
  \param endTime   - incremental scaler interval end time
  \param timestamp - Absolute system time at which the item was created.
  \param scalers   - Vector of scaler values.
  @param isIncremental - incremental flag.
  @param sid       - Original source id (ignored).
  @param timeOffsetDivisor - offset divisor (only used for on-incremental).

*/
CRingScalerItem::CRingScalerItem(uint32_t startTime,
				 uint32_t stopTime,
				 time_t   timestamp,
				 std::vector<uint32_t> scalers,
         bool isIncremental, uint32_t sid, uint32_t timeOffsetDivisor) :
  ::CRingScalerItem(startTime, stopTime, timestamp, scalers, isIncremental, sid, timeOffsetDivisor),
  m_isIncremental(isIncremental)
{
  init(scalers.size());
  
  // The ring item type depends on the state of the incremental flag:
  
  if (m_isIncremental) {
    v10::pScalerItem p = reinterpret_cast<v10::pScalerItem>(getItemPointer());
    p->s_intervalStartOffset = startTime;
    p->s_intervalEndOffset   = stopTime;
    p->s_timestamp           = timestamp;
    p->s_scalerCount         = scalers.size();
    for (int i = 0; i  < scalers.size(); i++) {
      p->s_scalers[i] = scalers[i];
    }
  } else {
    v10::pNonIncrTimestampedScaler p =
      reinterpret_cast<v10::pNonIncrTimestampedScaler>(getItemPointer());
      p->s_eventTimestamp = 0;                // No mechanism to get this.
      p->s_intervalStartOffset = startTime;
      p->s_intervalEndOffset   = stopTime;
      p->s_intervalDivisor     = timeOffsetDivisor;
      p->s_clockTimestamp      = timestamp;
      p->s_scalerCount         = scalers.size();
      for (int i = 0; i  < scalers.size(); i++) {
        p->s_scalers[i] = scalers[i];
      }
  }
  
}
CRingScalerItem::~CRingScalerItem() {}
    
/////////////////////////////////////////////////////////////////////////////////////
//
//  Accessors - selectors and mutators.
// Note: these are not as simple because the underlying ring item could be a
//       ScaleItem or a NonIncrTimestampedScaler
//       in the event we can eventually get rid of the m_isIncremental flag,
//      we use the underlying ring item type to differentiate. See isIncremental().
//

/*!
   Set the scaler interval start time.

   \param startTime - the new interval start time.
*/
void
CRingScalerItem::setStartTime(uint32_t startTime)
{
  if (isIncremental()) {
    pScalerItem p = reinterpret_cast<pScalerItem>(getItemPointer());
    p->s_intervalStartOffset = startTime;
  } else {
      pNonIncrTimestampedScaler p =
        reinterpret_cast<pNonIncrTimestampedScaler>(getItemPointer());
      p->s_intervalStartOffset = startTime;
  }
  
}
/*!
   \return uint32_t
   \retval the current interval start time.
*/
uint32_t
CRingScalerItem::getStartTime() const
{
  uint32_t result;
  if (isIncremental()) {
    const ScalerItem* p = reinterpret_cast<const ScalerItem*>(getItemPointer());
    result = p->s_intervalStartOffset;
  } else {
    const NonIncrTimestampedScaler* p =
      reinterpret_cast<const NonIncrTimestampedScaler*>(getItemPointer());
      result = p->s_intervalStartOffset;
  }
  return result;
}
/**
 * computeStartTime
 *    Compute the time in seconds of the start time:
 */
float
CRingScalerItem::computeStartTime() const
{
  uint32_t s = getStartTime();
  uint32_t d = getTimeDivisor();
  
  return float(s)/float(d);
}

/*!
   Set the interval ent time
   \pram endTime - the new interval end time.
*/
void
CRingScalerItem::setEndTime(uint32_t endTime) 
{
  if (isIncremental()) {
    pScalerItem p = reinterpret_cast<pScalerItem>(getItemPointer());
    p->s_intervalEndOffset = endTime;
  } else {
    pNonIncrTimestampedScaler p =
      reinterpret_cast<pNonIncrTimestampedScaler>(getItemPointer());
      p->s_intervalEndOffset = endTime;
  }
  
}
/*!
   \return uint32_t
   \retval The current interval end time.
*/
uint32_t
CRingScalerItem::getEndTime() const
{
  uint32_t result;
  if (isIncremental()) {
    const ScalerItem* p = reinterpret_cast<const ScalerItem*>(getItemPointer());
    result = p->s_intervalEndOffset;
  } else {
    const NonIncrTimestampedScaler* p =
      reinterpret_cast<const NonIncrTimestampedScaler*>(getItemPointer());
      result = p->s_intervalEndOffset;
  }
  return result;
}
/**
 * computeEndTime
 *     Determine the end time offset in seconds.
 *  @return float
 */
float
CRingScalerItem::computeEndTime() const
{
  uint32_t e = getEndTime();
  uint32_t d = getTimeDivisor();
  return float(e)/float(d);
}
/**
 * getTimeDivisor
 *    @return uint32_t
 */
uint32_t
CRingScalerItem::getTimeDivisor() const
{
  uint32_t result;
  if (isIncremental()) {
    result = 1;
  } else {
    const NonIncrTimestampedScaler* p =
      reinterpret_cast<const NonIncrTimestampedScaler*>(getItemPointer());
    result = p->s_intervalDivisor;      
  }
  return result;
}


/*!
   set the absolute timestamp of the event.

   \param stamp - the time stamp
*/
void
CRingScalerItem::setTimestamp(time_t  stamp)
{
  if (isIncremental()) {
    pScalerItem p = reinterpret_cast<pScalerItem>(getItemPointer());
    p->s_timestamp = stamp;
  } else {
    pNonIncrTimestampedScaler p =
      reinterpret_cast<pNonIncrTimestampedScaler>(getItemPointer());
      p->s_clockTimestamp = stamp;
  }
  
}
/*!
   \return time_t
   \retval the current timestamp.
*/
time_t
CRingScalerItem::getTimestamp() const
{
  time_t result;
  if (isIncremental()) {
    const ScalerItem* p = reinterpret_cast<const ScalerItem*>(getItemPointer());
    result = p->s_timestamp;
  } else {
    const NonIncrTimestampedScaler* p =
      reinterpret_cast<const NonIncrTimestampedScaler*>(getItemPointer());
      result = p->s_clockTimestamp;
  }
  return result;
  
}
/**
 * isIncremental
 *  @return true
 */
bool
CRingScalerItem::isIncremental() const
{
  const v10::RingItemHeader* p
    = reinterpret_cast<const v10::RingItemHeader*>(getItemPointer());
  return p->s_type == INCREMENTAL_SCALERS;
}

/*!
  Set a scaler value.
  \param channel - Number of the channel to modify.
  \param value   - The new value for that channel.
  \throw CRangError if channel is too big.
*/
void
CRingScalerItem::setScaler(uint32_t channel, uint32_t value)
{
  throwIfInvalidChannel(channel, "Attempting to set a scaler value");
  uint32_t* p;
  if (isIncremental()) {
    pScalerItem pItem = reinterpret_cast<pScalerItem>(getItemPointer());
    p = pItem->s_scalers;
  } else {
    pNonIncrTimestampedScaler pItem =
      reinterpret_cast<pNonIncrTimestampedScaler>(getItemPointer());
    p = pItem->s_scalers;
  }
  p[channel] = value;
}
/*!

  \param channel - Number of the channel we want.
  \return uint32_t
  \retval Value of the selected scaler channel
  \throw CRangeError - if the channel number is out of range.
*/
uint32_t
CRingScalerItem::getScaler(uint32_t channel) const
{
  throwIfInvalidChannel(channel, "Attempting to get a scaler value");
  const uint32_t* p;
  if (isIncremental()) {
    const ScalerItem* pItem = reinterpret_cast<const ScalerItem*>(getItemPointer());
    p = pItem->s_scalers;
  } else {
    const NonIncrTimestampedScaler* pItem =
      reinterpret_cast<const NonIncrTimestampedScaler*>(getItemPointer());
      p = pItem->s_scalers;
  }
  return p[channel];
}
/*!
    \return std::vector<uint32_t>
    \retval - the scalers.
*/
vector<uint32_t>
CRingScalerItem::getScalers() const
{
  uint32_t nScalers = getScalerCount();
  const uint32_t* p;
  vector<uint32_t> result;
  
  if (isIncremental()) {
    const ScalerItem* pItem = reinterpret_cast<const ScalerItem*>(getItemPointer());
    p        = pItem->s_scalers;
  } else {
    const NonIncrTimestampedScaler* pItem =
      reinterpret_cast<const NonIncrTimestampedScaler*>(getItemPointer());      
      p        = pItem->s_scalers;
  }
  
  for (int i =0; i < nScalers; i++) {
    result.push_back(p[i]);
  }
  return result;
}

/*!
   \return uint32_t
   \retval number of scalers
*/
uint32_t
CRingScalerItem::getScalerCount() const
{
  uint32_t result;
  if (isIncremental()) {
    const ScalerItem* pItem = reinterpret_cast<const ScalerItem*>(getItemPointer());
    result = pItem->s_scalerCount;
  } else {
    const NonIncrTimestampedScaler* pItem =
      reinterpret_cast<const NonIncrTimestampedScaler*>(getItemPointer());      
      result =  pItem->s_scalerCount;
  }
  return result;
}
/**
 * getOriginalSourceId
 * @return uint32_t
 */
uint32_t
CRingScalerItem::getOriginalSourceId() const
{
  return 0;
}
  
/////////////////////////////////////////////////////////////////////////////////////
//
// Private utilities


/*
 * Do common initialization.. setting m_pScalers and the cursor.
 *   We assume isIncremental has been set.
 */
void
CRingScalerItem::init(size_t n)
{
  uint8_t* p = reinterpret_cast<uint8_t*>(getItemPointer());
  v10::pRingItemHeader pH = reinterpret_cast<v10::pRingItemHeader>(p);
  uint32_t nBytes;
  // This is the one time we need that flag:
  
  if (m_isIncremental) {
    pH->s_type = v10::INCREMENTAL_SCALERS;
    nBytes    = sizeof(ScalerItem) + (n-1)*sizeof(uint32_t);
  } else {
    pH->s_type = v10::TIMESTAMPED_NONINCR_SCALERS;
    nBytes    = sizeof(NonIncrTimestampedScaler) + (n-1)*sizeof(uint32_t);
  }
  pH->s_size = nBytes;
  p += nBytes;
  setBodyCursor(p);
  updateSize();
}
/*-------------------------------------------------------
** Virtual method overrides
*-------------------------------------------------------*/

/**
 * getBodyHeader
 *   @return nullptr
 */
void*
CRingScalerItem::getBodyHeader() const
{
  return nullptr;
}
/**
 * setBodyHeader -- no-op.
 */
void
CRingScalerItem::setBodyHeader(
      uint64_t timestamp, uint32_t sourceId,
      uint32_t barrierType)
{}


/**
 * typeName
 *
 *  Returns the string associated with this type of
 *  ring item ("Scaler").
 *
 * @return std::string - the item type name string.
 */
std::string
CRingScalerItem::typeName() const
{
  if (isIncremental()) {
    return std::string("Incremental Scalers");
  } else {
    return std::string("Nonincremental Scalers");
  }
}
/**
 * toString
 *
 *  Return a textual human readable version of the data
 *  in this item.
 *
 * @return std::string - the text string.
 */

std::string
CRingScalerItem::toString() const
{
  std::ostringstream out;

  float end   = computeEndTime();
  float start = computeStartTime();
  time_t t = getTimestamp();
  string   time  = ctime(&t);
  vector<uint32_t> scalers = getScalers();

  float   duration = static_cast<float>(end - start);

  out << time << " : Incremental scalers:\n";
  out << "Interval start time: " << start << " end: " << end << " seconds in to the run\n\n";
  

  out << "Index         Counts                 Rate\n";
  for (int i=0; i < scalers.size(); i++) {
    char line[128];
    uint32_t scaler = (scalers[i] & m_ScalerFormatMask);
    double rate = (static_cast<double>(scaler)/duration);

    sprintf(line, "%5d      %9d                 %.2f\n",
	    i, scaler, rate);
    out << line;
  }


  return out.str();
  
}

/*-------------------------------------------------------
** Private utilities:
*---------------------------------------------------------
/*
 *  Given the number of scalers determine the size of the body.
 */

/*
** Throws a range error if the channel number is invalid.
*/
void
CRingScalerItem::throwIfInvalidChannel(uint32_t channel,
				       const char* message) const
{
  if (channel >= getScalerCount()) {
    std::stringstream s;
    s << "Channel " << channel << " is larger than the number of scalers ("
      << getScalerCount() << ")";
    std::string msg(s.str());
    throw std::range_error(msg);
  }
}
}