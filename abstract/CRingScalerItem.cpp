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
#include "DataFormat.h"                // abstract
#include <time.h>
#include <string.h>
#include <sstream>
#include <stdio.h>
#include <stdexcept>
#include <sstream>



using namespace std;

using namespace ufmt {
  uint32_t CRingScalerItem::m_ScalerFormatMask(0xffffffff); // by default scalers are 32 bits wide.

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
    CRingItem(PERIODIC_SCALERS,
        bodySize(numScalers))
  {
    init(numScalers);
    std::vector<uint32_t> dummy(numScalers, 0);
    
    
    fillScalerBody( 0, 0, 1, time(nullptr),  true, dummy, 0);
  }
  /*!
    Construct a scaler item with all the info specified.
    \param startTime - incremental scaler interval start time.
    \param endTime   - incremental scaler interval end time
    \param timestamp - Absolute system time at which the item was created.
    \param scalers   - Vector of scaler values.
    \param isIncremental - value of the incremental flag (defaults to true).
    @param sid       - original sourceid.
    \param timestampDivisor - Value of the timestamp divisor - defaults to 1.
    

  */
  CRingScalerItem::CRingScalerItem(uint32_t startTime,
          uint32_t stopTime,
          time_t   timestamp,
          std::vector<uint32_t> scalers,
          bool isIncremental, uint32_t sid,
          uint32_t timeOffsetDivisor) :
    CRingItem(PERIODIC_SCALERS, bodySize(scalers.size()))
  {
    init(scalers.size());
    fillScalerBody(
        startTime, stopTime, timeOffsetDivisor, timestamp,
        isIncremental, scalers, sid
    );

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
      pScalerItemBody pScalers = reinterpret_cast<pScalerItemBody>(getBodyPointer());
      
      pScalers->s_intervalStartOffset = startTime;
  }
  /*!
    \return uint32_t
    \retval the current interval start time.
  */
  uint32_t
  CRingScalerItem::getStartTime() const
  {
      const ScalerItemBody* pScalers =
        reinterpret_cast<const ScalerItemBody*>(getBodyPointer());

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
      const ScalerItemBody* pScalers =
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
      pScalerItemBody pScalers = reinterpret_cast<pScalerItemBody>(getBodyPointer());
      pScalers->s_intervalEndOffset = endTime;
  }
  /*!
    \return uint32_t
    \retval The current interval end time.
  */
  uint32_t
  CRingScalerItem::getEndTime() const
  {
      const ScalerItemBody* pScalers =
        reinterpret_cast<const ScalerItemBody*>(getBodyPointer());
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
      const ScalerItemBody* pScalers =
        reinterpret_cast<const ScalerItemBody*>(getBodyPointer());
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
      const ScalerItemBody* pScalers =
        reinterpret_cast<const ScalerItemBody*>(getBodyPointer());
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
      const ScalerItemBody* pScalers =
        reinterpret_cast<const ScalerItemBody*>(getBodyPointer());
      return (pScalers->s_isIncremental != 0);
  }

  /*!
    set the absolute timestamp of the event.

    \param stamp - the time stampe.
  */
  void
  CRingScalerItem::setTimestamp(time_t  stamp)
  {
      pScalerItemBody pScalers = reinterpret_cast<pScalerItemBody>(getBodyPointer());
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
    \throw std::out_of_range if channel is too big.
  */
  void
  CRingScalerItem::setScaler(uint32_t channel, uint32_t value) 
  {
      throwIfInvalidChannel(channel, "Attempting to set a scaler value");
      pScalerItemBody pScalers = reinterpret_cast<pScalerItemBody>(getBodyPointer());

      pScalers->s_scalers[channel] = value;
  }
  /*!

    \param channel - Number of the channel we want.
    \return uint32_t
    \retval Value of the selected scaler channel
    \throw std::out_of_range - if the channel number is out of range.
  */
  uint32_t
  CRingScalerItem::getScaler(uint32_t channel) const 
  {
      throwIfInvalidChannel(channel, "Attempting to get a scaler value");
      const ScalerItemBody* pScalers =
        reinterpret_cast<const ScalerItemBody*>(getBodyPointer());
    
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
      const ScalerItemBody* pScalers = reinterpret_cast<const ScalerItemBody*>(getBodyPointer());

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
      const ScalerItemBody* pScalers = reinterpret_cast<const ScalerItemBody*>(getBodyPointer());

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
    const ScalerItemBody* pScalers = reinterpret_cast<const ScalerItemBody*>(getBodyPointer());
    return pScalers->s_originalSid;
  }
  /////////////////////////////////////////////////////////////////////////////////////
  //
  // Private utilities


  /*
  * Do common initialization.. setting m_pScalers and the cursor.
  */
  void
  CRingScalerItem::init(size_t n)
  {
      pScalerItemBody pScalers = reinterpret_cast<pScalerItemBody>(getBodyPointer());
      size_t   size    = bodySize(n); 
      uint8_t* pCursor = reinterpret_cast<uint8_t*>(getBodyPointer());
      pCursor         += size;
      setBodyCursor(pCursor);
      updateSize();
      
  }
  /*-------------------------------------------------------
  ** Virtual method overrides
  *-------------------------------------------------------*/
  /**
   * getBodyHeader
   *    Throws std::logic_error the abstract version of this has
   *    no body header.
   */
  void*
  CRingScalerItem::getBodyHeader() const
  {
    throw std::logic_error("Abstract CRingScalerItem has no body header");
  }
  /**
   * setBodyHeader
   *    throws std::logic_error.
   */
  void
  CRingScalerItem::setBodyHeader(
        uint64_t timestamp, uint32_t sourceId,
        uint32_t barrierType
  )
  {
    throw std::logic_error("Abstract CRingScalerItem has no body header");
    
  }
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
    time_t tstamp = getTimestamp();
    string   time  = ctime(&tstamp);
    uint32_t sid   = getOriginalSourceId();
    vector<uint32_t> scalers = getScalers();
    for (int i =0; i < scalers.size(); i++) {
      scalers[i] = scalers[i] & m_ScalerFormatMask; // Mask off unused bits.
    }

    float   duration = end - start;

    out << time << " : Scalers (original Source Id " << sid << "):\n";
    out << "Interval start time: " << start << " end: " << end << " seconds in to the run\n\n";

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

  /*-------------------------------------------------------
  ** Private utilities:
  *---------------------------------------------------------
  /*
  *  Given the number of scalers determine the size of the body.
  */
  size_t
  CRingScalerItem::bodySize(size_t n)
  {
    size_t  size    = sizeof(ScalerItemBody) + n*sizeof(uint32_t);
    return  size;
  }
  /*
  ** Throws a range error if the channel number is invalid.
  */
  void
  CRingScalerItem::throwIfInvalidChannel(uint32_t channel,
                const char* message) const 
  {
      const ScalerItemBody* pScalers = reinterpret_cast<const ScalerItemBody*>(getBodyPointer());

      if (channel >= pScalers->s_scalerCount) {
        std::stringstream s;
        s << channel << " is out of the allowed range of [0.."
          << pScalers->s_scalerCount << ")";
        std::string msg = s.str();
        throw std::out_of_range(msg);
        
      }
  }
  /**
   * fillScalerBody
   *   Fill in the scaler body from the parameters.
   *   The body is assumed to already be pre-allocated in the
   *   ring item.
   *
   * @param startOffset - interval start time in run
   * @param endoffset   - Interval end time in run.
   * @param divisor     - Start/end offset divisors to support fractional
   *                      seconds.
   * @param timestamp   - Wall clock time.
   * @param incremental - Incremental flag.
   * @param scalers     - Vector of scaler counter values.
   * @param sid         - original source id.
   */
  void
  CRingScalerItem::fillScalerBody(
        uint32_t startOffset, uint32_t endOffset, uint32_t divisor,
        time_t timestamp, bool incremental,
        std::vector<uint32_t>& scalers, uint32_t sid
  )
  {
    pScalerItemBody pBody = reinterpret_cast<pScalerItemBody>(getBodyPointer());
    pBody->s_intervalStartOffset = startOffset;
    pBody->s_intervalEndOffset   = endOffset;
    pBody->s_timestamp           = timestamp;
    pBody->s_intervalDivisor     = divisor;
    pBody->s_scalerCount         = scalers.size();
    pBody->s_isIncremental       = incremental ? 1 : 0;
    pBody->s_originalSid         = sid;
    
    memcpy(pBody->s_scalers, scalers.data(), scalers.size()*sizeof(uint32_t));
  }
}