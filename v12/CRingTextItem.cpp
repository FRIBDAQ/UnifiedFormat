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


#include "CRingTextItem.h"
#include "DataFormat.h"
#include "CRingItem.h"

#include <string.h>
#include <sstream>
#include <stdexcept>

using namespace std;

namespace ufmt {
  namespace v12 {

  ///////////////////////////////////////////////////////////////////////////////////
  //
  //   Constructors and other canonical member functions.
  //

  /**
   * constructor
   *   @param type - Item type - must be valid.
   *   @param maxsize - maximum size of the item.
   */
  CRingTextItem::CRingTextItem(uint16_t type, size_t maxsize) :
    ::ufmt::CRingTextItem(type, maxsize)
  {
      if (!validType(type)) {
        throw std::logic_error("Invalid ring text item item type code");
      }
      v12::TextItem* pItem = reinterpret_cast<v12::TextItem*>(getItemPointer());
      pItem->s_header.s_type = type;
      pItem->s_body.u_noBodyHeader.s_empty = sizeof(uint32_t);
      
      v12::TextItemBody* pBody = &(pItem->s_body.u_noBodyHeader.s_body);
      std::vector<std::string> empty;
      void* pEnd = fillBody(pBody, 0, time(nullptr), 1, 0, empty);
      
      setBodyCursor(pEnd);
      updateSize();
  }

  /*!
    Construct a ring item that contains text strings.
    The item will have a timestamp of 'now' and an offset time of 0.
    \param type       - the ring item type. This should be 
                          PACKET_TYPES or MONITORED_VARIABLES.
    \param theStrings - the set of strings in the ring buffer item.

    @note The use of sizeof(Bodyheader) is fine because we're  building the
          we're building the  body header ourself.
  */
  CRingTextItem::CRingTextItem(uint16_t type, vector<string> theStrings) :
    ::ufmt::CRingTextItem(type, itemSize(theStrings))
  {
      if (!validType(type)) {
          throw std::logic_error("Invalid ring text item type code");
      }
      v12::TextItem* pItem = reinterpret_cast<v12::TextItem*>(getItemPointer());
      pItem->s_header.s_type = type;
      pItem->s_body.u_noBodyHeader.s_empty = sizeof(uint32_t);
      
      v12::TextItemBody* pBody = &(pItem->s_body.u_noBodyHeader.s_body);
      void* pEnd = fillBody(pBody, 0, time(nullptr), 1, 0, theStrings);
      
      setBodyCursor(pEnd);
      updateSize();
    
  }
  /*!
    Construct a ring buffer, but this time provide actual values for the
    time offset and time stamp.
    \param type    - Type of ring item... see the previous constructor.
    \param strings - The strings to put in the buffer.
    \param offsetTime - The time in to the run at which this is being inserted.
    \param timestamp  - The absolute time when this is being created.


    @note The use of sizeof(Bodyheader) is fine because we're  building the
          we're building the  body header ourself.
  */
  CRingTextItem::CRingTextItem(uint16_t       type,
            vector<string> strings,
            uint32_t       offsetTime,
            time_t         timestamp, uint32_t divisor) :
    ::ufmt::CRingTextItem(type, itemSize(strings))
  {
    if(!validType(type)) {
      throw std::logic_error("Invalid text item type code");
    }
    v12::TextItem* pItem = reinterpret_cast<v12::TextItem*>(getItemPointer());
    pItem->s_header.s_type = type;
    
    
    pItem->s_body.u_noBodyHeader.s_empty = sizeof(uint32_t);
      
    v12::TextItemBody* pBody = &(pItem->s_body.u_noBodyHeader.s_body);
    void* pEnd = fillBody(pBody, offsetTime, timestamp, divisor, 0, strings);
    
    setBodyCursor(pEnd);
    updateSize();
  }
  /**
   * constructor
   * 
   * Construct a text item that has a body header as well as the strings and
   * timestamp.
   *
   * @param type           - Type of ring item.
   * @param eventTimestamp - Event clock time at which this occured.
   * @param source         - Id of the data source.
   * @param barrier        - Type of barrier (or 0 if not a barrier).;
    \param strings - The strings to put in the buffer.
    \param offsetTime - The time in to the run at which this is being inserted.
    \param timestamp  - The absolute time when this is being created.
    @param divisor    - offsetTime/divisor = seconds into the run (default: 1)
    
    
    @note The use of sizeof(Bodyheader) is fine because we're  building the
          we're building the  body header ourself.

    
  */
  CRingTextItem::CRingTextItem(
      uint16_t type, uint64_t eventTimestamp, uint32_t source, uint32_t barrier,
      std::vector<std::string> theStrings, uint32_t offsetTime, time_t timestamp,
      int divisor) :
    ::ufmt::CRingTextItem(type, itemSize(theStrings))
      
