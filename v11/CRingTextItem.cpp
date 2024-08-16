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
#include <time.h>
using namespace std;

namespace ufmt {
  namespace v11 {

  ///////////////////////////////////////////////////////////////////////////////////
  //
  //   Constructors and other canonical member functions.
  //


  /*!
    Construct a ring buffer, but this time provide actual values for the
    time offset and time stamp.
    \param type    - Type of ring item... see the previous constructor.
    \param strings - The strings to put in the buffer.
    \param offsetTime - The time in to the run at which this is being inserted.
    \param timestamp  - The absolute time when this is being created.
    @param divisor   - elapsed time divisor.
    @note This constructor produces an item without a body header.
  */
  CRingTextItem::CRingTextItem(uint16_t       type,
            vector<string> strings,
            uint32_t       offsetTime,
            time_t         timestamp,
            uint32_t       divisor) :
    ::ufmt::CRingTextItem(type, strings, offsetTime, timestamp, divisor)
  {
    
    v11::pTextItem pItem = reinterpret_cast<v11::pTextItem>(getItemPointer());
    pItem->s_header.s_type = type;
    if (!validType()) {
      throw std::invalid_argument("Not a valid v11::TextItem item type");
    }
    pItem->s_header.s_size = sizeof(v11::RingItemHeader) + sizeof(uint32_t)
      + sizeof(v11::TextItemBody) + sizeStrings(strings);
    pItem->s_body.u_noBodyHeader.s_mbz = 0;   // Makes getBodyPointer work.
    
    v11::pTextItemBody pBody =
      reinterpret_cast<v11::pTextItemBody>(getBodyPointer());
    pBody->s_timeOffset = offsetTime;
    pBody->s_timestamp = timestamp;
    pBody->s_stringCount = strings.size();
    pBody->s_offsetDivisor = divisor;
    copyStrings(pBody->s_strings, strings);
    
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
    @note This,of course creates an  item with a body header.
  */
  CRingTextItem::CRingTextItem(
      uint16_t type, uint64_t eventTimestamp, uint32_t source, uint32_t barrier,
      std::vector<std::string> theStrings, uint32_t offsetTime, time_t timestamp,
      int divisor)
    : ::ufmt::CRingTextItem(type, theStrings, offsetTime, timestamp, divisor
    )
  {
      v11::pTextItem pItem = reinterpret_cast<v11::pTextItem>(getItemPointer());
      pItem->s_header.s_type = type;
      if (!validType()) {
        throw std::invalid_argument("Not a valid v11::TextItem item type");
      }
      pItem->s_header.s_size =
        sizeof(v11::RingItemHeader) +
        sizeof(v11::BodyHeader) +
        sizeof(v11::TextItemBody)  + sizeStrings(theStrings);
      
      // getBodyHeader won't work yet.... 
        
      v11::pBodyHeader pBodyHdr = &(pItem->s_body.u_hasBodyHeader.s_bodyHeader);
      pBodyHdr->s_size = sizeof(v11::BodyHeader);
      pBodyHdr->s_timestamp = eventTimestamp;
      pBodyHdr->s_sourceId = source;
      pBodyHdr->s_barrier = barrier;
      
      // getBodyPointer will work now.
      
      v11::pTextItemBody pBody =
        reinterpret_cast<v11::pTextItemBody>(getBodyPointer());
      pBody->s_timeOffset = offsetTime;
      pBody->s_timestamp = timestamp;
      pBody->s_stringCount = theStrings.size();
      pBody->s_offsetDivisor = divisor;
      copyStrings(pBody->s_strings, theStrings);
      

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
    const v11::TextItemBody* pItem =
      reinterpret_cast<const v11::TextItemBody*>(getBodyPointer());
    
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
    v11::pTextItemBody pItem =
      reinterpret_cast<v11::pTextItemBody>(getBodyPointer());    
    pItem->s_timeOffset = offset;
  }
  /*!
    \return uint32_t
    \retval the time offset value.
  */
  uint32_t
  CRingTextItem::getTimeOffset() const
  {
    const v11::TextItemBody* pItem =
      reinterpret_cast<const v11::TextItemBody*>(getBodyPointer());
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
      const v11::TextItemBody* pItem =
        reinterpret_cast<const v11::TextItemBody*>(getBodyPointer());
      float time   = pItem->s_timeOffset;
      float divisor= pItem->s_offsetDivisor;
      
      return time/divisor;
  }
  /**
   * @return the time offset divisor offset/divisor in float gives seconds.
   */
  uint32_t
  CRingTextItem::getTimeDivisor() const
  {
    const v11::TextItemBody* pItem =
      reinterpret_cast<const v11::TextItemBody*>(getBodyPointer());
    return pItem->s_offsetDivisor;
  }
  /*!
    Set a new value for the timestamp of the item.
  */
  void
  CRingTextItem::setTimestamp(time_t stamp)
  {
    v11::pTextItemBody pItem = reinterpret_cast<v11::pTextItemBody>(getBodyPointer());
    pItem->s_timestamp = stamp;
  }
  /*!
    \return time_t
    \retval absolute timestamp from the buffer.

  */
  time_t
  CRingTextItem::getTimestamp() const
  {
    const v11::TextItemBody* pItem =
      reinterpret_cast<const v11::TextItemBody*>(getBodyPointer());
    return pItem->s_timestamp;
  }
  /**
   * getOriginalSourceId
   *   @return uint32_t - original source id. Note that since
   *        v11 items don't maintain an original source Id field,
   *        - items without a body header will give 0
   *        - items with a body header will just return the source id from
   *          that.
   */
  uint32_t
  CRingTextItem::getOriginalSourceId() const
  {
    const v11::BodyHeader* pBodyHdr =
      reinterpret_cast<const v11::BodyHeader*>(getBodyHeader());
    uint32_t result(0);
    if (pBodyHdr) {
      result = pBodyHdr->s_sourceId;
    }
    return result;
  }

  /**
   * getBodyPointer
   *   @return void* (or const void*)
   *     Pointer to the body header.
   *    @note this delegates to the v11::CRingItem class.
   *          in a rather dirty way.
   */
  void*
  CRingTextItem::getBodyPointer()
  {
      v11::CRingItem* pThis = reinterpret_cast<v11::CRingItem*>(this);
      return pThis->v11::CRingItem::getBodyPointer();
  }
  const void*
  CRingTextItem::getBodyPointer() const
  {
    const v11::CRingItem* pThis = reinterpret_cast<const v11::CRingItem*>(this);
      return pThis->v11::CRingItem::getBodyPointer();
  }

  bool
  CRingTextItem::hasBodyHeader() const
  {
    return getBodyHeader() != nullptr;
  }
  /**
   * getBodyHeader
   *    @return void* - pointer to the body header, nullptr if there isn't one.
   *    @note this delegates to the v11::CRingItem class.
   *          in a rather dirty way.
   */
  void*
  CRingTextItem::getBodyHeader() const
  {
    const v11::CRingItem* pThis = reinterpret_cast<const v11::CRingItem*>(this);
    return pThis->v11::CRingItem::getBodyHeader();
  }
  /**
   * setBodyHeader
   *   @param timestamp - event timestamp.
   *   @param sid       - Sourceid.
   *   @param barrierType - Barrier type code.
   */
  void
  CRingTextItem::setBodyHeader(
    uint64_t timestamp, uint32_t sid, uint32_t barrierType
  )
  {
    v11::CRingItem* pThis = reinterpret_cast<v11::CRingItem*>(this);
    pThis->v11::CRingItem::setBodyHeader(timestamp, sid, barrierType);
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
      throw std::runtime_error("CRingTextItem::typeName - Invalid type!");
    }
  }
  /**
   * toString
   *
   * Returns a stringified version of the item that can
   * be read by humans.
   *
   * @return std::string - stringified output.
   */
  std::string
  CRingTextItem::toString() const
  {
    std::ostringstream out;

    // uint32_t elapsed  = getTimeOffset();
    time_t t = getTimestamp();
    string   time     = ctime(&t);
    vector<string> strings = getStrings();

    out << time << " : Documentation item ";
    out << typeName();
    const v11::CRingItem* pThis = reinterpret_cast<const v11::CRingItem*>(this);
    out << pThis->v11::CRingItem::bodyHeaderToString();

    out << computeElapsedTime() << " seconds in to the run\n";
    for (int i = 0; i < strings.size(); i++) {
      out << strings[i] << endl;
    }


    return out.str();
  }

