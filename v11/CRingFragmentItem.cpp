/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2009.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#include "CRingFragmentItem.h"
#include "DataFormat.h"

#include <string.h>
#include <sstream>
#include <stdexcept> 

namespace v11 {
/*-------------------------------------------------------------------------------------
 *   Canonical methods
 */

/**
 * constructor 
 *
 *  Create a new ring fragmen from the data supplied:
 *
 * @param timestamp   - The event builder timestamp.
 * @param source      - Source id of fragment creator.
 * @param payloadSize - Size of the fragment payload.
 * @param pBody       - Pointer to payload.
 * @param barrier     - Barrier type (defaults to 0 which is not a barrier).
 */
CRingFragmentItem::CRingFragmentItem(uint64_t timestamp, uint32_t source, uint32_t payloadSize, 
				     const void* pBody, uint32_t barrier) :
  ::CRingFragmentItem(timestamp, source, payloadSize, pBody, barrier) 

{
  
  v11::pEventBuilderFragment pFrag =
    reinterpret_cast<v11::pEventBuilderFragment>(getItemPointer());
  v11::pBodyHeader pHeader = &(pFrag->s_bodyHeader);
  
	pFrag->s_header.s_type=  v11::EVB_FRAGMENT;             // since base class type may differe.
	
  pHeader->s_size        = sizeof(v11::BodyHeader);
  pHeader->s_timestamp   = timestamp;
  pHeader->s_sourceId    = source;
  pHeader->s_barrier = barrier;
	
	uint8_t* pPayload = reinterpret_cast<uint8_t*>(pHeader+1);
  memcpy(pPayload, pBody, payloadSize);
	pPayload += payloadSize;
	setBodyCursor(pPayload);
  updateSize();                         // sets pFrag->s_header.s_size.
  
}

/**
 * destructor
 *
 * Base class does everything we need:
 */
CRingFragmentItem::~CRingFragmentItem() {}

/*------------------------------------------------------------------------------
 * getters:  Note that by 'law', v11::EventBuilderFragment items always have a
 *           body header.
 */

/**
 * Return the current object's timestamp.
 *
 * @return uint64_t 
 */
uint64_t
CRingFragmentItem::timestamp() const
{
	const v11::BodyHeader* pH = reinterpret_cast<const v11::BodyHeader*>(getBodyHeader());
  return pH->s_timestamp;
}
/**
 * source
 *
 * Return the source id.
 *
 * @return uint32_t
 */
uint32_t 
CRingFragmentItem::source() const
{
	const v11::BodyHeader* pH = reinterpret_cast<const v11::BodyHeader*>(getBodyHeader());
  return pH->s_sourceId;
}
/**
 * return the size of thefragment payload
 * 
 * @return size_t
 */
size_t
CRingFragmentItem::payloadSize() const
{
  const v11::EventBuilderFragment* pItem =
    reinterpret_cast<const v11::EventBuilderFragment*>(getItemPointer());
    
  return pItem->s_header.s_size - sizeof(RingItemHeader) - sizeof(BodyHeader);

}
/**
 * Return a const pointer to the payload.
 *
 * @return const void*
 */
void*
CRingFragmentItem::payloadPointer()
{
  v11::pEventBuilderFragment pItem =
    reinterpret_cast<v11::pEventBuilderFragment>(getItemPointer());

  return pItem->s_body;  

}
/**
 * return the barrier type:
 *
 * @return int32_t
 */
uint32_t
CRingFragmentItem::barrierType() const
{
	const v11::BodyHeader* pH = reinterpret_cast<const v11::BodyHeader*>(getBodyHeader());
  return pH->s_barrier;
}
/**
 *  getBody Pointer - return pointers to the body of the item.
 * @return (const) void*
 */
const void*
CRingFragmentItem::getBodyPointer() const
{
	  auto p = reinterpret_cast<const v11::EventBuilderFragment*>(getItemPointer());
		return p->s_body;
}
void*
CRingFragmentItem::getBodyPointer()
{
	  auto p = reinterpret_cast<v11::EventBuilderFragment*>(getItemPointer());
		return p->s_body;
	
}
/**
 * typeName 
 *   Provide the type of the ringitem as a string.
 *
 * @return std::string "Event fragment ".
 */
std::string
CRingFragmentItem::typeName() const
{
  return "Event fragment";
}
/**
 * toString
 *
 *  Dumps the contents of a ring fragment.
 * @return std::string - the stringified version of the fragment.
 */
std::string
CRingFragmentItem::toString() const
{
  static const int perLine = 16;
  std::ostringstream out;
  
  v11::CRingFragmentItem* This = const_cast<v11::CRingFragmentItem*>(this);
  out << typeName() << ':' << std::endl;
  out << "Fragment timestamp:    " << timestamp()   << std::endl;
  out << "Source ID         :    " << source()      << std::endl;
  out << "Payload size      :    " << payloadSize() << std::endl;
  out << "Barrier type      :    " << barrierType() << std::endl;


  out << "- - - - - -  Payload - - - - - - -\n";
    out << std::hex << std::endl;
    const uint8_t* p = reinterpret_cast<const uint8_t*>(This->payloadPointer());
    for (int i = 0; i < This->payloadSize(); i++) {
      out << *p++ << ' ';
      if (((i % perLine) == 0) && (i != 0)) {
				out << std::endl;
      }
		}
    if (payloadSize() % perLine) {
      out << std::endl;		// if needed a trailing endl.
		}
    

  return out.str();
}
/**
 * hasBodyHeader
 *    v11 ring fragment items always have body headers:
 */
bool
CRingFragmentItem::hasBodyHeader() const
{
	return true;
}
/**
 * getBodyHeader
 *   @return void* - pointer to the body header.
 */
void*
CRingFragmentItem::getBodyHeader() const
{
	 const v11::EventBuilderFragment* pFrag =
				reinterpret_cast<const v11::EventBuilderFragment*>(getItemPointer());
	 return const_cast<v11::pBodyHeader>(&(pFrag->s_bodyHeader));
}
/**
 * setBodyHeader
 *     Override what's in the body header.  We know we have one so we don't need
 *     to do the slide thing:
 */
void
CRingFragmentItem::setBodyHeader(
	uint64_t timestamp, uint32_t sourceId,
  uint32_t barrierType
)
{
	v11::pBodyHeader p = reinterpret_cast<v11::pBodyHeader>(getBodyHeader());
	p->s_timestamp   = timestamp;
	p->s_sourceId    = sourceId;
	p->s_barrier     = barrierType;
}
/*---------------------------------------------------------------------------
 * Private utilities:
 */

/**
 * bodySize
 *
 *  Returns the full size of the body of the event given the size of the payload.
 *
 * @param payloadSize Number of bytes in the payload.
 *
 * @return size_t
 */
size_t
CRingFragmentItem::bodySize(size_t payloadSize) const
{
  return sizeof(v11::EventBuilderFragment) + (payloadSize-sizeof(uint32_t)) -
    sizeof(v11::RingItemHeader);
}
}                                // v11 namespace.
