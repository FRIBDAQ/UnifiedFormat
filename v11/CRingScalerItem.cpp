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
#include "CRingItem.h"
#include "DataFormat.h"
#include <time.h>
#include <string.h>
#include <sstream>
#include <stdio.h>
#include <stdexcept>

using namespace std;


namespace v11 {
///////////////////////////////////////////////////////////////////////////////////////
//
// Constructors and other canonicals.
//

/*!
  Construct a scaler item with the scalers all zeroed and the times not set...
  except for the absolute timestamp which is set to now.
  \param numScalers - Number of scalers that will be put in the item.
*/
CRingScalerItem::CRingScalerItem(size_t numScalers) :
  ::CRingScalerItem(numScalers)
  
{
  v11::pScalerItem pItem = reinterpret_cast<v11::pScalerItem>(getItemPointer());
  pItem->s_header.s_size = v11::PERIODIC_SCALERS;
  pItem->s_body.u_noBodyHeader.s_mbz = 0;
  
  v11::pScalerItemBody pScalers = &(pItem->s_body.u_noBodyHeader.s_body);
  
  pScalers->s_intervalStartOffset = 0;
  pScalers->s_intervalEndOffset   = 0;
  pScalers->s_timestamp = static_cast<uint32_t>(time(NULL));
  pScalers->s_scalerCount = numScalers;
  pScalers->s_isIncremental = 1;
  pScalers->s_intervalDivisor = 1;
  memset(pScalers->s_scalers, 0, numScalers*sizeof(uint32_t));
  
  uint32_t* pCursor = reinterpret_cast<uint32_t*>(pScalers+1); // -> past body.
  pCursor += numScalers;                      // -> past scalers.
  setBodyCursor(pCursor);
  updateSize();                      // Sets pItem->s_header.s_size properl8y.               
  
}
/*!
  Construct a scaler item with all the info specified.
  \param startTime - incremental scaler interval start time.
  \param endTime   - incremental scaler interval end time
  \param timestamp - Absolute system time at which the item was created.
  \param scalers   - Vector of scaler values.
  \param isIncremental - value of the incremental flag (defaults to true).
  @param sid       - Source id.
  \param timestampDivisor - Value of the timestamp divisor - defaults to 1.

   @note  This constructor forces a body header with timestamp 0xffffffffffffffff
          and barrier 0 with sourceId = sid.
*/
CRingScalerItem::CRingScalerItem(uint32_t startTime,
        uint32_t stopTime,
        time_t   timestamp,
        std::vector<uint32_t> scalers,
        bool isIncremental,
        uint32_t sid,
        uint32_t timeOffsetDivisor) :
  CRingScalerItem(scalers.size())
{
  v11::pScalerItem pItem = reinterpret_cast<v11::pScalerItem>(getItemPointer());
  pItem->s_header.s_size = v11::PERIODIC_SCALERS;
  
  v11::pBodyHeader pB    = &(pItem->s_body.u_hasBodyHeader.s_bodyHeader);
  pB->s_size = sizeof(v11::BodyHeader);
  pB->s_timestamp = 0xffffffffffffffffL;
  pB->s_sourceId  = sid;
  pB->s_barrier   = 0;

  v11::pScalerItemBody pScalers = &(pItem->s_body.u_hasBodyHeader.s_body);
    
  pScalers->s_intervalStartOffset = startTime;
  pScalers->s_intervalEndOffset   = stopTime;
  pScalers->s_timestamp           = timestamp;
  pScalers->s_scalerCount         = scalers.size();
  pScalers->s_isIncremental       = isIncremental ? 1: 0;
  pScalers->s_intervalDivisor     = timeOffsetDivisor;

  for (int i = 0; i  < scalers.size(); i++) {
    pScalers->s_scalers[i] = scalers[i];
  }
  uint32_t* pCursor = reinterpret_cast<uint32_t*>(pScalers + 1);
  pCursor          += pScalers->s_scalerCount;
  setBodyCursor(pCursor);
  updateSize();
}
/**
 * constructor - with body header.
 *
 * Construcs a scaler ring item that has a body header.
 *
 * @param eventTimestamp - The event timestamp for the body header.
 * @param source         - id of the source that created this item.
 * @param barrier        - barrier type or 0 if this was not a barrier.
  \param startTime - incremental scaler interval start time.
  \param endTime   - incremental scaler interval end time
  \param timestamp - Absolute system time at which the item was created.
  \param scalers   - Vector of scaler values.
  @param timeDivisor - The divisor for the start/end times that yields seconds.
                       defaults to 1.
  @param incremental - True (default) if the scaler item is incremental.
 */
CRingScalerItem::CRingScalerItem(
    uint64_t eventTimestamp, uint32_t source, uint32_t barrier, uint32_t startTime,
    uint32_t stopTime, time_t   timestamp, std::vector<uint32_t> scalers,
    uint32_t timeDivisor, bool incremental) :
    ::CRingScalerItem(scalers.size())
{
  
    v11::pScalerItem pItem = reinterpret_cast<v11::pScalerItem>(getItemPointer());
    pItem->s_header.s_type = v11::PERIODIC_SCALERS;
    
    v11::pBodyHeader pBh   = &(pItem->s_body.u_hasBodyHeader.s_bodyHeader);
    pBh->s_size = sizeof(v11::BodyHeader);
    pBh->s_timestamp = eventTimestamp;
    pBh->s_sourceId  = source;
    pBh->s_barrier   = barrier;
    
    v11::pScalerItemBody pScalers = &(pItem->s_body.u_hasBodyHeader.s_body);
      
    pScalers->s_intervalStartOffset = startTime;
    pScalers->s_intervalEndOffset   = stopTime;
    pScalers->s_timestamp           = timestamp;
    pScalers->s_scalerCount         = scalers.size();
    pScalers->s_isIncremental = incremental ? 1 : 0;
    pScalers->s_intervalDivisor = timeDivisor;

    for (int i = 0; i  < scalers.size(); i++) {
      pScalers->s_scalers[i] = scalers[i];
    }    
    uint32_t* pCursor = reinterpret_cast<uint32_t*>(pScalers + 1);
    pCursor          += pScalers->s_scalerCount ;
    setBodyCursor(pCursor);
    updateSize();
}


/*!
  Destruction just delegates.
*/
CRingScalerItem::~CRingScalerItem()
{}
   
/////////////////////////////////////////////////////////////////////////////////////
//
//  Accessors - selectors and mutators.
//

/*!
   Set the scaler interval start time.

   \param startTime - the new interval start time.
*/
void
CRingScalerItem::setStartTime(uint32_t startTime)
{
    v11::pScalerItemBody pScalers =
      reinterpret_cast<v11::pScalerItemBody>(getBodyPointer());
    
    pScalers->s_intervalStartOffset = startTime;
}
/*!
   \return uint32_t
   \retval the current interval start time.
*/
uint32_t
CRingScalerItem::getStartTime() const
{
    const v11::ScalerItemBody* pScalers =
      reinterpret_cast<const v11::ScalerItemBody*>(getBodyPointer());

    return pScalers->s_intervalStartOffset;
}
/**
 * computeStartTime
 *
 * Determine the floating point start timein seconds using the btime and
 * divisor values.
 *
 * @return float
 */
float
CRingScalerItem::computeStartTime() const
{
     const v11::ScalerItemBody* pScalers =
      reinterpret_cast<const v11::ScalerItemBody*>(getBodyPointer());
     float start   = pScalers->s_intervalStartOffset;
     float divisor = pScalers->s_intervalDivisor;
     return start/divisor;
}

/*!
   Set the interval end time
   \pram endTime - the new interval end time.
*/
void
CRingScalerItem::setEndTime(uint32_t endTime) 
{
    v11::pScalerItemBody pScalers =
      reinterpret_cast<v11::pScalerItemBody>(getBodyPointer());
    pScalers->s_intervalEndOffset = endTime;
}
/*!
   \return uint32_t
   \retval The current interval end time.
*/
uint32_t
CRingScalerItem::getEndTime() const
{
    const v11::ScalerItemBody* pScalers =
      reinterpret_cast<const v11::ScalerItemBody*>(getBodyPointer());
    return pScalers->s_intervalEndOffset;
}
/**
 * computeEndTime
 *
 * Compute the interval end time using both the etime and the divisor.
 *
 * @return float - time in seconds.
 */
float
CRingScalerItem::computeEndTime() const
{
    const v11::ScalerItemBody* pScalers =
      reinterpret_cast<const v11::ScalerItemBody*>(getBodyPointer());
    float end   = pScalers->s_intervalEndOffset;
    float divisor = pScalers->s_intervalDivisor;
    return end/divisor;   
}
/**
 * getTimestampDivisor
 *   Get the divisor that must be applied to get a real time offset.
 *
 * @return uint32_t
 */
uint32_t
CRingScalerItem::getTimeDivisor() const
{
    const v11::ScalerItemBody* pScalers =
      reinterpret_cast<const v11::ScalerItemBody*>(getBodyPointer());
    return pScalers->s_intervalDivisor;
}

/**
 * isIncremental
 *
 * @return bool true if s_isIncremental is true etc.
 */
bool
CRingScalerItem::isIncremental() const
{
    const v11::ScalerItemBody* pScalers =
      reinterpret_cast<const v11::ScalerItemBody*>(getBodyPointer());
    return (pScalers->s_isIncremental != 0);
}

/*!
   set the absolute timestamp of the event.

   \param stamp - the time stampe.
*/
void
CRingScalerItem::setTimestamp(time_t  stamp)
{
    v11::pScalerItemBody pScalers =
      reinterpret_cast<v11::pScalerItemBody>(getBodyPointer());
    pScalers->s_timestamp = stamp;
}
/*!
   \return time_t
   \retval the current timestamp.
*/
time_t
CRingScalerItem::getTimestamp() const
{
    const v11::ScalerItemBody* pScalers =
      reinterpret_cast<const v11::ScalerItemBody*>(getBodyPointer());
    return pScalers->s_timestamp;
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
    v11::pScalerItemBody pScalers =
      reinterpret_cast<v11::pScalerItemBody>(getBodyPointer());

    pScalers->s_scalers[channel] = value;
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
    const v11::ScalerItemBody* pScalers =
      reinterpret_cast<const v11::ScalerItemBody*>(getBodyPointer());
  
    return pScalers->s_scalers[channel];
}
/*!
    \return std::vector<uint32_t>
    \retval - the scalers.
*/
vector<uint32_t>
CRingScalerItem::getScalers() const
{
    vector<uint32_t> result;
    const v11::ScalerItemBody* pScalers =
      reinterpret_cast<const v11::ScalerItemBody*>(getBodyPointer());

    for (int i =0; i < pScalers->s_scalerCount; i++) {
      result.push_back(pScalers->s_scalers[i]);
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
    const v11::ScalerItemBody* pScalers =
      reinterpret_cast<const v11::ScalerItemBody*>(getBodyPointer());

    return pScalers->s_scalerCount;
}
/**
 * getOriginalSourceId
 *    There is no original source id in v11 so we return the
 *    one in the body header if there is one or 0 if not:
 * @return uint32_t
 */
uint32_t
CRingScalerItem::getOriginalSourceId() const
{
    const v11::BodyHeader* pH =
      reinterpret_cast<const v11::BodyHeader*>(getBodyHeader());
    if (pH) {
      return pH->s_sourceId;
    } else {
      return 0;
    }
}
/////////////////////////////////////////////////////////////////////////////////////
//
// Private utilities



/*-------------------------------------------------------
** Virtual method overrides
*-------------------------------------------------------*/

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
  return std::string("Scaler");
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
  time_t ts = getTimestamp();
  string   time  = ctime(&ts);
  vector<uint32_t> scalers = getScalers();
  for (int i =0; i < scalers.size(); i++) {
    scalers[i] = scalers[i] & m_ScalerFormatMask; // Mask off unused bits.
  }

  float   duration = end - start;

  out << time << " : Scalers:\n";
  out << "Interval start time: " << start << " end: " << end << " seconds in to the run\n\n";
  const v11::CRingItem* p = reinterpret_cast<const v11::CRingItem*>(this);
  out << p->v11::CRingItem::bodyHeaderToString();
  out << (isIncremental() ? "Scalers are incremental" : "Scalers are not incremental") << std::endl;

  out << "Index         Counts                 Rate\n";
  for (int i=0; i < scalers.size(); i++) {
    char line[128];
    double rate = (static_cast<double>(scalers[i])/duration);

    sprintf(line, "%5d      %9d                 %.2f\n",
	    i, scalers[i], rate);
    out << line;
  }


  return out.str();
  
}
// delegations to v11::CRingItem:

void*
CRingScalerItem::getBodyPointer()
{
  v11::CRingItem* p = reinterpret_cast<v11::CRingItem*>(this);
  return p->v11::CRingItem::getBodyPointer();
}
const void*
CRingScalerItem::getBodyPointer() const
{
  const v11::CRingItem* p = reinterpret_cast<const v11::CRingItem*>(this);
  return p->v11::CRingItem::getBodyPointer();
}

void*
CRingScalerItem::getBodyHeader() const
{
  const v11::CRingItem* p = reinterpret_cast<const v11::CRingItem*>(this);
  return p->v11::CRingItem::getBodyHeader();
}


void
CRingScalerItem::setBodyHeader(
      uint64_t timestamp, uint32_t sourceId,
      uint32_t barrierType
)
{
  v11::CRingItem* p = reinterpret_cast<v11::CRingItem*>(this);
  p->v11::CRingItem::setBodyHeader(
    timestamp, sourceId, barrierType
  );
}

/*-------------------------------------------------------
** Private utilities:
*---------------------------------------------------------

/*
** Throws a range error if the channel number is invalid.
*/
void
CRingScalerItem::throwIfInvalidChannel(uint32_t channel,
				       const char* message) const 
{
    const v11::ScalerItemBody* pScalers =
      reinterpret_cast<const ScalerItemBody*>(getBodyPointer());

    if (channel >= pScalers->s_scalerCount) {
      std::stringstream strMsg;
      strMsg << "Request for scaler " << channel << " is out of range."
        << "   There are only " << pScalers->s_scalerCount << " scalers";
      std::string msg = strMsg.str();
      throw std::out_of_range(msg);
    }
}
}                                     // namespace