  {
      
    if(!validType(type)) {
      throw std::logic_error("Invalid text item type code");
    }
    v12::TextItem* pItem = reinterpret_cast<v12::TextItem*>(getItemPointer());
    pItem->s_header.s_type = type;
    
    
    pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_size = sizeof(v12::BodyHeader);
    pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_timestamp = eventTimestamp;
    pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_sourceId  = source;
    pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_barrier   = barrier;
      
    v12::TextItemBody* pBody = &(pItem->s_body.u_hasBodyHeader.s_body);
    void* pEnd = fillBody(pBody, offsetTime, timestamp, divisor, source, theStrings);
    
    setBodyCursor(pEnd);
    updateSize();
  }

  /*!
    Destructor just chains to base class.
  */
  CRingTextItem::~CRingTextItem()
  {}

  ///////////////////////////////////////////////////////////////////////////////////////
  //
  // accessors (both selectors and mutators).
  //

  /*!
      \return vector<string>
      \retval The strings that were put in the item unpacked into elements of the vector.
  */
  vector<string>
  CRingTextItem::getStrings() const
  {
    vector<string> result;
    const v12::TextItemBody* pItem =
      reinterpret_cast<const v12::TextItemBody* >(getBodyPointer());
    
    const char*     pNextString = pItem->s_strings;

    for (int i = 0; i < pItem->s_stringCount; i++) {
      string aString = pNextString;
      pNextString   += aString.size() + 1;  // +1 for the trailing null.

      result.push_back(aString);

    }

    return result;
  }
  /*!
    Modify the buffered value of the run time offset.  This may be done if you use the
    simplified constuctor and only later figure out what the run time offset actually is.
    \param offset
  */
  void
  CRingTextItem::setTimeOffset(uint32_t offset)
  {
    v12::pTextItemBody pItem =
        reinterpret_cast<v12::pTextItemBody>(getBodyPointer());    
    pItem->s_timeOffset = offset;
  }
  /*!
    \return uint32_t
    \retval the time offset value.
  */
  uint32_t
  CRingTextItem::getTimeOffset() const
  {
    const v12::TextItemBody* pItem =
        reinterpret_cast<const v12::TextItemBody*>(getBodyPointer());
    return pItem->s_timeOffset;
  }
  /**
   * computeElapsedTime
   *
   * Determin the floating point seconds into the run using the
   * time offset and the divisor.
   *
   * @return float
   */
  float
  CRingTextItem::computeElapsedTime() const
  {
      
      float time   = getTimeOffset();
      float divisor= getTimeDivisor();
      
      return time/divisor;
  }
  /**
   * @return the time offset divisor offset/divisor in float gives seconds.
   */
  uint32_t
  CRingTextItem::getTimeDivisor() const
  {
    const v12::TextItemBody* pItem =
      reinterpret_cast<const v12::TextItemBody*>(getBodyPointer());
    return pItem->s_offsetDivisor;
  }
  /*!
    Set a new value for the timestamp of the item.
  */
  void
  CRingTextItem::setTimestamp(time_t stamp)
  {
    v12::pTextItemBody pItem = reinterpret_cast<v12::pTextItemBody>(getBodyPointer());
    pItem->s_timestamp = stamp;
  }
  /*!
    \return time_t
    \retval absolute timestamp from the buffer.

  */
  time_t
  CRingTextItem::getTimestamp() const
  {
    const v12::TextItemBody* pItem =
      reinterpret_cast<const v12::TextItemBody*>(getBodyPointer());
    return pItem->s_timestamp;
  }
  /**
   * getOriginalSourceId
   *    Returns the sourceId that was used to construct this item.
   *    Note that the body header can be rewritten by glom. This method
   *    returns the source id that was saved in the body of the item.
   * @return uint32_t
   */
  uint32_t
  CRingTextItem::getOriginalSourceId() const
  {
    const v12::TextItemBody* pItem =
      reinterpret_cast<const v12::TextItemBody*>(getBodyPointer());
    return pItem->s_originalSid;
  }
  ///////////////////////////////////////////////////////////
  //
  // Virtual method implementations.

