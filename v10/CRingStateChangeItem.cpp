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



#include "CRingStateChangeItem.h"
#include "DataFormat.h"

#include <sstream>
#include <string.h>
#include <stdexcept>


using namespace std;
namespace v10 {
////////////////////////////////////////////////////////////////////////////
//
// Constructors and canonicals.
//

/*!
   Construct a bare bone state change buffer.  The run number, time offset
   are both set to zero.  The timestamp is set to 'now'.
   The title is set to an emtpy string.
   \param reason - Reason for the state change.  This defaults to BEGIN_RUN.
*/
CRingStateChangeItem::CRingStateChangeItem(uint16_t reason) :
  ::CRingStateChangeItem(reason)

{
  init(reason);

  // Fill in the body:

  v10::pStateChangeItem  p =
    reinterpret_cast<v10::pStateChangeItem>(getItemPointer());
  p->s_runNumber    = 0;
  p->s_timeOffset   = 0;
  p->s_Timestamp = static_cast<uint32_t>(time(NULL));
  memset(p->s_title, 0, v10::TITLE_MAXSIZE+1);
}
/*!
   Fully specified construction the initial values of the various
   fields are specified by the constructor parameters. 

   \param reason     - Why the state change buffer is being emitted (the item type).
   \param runNumber  - Number of the run that is undegoing transitino.
   \param timeOffset - Number of seconds into the run at which this is being emitted.
   \param timestamp  - Absolute time to be recorded in the buffer.. tyically
                       this should be time(NULL).
   \param title      - Title string.  The length of this string must be at most
                       TITLE_MAXSIZE.

   \throw CRangeError - If the title string can't fit in s_title.
*/
CRingStateChangeItem::CRingStateChangeItem(uint16_t reason,
					   uint32_t runNumber,
					   uint32_t timeOffset,
					   time_t   timestamp,
					   std::string title)  :
  CRingStateChangeItem(reason)
  

{
  init(reason);

  // Everything should work just fine now:

  v10::pStateChangeItem  p =
    reinterpret_cast<v10::pStateChangeItem>(getItemPointer());
  p->s_runNumber = runNumber;
  p->s_timeOffset= timeOffset;
  p->s_Timestamp = timestamp;
  setTitle(title);		// takes care of the exception.


}
/*!
  Destruction just chains to the base class.

*/
CRingStateChangeItem::~CRingStateChangeItem()
{
}



///////////////////////////////////////////////////////////////////////////////
//
// Accessor methods.

/*!
   Set the run number for the item.
   \param run - new run number.
*/
void
CRingStateChangeItem::setRunNumber(uint32_t run)
{
  v10::pStateChangeItem  p =
    reinterpret_cast<v10::pStateChangeItem>(getItemPointer());
  p->s_runNumber = run;
}
/*!
  \return uint32_t
  \retval the item's current run number.
*/
uint32_t
CRingStateChangeItem::getRunNumber() const
{
  const v10::StateChangeItem* p =
    reinterpret_cast<const v10::StateChangeItem*>(getItemPointer());
  return p->s_runNumber;
}

/*!
   Set the elapsed run time field for the item.
   \param offset - seconds into the run.
*/
void
CRingStateChangeItem::setElapsedTime(uint32_t offset)
{
  v10::pStateChangeItem  p =
    reinterpret_cast<v10::pStateChangeItem>(getItemPointer());
  p->s_timeOffset = offset;
}
/*!
  \return uint32_t
  \retval time offset field of the item.
*/
uint32_t
CRingStateChangeItem::getElapsedTime() const
{
  const v10::StateChangeItem* p =
    reinterpret_cast<const v10::StateChangeItem*>(getItemPointer());
  return p->s_timeOffset;
}

/**
 * getTimeDivisor
 *  in v10 this is always 1:
 * @return uint32_t
 */
uint32_t
CRingStateChangeItem::getTimeDivisor() const
{
  return 1;
}
/**
 * computeElapsedTime
 *    This is the same as getElapsed time in v10:
 *  @return float
 */
float
CRingStateChangeItem::computeElapsedTime() const
{
  return float(getElapsedTime());
}


/*!
  Set the title string.
  \param title - new title string.
  \throw CRangeError - if the title string is too long to fit.
*/
void
CRingStateChangeItem::setTitle(string title) 
{
  // Ensure the title is small enough.

  if(title.size() > TITLE_MAXSIZE) {
    std::stringstream msg;
    msg << "The title: '" << title << "' is more than " << TITLE_MAXSIZE
      << " bytes long";
    std::string strMsg = msg.str();
    throw std::range_error(strMsg);
    
  }
  v10::pStateChangeItem  p =
    reinterpret_cast<v10::pStateChangeItem>(getItemPointer());
  strcpy(p->s_title, title.c_str());
}

/*!
    \return std::string
    \retval the title string of the item.
*/
string
CRingStateChangeItem::getTitle() const
{
  const v10::StateChangeItem* p =
    reinterpret_cast<const v10::StateChangeItem*>(getItemPointer());
  return string(p->s_title);
}
/*!
   Set the timestamp
   \param stamp  - new timestamp.
*/
void
CRingStateChangeItem::setTimestamp(time_t stamp)
{
  v10::pStateChangeItem  p =
    reinterpret_cast<v10::pStateChangeItem>(getItemPointer());
  p->s_Timestamp  = stamp;
}
/*!
    \return time_t
    \retval timestamp giving absolute time of the item.
*/
time_t
CRingStateChangeItem::getTimestamp() const
{
  const v10::StateChangeItem* p =
    reinterpret_cast<const v10::StateChangeItem*>(getItemPointer());
  return p->s_Timestamp;
}
/**
 * getOriginalSourceId
 * @return uint32_t
 * @retval 0 - v 10 has no original source id.
 */
uint32_t
CRingStateChangeItem::getOriginalSourceId() const
{
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// Virtual method overrides:

/**
 * getBodyHeader
 * @return void*
 * @retval nullptr - v10 items don't have body headers.
 */
void*
CRingStateChangeItem::getBodyHeader() const
{
  return nullptr;
}
/**
 * setBodyHeader
 *   No op
 */
void
CRingStateChangeItem::setBodyHeader(uint64_t timestamp, uint32_t sourceId,
                         uint32_t barrierType)
{}
/**
 * typeName
 *
 *  Stringify the type of the item.
 *
 * @return std::string - the item type
 */
std::string
CRingStateChangeItem::typeName() const
{
  switch (type()) {
  case v10::BEGIN_RUN:
    return " Begin Run ";
  case v10::END_RUN:
    return " End Run ";
  case v10::PAUSE_RUN:
    return " Pause Run ";
  case v10::RESUME_RUN:
    return " Resume Run ";
  default:
    return "Unrecognized type for v10 state change item";
  }
}
/**
 * toString
 *
 * Returns a string that is the ascified body of the item.
 *
 * @return std::string - ascified version of the item.
 */
std::string
CRingStateChangeItem::toString() const
{
  std::ostringstream out;		//  Build up via outputting to this psuedo stream.

  uint32_t run       = getRunNumber();
  uint32_t elapsed   = getElapsedTime();
  string   title     = getTitle();
  time_t   t         = getTimestamp();
  string   timestamp = ctime(&t);

  out <<  timestamp << " : Run State change : " << typeName();
  out << " at " << elapsed << " seconds into the run\n";
  out << "Title     : " << title << std::endl;
  out << "Run Number: " << run   << endl;


  return out.str();
}
    
///////////////////////////////////////////////////////////////////////////////
// Private utilities.

/* 
 *  Initialize member data construction time.
 */
void 
CRingStateChangeItem::init(uint32_t type)
{
  pStateChangeItem p = reinterpret_cast<pStateChangeItem>(getItemPointer());
  p->s_header.s_type = type;
  p ++;
  setBodyCursor(p);
  updateSize();
  
}

}