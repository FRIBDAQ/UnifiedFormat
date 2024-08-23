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
#include "DataFormat.h"   // shape of v12 ring items.
#include <time.h>
#include <string.h>
#include <sstream>
#include <stdio.h>
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
using namespace std;

namespace ufmt {
  namespace v12 {
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
    ::ufmt::CRingScalerItem(numScalers)
  {
    v12::pScalerItem  pItem = reinterpret_cast<v12::pScalerItem>(getItemPointer());
    pItem->s_header.s_type = v12::PERIODIC_SCALERS;
    pItem->s_body.u_noBodyHeader.s_empty = sizeof(uint32_t);
    
    v12::pScalerItemBody pBody = &(pItem->s_body.u_noBodyHeader.s_body);
    pBody->s_intervalStartOffset = 0;
    pBody->s_intervalEndOffset   = 0;
    pBody->s_timestamp           = time(nullptr);
    pBody->s_intervalDivisor     = 1;
    pBody->s_isIncremental       = true;
    pBody->s_originalSid         = 0;
    pBody->s_scalerCount         = numScalers;
    
    uint32_t* p                  = pBody->s_scalers;
    memset(p, 0, numScalers*sizeof(uint32_t));
    p += numScalers;
    
    setBodyCursor(p);
    updateSize();
  }
  /*!
    Construct a scaler item with all the info specified.
    \param startTime - incremental scaler interval start time.
    \param endTime   - incremental scaler interval end time
    \param timestamp - Absolute system time at which the item was created.
    \param scalers   - Vector of scaler values.
    \param isIncremental - value of the incremental flag (defaults to true).
    \param timestampDivisor - Value of the timestamp divisor - defaults to 1.


  */
  CRingScalerItem::CRingScalerItem(uint32_t startTime,
          uint32_t stopTime,
          time_t   timestamp,
          std::vector<uint32_t> scalers,
                                  bool isIncremental,
                                  uint32_t timeOffsetDivisor) :
    CRingScalerItem(scalers.size())
  {
      v12::pScalerItem  pItem = reinterpret_cast<v12::pScalerItem>(getItemPointer());
      v12::pScalerItemBody pBody = &(pItem->s_body.u_noBodyHeader.s_body);
      
      pBody->s_intervalStartOffset = startTime;
      pBody->s_intervalEndOffset   = stopTime;
      pBody->s_timestamp           = timestamp;
      pBody->s_intervalDivisor     = timeOffsetDivisor;
      pBody->s_scalerCount         = scalers.size();
      pBody->s_isIncremental       = isIncremental ? 1 : 0;
      pBody->s_originalSid          = 0;
      uint32_t* p = pBody->s_scalers;
      memcpy(p, scalers.data(), scalers.size()*sizeof(uint32_t));
      p += scalers.size();
      setBodyCursor(p);
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
      CRingScalerItem(scalers.size())
  {
      v12::pScalerItem pItem =
        reinterpret_cast<v12::pScalerItem>(getItemPointer());
      
      pItem->s_header.s_type = v12::PERIODIC_SCALERS;
      pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_size = sizeof(v12::BodyHeader);
      pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_timestamp = eventTimestamp;
      pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_sourceId = source;
      pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_barrier  = barrier;
      
      v12::pScalerItemBody pBody = &(pItem->s_body.u_hasBodyHeader.s_body);
      pBody->s_intervalStartOffset = startTime;
      pBody->s_intervalEndOffset   = stopTime;
      pBody->s_timestamp           = timestamp;
      pBody->s_intervalDivisor     = timeDivisor;
      pBody->s_scalerCount         = scalers.size();
      pBody->s_isIncremental       = incremental ? 1 : 0;
      pBody->s_originalSid          = source;
      uint32_t* p = pBody->s_scalers;
      memcpy(p, scalers.data(), scalers.size()*sizeof(uint32_t));
      p += scalers.size();
      setBodyCursor(p);
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
      v12::pScalerItemBody pScalers =
        reinterpret_cast<v12::pScalerItemBody>(getBodyPointer());
      
      pScalers->s_intervalStartOffset = startTime;
  }
  /*!
    \return uint32_t
    \retval the current interval start time.
  */
  uint32_t
  CRingScalerItem::getStartTime() const
  {
      const v12::ScalerItemBody* pScalers =
        reinterpret_cast<const v12::ScalerItemBody*>(getBodyPointer());

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
      const v12::ScalerItemBody* pScalers =
        reinterpret_cast<const ScalerItemBody*>(getBodyPointer());
      float start   = pScalers->s_intervalStartOffset;
      float divisor = pScalers->s_intervalDivisor;
      return start/divisor;
  }

  /*!
    Set the interval ent time
    \pram endTime - the new interval end time.
  */
  void
  CRingScalerItem::setEndTime(uint32_t endTime) 
  {
      v12::pScalerItemBody pScalers =
        reinterpret_cast<v12::pScalerItemBody>(getBodyPointer());
      pScalers->s_intervalEndOffset = endTime;
  }
  /*!
    \return uint32_t
    \retval The current interval end time.
  */
  uint32_t
  CRingScalerItem::getEndTime() const
  {
      const v12::ScalerItemBody* pScalers =
        reinterpret_cast<const v12::ScalerItemBody*>(getBodyPointer());
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
      const v12::ScalerItemBody* pScalers =
        reinterpret_cast<const v12::ScalerItemBody*>(getBodyPointer());
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
      const v12::ScalerItemBody* pScalers =
        reinterpret_cast<const v12::ScalerItemBody*>(getBodyPointer());
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
      const v12::ScalerItemBody* pScalers =
        reinterpret_cast<const v12::ScalerItemBody*>(getBodyPointer());
      return (pScalers->s_isIncremental != 0);
  }

  /*!
    set the absolute timestamp of the event.

    \param stamp - the time stampe.
  */
  void
  CRingScalerItem::setTimestamp(time_t  stamp)
  {
      v12::pScalerItemBody pScalers =
        reinterpret_cast<v12::pScalerItemBody>(getBodyPointer());
      pScalers->s_timestamp = stamp;
  }
  /*!
    \return time_t
    \retval the current timestamp.
  */
  time_t
  CRingScalerItem::getTimestamp() const
  {
      const ScalerItemBody* pScalers =
        reinterpret_cast<const ScalerItemBody*>(getBodyPointer());
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
      v12::pScalerItemBody pScalers =
        reinterpret_cast<v12::pScalerItemBody>(getBodyPointer());

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
      const v12::ScalerItemBody* pScalers =
        reinterpret_cast<const v12::ScalerItemBody*>(getBodyPointer());
    
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
      const v12::ScalerItemBody* pScalers =
        reinterpret_cast<const v12::ScalerItemBody*>(getBodyPointer());

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
      const v12::ScalerItemBody* pScalers =
        reinterpret_cast<const v12::ScalerItemBody*>(getBodyPointer());

      return pScalers->s_scalerCount;
  }
  /**
   * getOriginalSourceId
   *    Return the source id that was originally used to construt the item
   *    when it was emitted.  Glom is allowed to rewrite the body header source id
   *    this allows us to retain information about which data source actually produced
   *    the source.
   * @return uint32_t
   */
  uint32_t
  CRingScalerItem::getOriginalSourceId() const
  {
    const v12::ScalerItemBody* pScalers =
      reinterpret_cast<const v12::ScalerItemBody*>(getBodyPointer());
    return pScalers->s_originalSid;
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
   * a word about the wonky casts in this section of code.
   * We derive from abstract's CRingScalerItem which,
   * ultimately derives from abstract CRingItem
   * which does not know how to do what we want
   * done here.  This derivation allows the abstract
   * items to define an interface which is exported
   * polymorphically for all data formats.
   * The casts below work because the actual underlying
   * _data_ is that of a v12::CRingItem which knows
   * how to execute these operations.
   * This is DRY but a bit dirty.
   * Take a shower after reading this code.
   */

  /**
   * getBodySize
   *    @return size_t - size of the body.
   */
  size_t
  CRingScalerItem::getBodySize() const
  {
    const v12::CRingItem* pThis = reinterpret_cast<const v12::CRingItem*>(this);
    return pThis->v12::CRingItem::getBodySize();
  }
  /**
   * getBodyPointer
   *     @return [const] void* - Pointer to the body pointer
   */
  const void*
  CRingScalerItem::getBodyPointer() const
  {
      const v12::CRingItem* pThis = reinterpret_cast<const v12::CRingItem*>(this);
      return pThis->v12::CRingItem::getBodyPointer();
  }
  void*
  CRingScalerItem::getBodyPointer()
  {
      v12::CRingItem* pThis = reinterpret_cast<v12::CRingItem*>(this);
      return pThis->v12::CRingItem::getBodyPointer();
  }
  /**
   * hasBodyHeader
   *    @return bool - true if the item has a body header.
   */
  bool
  CRingScalerItem::hasBodyHeader() const
  {
    const v12::CRingItem* pThis =
          reinterpret_cast<const v12::CRingItem*>(this);
      return pThis->v12::CRingItem::hasBodyHeader();
    
  }
  /**
   * getBodyHeader
   *    @return void* Pointer to the body header.
   *    @retval nullptr if the item has no body header.
   *    
   */
  void*
  CRingScalerItem::getBodyHeader() const
  {
    const v12::CRingItem* pThis =
          reinterpret_cast<const v12::CRingItem*>(this);
    return pThis->v12::CRingItem::getBodyHeader();
  }
  /**
   * setBodyHeader
   *   Set the contents of the item's body header.
   *   If there's no body header, space is created for one.
   * @param timestamp - the event timestamp.
   * @param sourceId  - the fragment's source id.
   * @param barrierType - the fragment's barrier type code.
   */
  void
  CRingScalerItem::setBodyHeader(
    uint64_t timestamp, uint32_t sourceId,
    uint32_t barrierType
  )
  {
    v12::CRingItem* pThis = reinterpret_cast<v12::CRingItem*>(this);
    pThis->v12::CRingItem::setBodyHeader(
        timestamp, sourceId, barrierType
    );
  }
  /**
   * getEventTimestamp
   *    Get the event timestamp from the body header.
   *  @return uint64_t
   *  @throw std::logic_error if there is no body header in this item.
   */
  uint64_t
  CRingScalerItem::getEventTimestamp() const
  {
    const v12::CRingItem* pThis =
        reinterpret_cast<const v12::CRingItem*>(this);
    return pThis->v12::CRingItem::getEventTimestamp();
  }
  /**
   * getSourceId
   *    Fetch the source id of the fragment from its body header.
   *  @return uint32_t
   *  @throw std::logic_error if there is no body header in this item.
   */
  uint32_t
  CRingScalerItem::getSourceId() const
  {
    const v12::CRingItem* pThis =
        reinterpret_cast<const v12::CRingItem*>(this);
    return pThis->v12::CRingItem::getSourceId();
  }
  /**
   * getBarrierType
   *   Fetch the barrier type code from the body header.
   *  @return uint32_t
   *  @throw std::logic_error if the item has no body header.
   */
  uint32_t
  CRingScalerItem::getBarrierType() const
  {
    const v12::CRingItem* pThis =
      reinterpret_cast<const v12::CRingItem*>(this);
    return pThis->v12::CRingItem::getBarrierType();
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
      const v12::ScalerItemBody* pScalers =
        reinterpret_cast<const v12::ScalerItemBody*>(getBodyPointer());

      if (channel >= getScalerCount()) {
        std::stringstream msg;
        msg << message
          << " : Requested scaler " << channel << " but there are only "
          << getScalerCount() << " scalers in the item";
        std::string m = msg.str();
        throw std::logic_error(m);
      }
  }

  }             // v12 namespace.
}