  /**
   * typeName
   *
   *  return the stringified item type name.
   *
   * @return std::string - name of item-type.
   */
  std::string
  CRingTextItem::typeName() const
  {
    if(type() == PACKET_TYPES) {
      return std::string("Packet types");
    } else if (type() == MONITORED_VARIABLES) {
      return std::string("Monitored Variables");
    } else {
      throw std::string("CRingTextItem::typeName - Invalid type!");
    }
  }
  

  // Subsequent methods use our sideways cast to v12::CRingItem.. While
  // this class is not in our inheritance hierarchy (and to put it there will result
  // in an inheritance diamond of death leading to ::CRingItem), the data structures
  // used to store our ring item and our mutual derivation from ::CRingItem which
  // provides services needed to get at stuff we really need.


  /**
   * getBodyHeader
   *    @return void* - pointer to the items' body header.
   *    @retval nullptr - the item has no body header (see hasbodyheader).
   */
  void*
  CRingTextItem::getBodyHeader() const
  {
      const v12::CRingItem* pThis = reinterpret_cast<const v12::CRingItem*>(this);
      return pThis->v12::CRingItem::getBodyHeader();
  }
  /**
   * setBodyHeader
   *    @param timestamp - fragment timestamp to put in the body header.
   *    @param source    - fragment source id to put in the body header.
   *    @param barrierType - barrier type codee to put in the body header.
   *    @note if the underlying ring item does not yet have a body header,
   *          data will be slid down to make room for one and that will be filled in.
   */
  void
  CRingTextItem::setBodyHeader(uint64_t timestamp, uint32_t source , uint32_t barrierType)
  {
      v12::CRingItem* pThis = reinterpret_cast<v12::CRingItem*>(this);
      pThis->v12::CRingItem::setBodyHeader(timestamp, source, barrierType);
  }
  /**
   * getBodySize
   *   @return size_t number of bytes in the item body.
   */
  size_t
  CRingTextItem::getBodySize() const
  {
    const v12::CRingItem* pThis = reinterpret_cast<const v12::CRingItem*>(this);
    return pThis->v12::CRingItem::getBodySize();
  }
  /** getBodyPointer
   *  @return [const] void* - pointer to the body.
   */
  const void*
  CRingTextItem::getBodyPointer() const
  {
    const v12::CRingItem* pThis = reinterpret_cast<const v12::CRingItem*>(this);
    return pThis->v12::CRingItem::getBodyPointer();
  }
  void*
  CRingTextItem::getBodyPointer()
  {
    v12::CRingItem* pThis = reinterpret_cast<v12::CRingItem*>(this);
    return pThis->v12::CRingItem::getBodyPointer();
  }
  /**
   * hasBodyHeader
   *   @return bool
   *   @retval false - item has no body header.
   */
  bool
  CRingTextItem::hasBodyHeader() const
  {
    const v12::CRingItem* pThis = reinterpret_cast<const v12::CRingItem*>(this);
    return pThis->v12::CRingItem::hasBodyHeader();
  }
  /**
   * getEventTimestamp
   *    @return uint64_t fragment timestamp.
   *    @throw std::logic_error - no body header in item.
   */
  uint64_t
  CRingTextItem::getEventTimestamp() const
  {
    const v12::CRingItem* pThis = reinterpret_cast<const v12::CRingItem*>(this);
    return pThis->v12::CRingItem::getEventTimestamp();
  }
  /**
   * getSourceId
   *    @return uint32_t - source id
   *    @throw std::logic_error - no body header in item.
   */
  uint32_t
  CRingTextItem::getSourceId() const
  {
    const v12::CRingItem* pThis = reinterpret_cast<const v12::CRingItem*>(this);
    return pThis->v12::CRingItem::getSourceId();
  }
  /**
   * getBarrierType
   *   @return uint32_t barrier type.
   *   @throw std::logic_error - no body header.
   */
  uint32_t
  CRingTextItem::getBarrierType() const
  {
    const v12::CRingItem* pThis = reinterpret_cast<const v12::CRingItem*>(this);
    return pThis->v12::CRingItem::getBarrierType();
  }