  //////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Private utilities.
  //
  /**
   * sizeStrings
   *    @param strings - vector of strings to size.
   *    @return size_t - Bytes of storage required to store in cz format.
  */
  size_t
  CRingTextItem::sizeStrings(const std::vector<std::string>& strings) const
  {
    size_t result = 0;
    for (int i =0; i < strings.size(); i++) {
      result += strings[i].size() + 1;
    }
    return result;
  }
  /*
  **  Returns true if the type of the current item is a valid text item.
  **
  */
  bool
  CRingTextItem::validType() const
  {
    uint32_t t = type();
    return ((t == PACKET_TYPES)               ||
      (t == MONITORED_VARIABLES));
  }
  /*
  ** Copies a set of strings into the item's 
  ** s_scalers region.   Each string is followed by a 
  ** null.  
  **   When done the cursor is updated to point past the item.
  **   When done, s_stringCount is updated to the number of strings.
  */
  void
  CRingTextItem::copyStrings(void* destination, const std::vector<std::string>& strings)
  {
    char* p = reinterpret_cast<char*>(destination);
    
    for (int i = 0; i < strings.size(); i++) {
      strcpy(p, strings[i].c_str());
      p += strlen(strings[i].c_str()) + 1;
    }
    setBodyCursor(p);
    updateSize();
  }


  }
}