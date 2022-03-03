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
#include "DataFormat.h"
using namespace std;

namespace v10 {
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

*/
CRingTextItem::CRingTextItem(uint16_t type, vector<string> theStrings) :
  ::CRingTextItem(type, bodySize(theStrings))
{
  
  copyStrings(theStrings);

  v10::pTextItem p = reinterpret_cast<v10::pTextItem>(getItemPointer());
  p->s_timeOffset = 0;
  p->s_timestamp = static_cast<uint32_t>(time(NULL));
}
/*!
  Construct a ring buffer, but this time provide actual values for the
  time offset and time stamp.
  \param type    - Type of ring item... see the previous constructor.
  \param strings - The strings to put in the buffer.
  \param offsetTime - The time in to the run at which this is being inserted.
  \param timestamp  - The absolute time when this is being created.

*/
CRingTextItem::CRingTextItem(uint16_t       type,
			     vector<string> strings,
			     uint32_t       offsetTime,
			     time_t         timestamp, uint32_t divisor) :
  ::CRingTextItem(type, bodySize(strings))
{
  
  copyStrings(strings);

  v10::pTextItem p = reinterpret_cast<v10::pTextItem>(getItemPointer());
  p->s_timeOffset = offsetTime;
  p->s_timestamp  = timestamp;
  
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
  const v10::TextItem* p = reinterpret_cast<const v10::TextItem*>(getItemPointer());
  const char*     pNextString = p->s_strings;

  for (int i = 0; i < p->s_stringCount; i++) {
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
  v10::pTextItem p = reinterpret_cast<v10::pTextItem>(getItemPointer());
  p->s_timeOffset = offset;
}
/*!
   \return uint32_t
   \retval the time offset value.
*/
uint32_t
CRingTextItem::getTimeOffset() const
{
  const v10::TextItem* p = reinterpret_cast<const v10::TextItem*>(getItemPointer());
  return p->s_timeOffset;
}
/**
 * ComputeElapsedTime
 *     For v10, the divisor is always 1 so:
 * @return float
 */
float
CRingTextItem::computeElapsedTime() const
{
  return float(getTimeOffset());
}
uint32_t
CRingTextItem::getTimeDivisor() const
{
  return 1;
}

/*!
   Set a new value for the timestamp of the item.
*/
void
CRingTextItem::setTimestamp(time_t stamp)
{
  v10::pTextItem p = reinterpret_cast<v10::pTextItem>(getItemPointer());
  p->s_timestamp = stamp;
}
/*!
   \return time_t
   \retval absolute timestamp from the buffer.

*/
time_t
CRingTextItem::getTimestamp() const
{
  const v10::TextItem* p = reinterpret_cast<const v10::TextItem*>(getItemPointer());
  return p->s_timestamp;
}
/**
 * getOriginalSourceId
 */
uint32_t
CRingTextItem::getOriginalSourceId() const
{
  return 0;
}

///////////////////////////////////////////////////////////
//
// Virtual method implementations. Note: V10 items do not have body headers

/** 
*  getBodyHeader
*/
void*
CRingTextItem::getBodyHeader() const
{
    return nullptr;
}
/**
 * setBodyHeader
 */
void
CRingTextItem::setBodyHeader(
        uint64_t timestamp, uint32_t sid, uint32_t barrierType
)
{}
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
    return std::string(" Packet types: ");
  } else if (type() == MONITORED_VARIABLES) {
    return std::string(" Monitored Variables: ");
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

  uint32_t elapsed  = getTimeOffset();
  time_t t = getTimestamp();
  string   time     = ctime(&t);
  vector<string> strings = getStrings();

  out << time << " : Documentation item ";
  out << typeName();

  out << elapsed << " seconds in to the run\n";
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
  size_t result = sizeof(TextItem) - sizeof(RingItemHeader) - sizeof(char);
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
/*
** Copies a set of strings into the item's 
** s_scalers region.   Each string is followed by a 
** null.  
**   When done the cursor is updated to point past the item.
**   When done, s_stringCount is updated to the number of strings.
*/
void
CRingTextItem::copyStrings(vector<string> strings)
{
  v10::pTextItem pI = reinterpret_cast<v10::pTextItem>(getItemPointer());
  pI->s_stringCount = strings.size();
  char* p                = pI->s_strings;
  for (int i = 0; i < strings.size(); i++) {
    strcpy(p, strings[i].c_str());
    p += strings[i].size() + 1;
  }
  setBodyCursor(p);
  updateSize();
}

}                                            // v10 namespace.