  //////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Private utilities.
  //

  /*
  **  Compute the size of the body of a text item buffer.  This is the sum of the sizes of the string
  **  +1 for each string to allow for the null termination byte, + sizeof(TextItem) 
  **   - sizeof(RingItemHeader) - sizeof(char) 
  ** (this last - sizeof(char) is the dummy s_strings[] array size.
  */
  size_t 
  CRingTextItem::bodySize(vector<string> strings) const
  {
    size_t result = sizeof(v12::TextItemBody) ;
    for (int i=0; i < strings.size(); i++) {
      result += strings[i].size() + 1;
    }
    return result;
  }
  /*
  **  Returns true if the type of the current item is a valid text item.
  **
  */
  bool
  CRingTextItem::validType(uint16_t t) const
  {
    
    return ((t == v12::PACKET_TYPES)               ||
      (t == v12::MONITORED_VARIABLES));
  }
  /**
   * itemSize
   *   Given a set of strings determines an uppper bound on the size of the
   *   string item needed to hold it.
   * @param strings - vector of strings to put in the body.
   * @return size_t - note this is an upper bound.
   *
   */
  size_t
  CRingTextItem::itemSize(const std::vector<std::string>& strings) const
  {
    size_t result = bodySize(strings);
    result += sizeof(v12::RingItemHeader) + sizeof(v12::BodyHeader);
    return result;
  }
  /**
   * fillBody
   *    Fills a text item body:
   * @param pBody - pointer to the body.
   * @param offset - time offset.
   * @param stamp  - clock time stamp
   * @param divisor - time offset divisor.
   * @param osid   - original source id.
   * @param strings - strings to put in the payload.
   * @return void*  - pointer past the strings - suitable for e.g. setBodyCursor.
   */
  void*
  CRingTextItem::fillBody(
    void* pBody, unsigned offset, time_t stamp, unsigned divisor,
    unsigned osid, const std::vector<std::string>& strings
  )
  {
    v12::pTextItemBody p = reinterpret_cast<v12::pTextItemBody>(pBody);
    p->s_timeOffset = offset;
    p->s_timestamp  = stamp;
    p->s_stringCount  = strings.size();
    p->s_offsetDivisor = divisor;
    p->s_originalSid   = osid;
    
    char* pDest = p->s_strings;
    for (int i =0; i < strings.size(); i++) {
      strcpy(pDest, strings[i].c_str());
      pDest += strings[i].size() + 1;   // +1 for null terminator.
    }
    
    return pDest;
    
  }


  }                               // v12 namespace.
}