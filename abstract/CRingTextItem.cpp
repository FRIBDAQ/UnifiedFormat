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
#include <string.h>
#include <sstream>
#include <stdexcept>
#include <time.h>
using namespace std;

///////////////////////////////////////////////////////////////////////////////////
//
//   Constructors and other canonical member functions.
//

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
  CRingItem(type, bodySize(theStrings) + sizeof(BodyHeader))
{

    auto stringPtrs = makeStringPointers(theStrings);
    void* p = fillTextItemBody(
     0, 1, time(nullptr), stringPtrs.size(), stringPtrs.data(),
      0
    );
    setBodyCursor(p);
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
			     time_t         timestamp) :
  CRingItem(type, bodySize(strings) + sizeof(BodyHeader))
{
  
  auto stringPtrs = makeStringPointers(strings);
  void* p = fillTextItemBody(
    offsetTime, 1, timestamp, stringPtrs.size(),
    stringPtrs.data(), 0
  );
  setBodyCursor(p);
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
  const TextItemBody* pItem = reinterpret_cast<const TextItemBody*>(getBodyPointer());
  
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
  pTextItemBody pItem = reinterpret_cast<pTextItemBody>(getBodyPointer());    
  pItem->s_timeOffset = offset;
}
/*!
   \return uint32_t
   \retval the time offset value.
*/
uint32_t
CRingTextItem::getTimeOffset() const
{
  const TextItemBody* pItem = reinterpret_cast<const TextItemBody*>(getBodyPointer());
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
    const TextItemBody* pItem = reinterpret_cast<const TextItemBody*>(getBodyPointer());
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
  const TextItemBody* pItem = reinterpret_cast<const TextItemBody*>(getBodyPointer());
  return pItem->s_offsetDivisor;
}
/*!
   Set a new value for the timestamp of the item.
*/
void
CRingTextItem::setTimestamp(time_t stamp)
{
  pTextItemBody pItem = reinterpret_cast<pTextItemBody>(getBodyPointer());
  pItem->s_timestamp = stamp;
}
/*!
   \return time_t
   \retval absolute timestamp from the buffer.

*/
time_t
CRingTextItem::getTimestamp() const
{
  const TextItemBody* pItem = reinterpret_cast<const TextItemBody*>(getBodyPointer());
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
  const TextItemBody* pItem = reinterpret_cast<const TextItemBody*>(getBodyPointer());
  return pItem->s_originalSid;
}
///////////////////////////////////////////////////////////
//
// Virtual method implementations.

/**
 * getBodyHeader
 *   @throw std::logic_error since there's not body header.
 */
void*
CRingTextItem::getBodyHeader() const
{
  throw std::logic_error("Abstract CRingTextItems don't have body headers");
}
/**
 *  same for attempts to set the body header.
 */
void
CRingTextItem::setBodyHeader(
  uint64_t timestamp, uint32_t sid, uint32_t barrierType
)
{
  throw std::logic_error("Abstract ring text items dont' have body headers");
}

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
  time_t ts = getTimestamp();
  string   time     = ctime(&ts);
  vector<string> strings = getStrings();
  uint32_t sid      = getOriginalSourceId();

  out << time << " : Documentation item ";
  out << typeName();
  out << "Originally emitted by source id: " << sid << " ";
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

/*
**  Compute the size of the body of a text item buffer.  This is the sum of the sizes of the string
**  +1 for each string to allow for the null termination byte, + sizeof(TextItem) 
**   - sizeof(RingItemHeader) - sizeof(char) 
** (this last - sizeof(char) is the dummy s_strings[] array size.
*/
size_t 
CRingTextItem::bodySize(vector<string> strings) const
{
  size_t result = sizeof(TextItemBody) - sizeof(char);
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
CRingTextItem::validType() const
{
  uint32_t t = type();
  return ((t == PACKET_TYPES)               ||
	  (t == MONITORED_VARIABLES));
}

/**
 * makeStringPointers
 *    Sets up for e.g. fillTextItemBody by producing a vector of
 *    pointers to the string data.  The data method of that vector can
 *    then provide the pointer to the array of string pointers needed by
 *    that call.
 *
 *  @param strings - Reference to the vector of strings we're getting pointers to
 *  @return  std::vector<const char*
 *  @note any operations on  original vector
 *        can invalidate the pointers.
 */
std::vector<const char*>
CRingTextItem::makeStringPointers(const std::vector<std::string>& strings)
{
  std::vector<const char*> result;
  
  for (int i =0; i < strings.size(); i++) {
    result.push_back(strings[i].c_str());
  }
  
  return result;
}
/**
 * fillTextItemBody
 *    Fill the body of a text item.
 * @param offset - offset into the run.
 * @param divisor - offset/divisor (floating) is seconds into run.
 * @param stamp  - Unix timestamp at which item was created.
 * @param nStrings - Number of strings.
 * @param ppStrings - Pointers to the strings.
 * @param sid     - Source id.
 * @return void*   - Pointer past the filled in item.
 * @throw std::out_of_range if the strings won't fit in the item.
 */
void*
CRingTextItem::fillTextItemBody(
    uint32_t offset, uint32_t divisor, time_t stamp, uint32_t nStrings,
    const char** ppStrings, int sid
)
{
    pRingItemHeader pHeader = reinterpret_cast<pRingItemHeader>(getItemPointer());
    pTextItemBody   pBody   = reinterpret_cast<pTextItemBody>(pHeader+1);
    
    // First fill in the fixed part:
    
    pBody->s_timeOffset = offset;
    pBody->s_timestamp  = stamp;
    pBody->s_stringCount = nStrings;
    pBody->s_offsetDivisor = divisor;
    pBody->s_originalSid   = sid;
    
    // Now the strings:
    
    size_t maxSize = getStorageSize();
    size_t remainingSize = maxSize - sizeof(RingItemHeader) - sizeof(TextItemBody);
    
    char* p = pBody->s_strings;
    for(int i =0; i < nStrings; i++) {
      auto size = strlen(*ppStrings)+1;
      if (size > remainingSize) {
        throw std::out_of_range(
            "CRingTextItem::fillTextItemBody: The string items won't fit in the ring item space!"
        );
      }
      strcpy(p, *ppStrings);
      ppStrings++;
      p += size;
      remainingSize -= size;
    }
    return p;
    
}