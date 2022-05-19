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
#include "CRingItem.h"  // for side casts
#include "DataFormat.h"

#include <string.h>
#include <sstream>

namespace v12 {

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
CRingFragmentItem::CRingFragmentItem(
			uint64_t timestamp, uint32_t source, uint32_t payloadSize, 
			const void* pBody, uint32_t barrier
) : ::CRingFragmentItem(timestamp, source, payloadSize, pBody, barrier)
{
	v12::pEventBuilderFragment pItem =
		reinterpret_cast<v12::pEventBuilderFragment>(getItemPointer());
  
	pItem->s_header.s_type = v12::EVB_FRAGMENT;
	pItem->s_bodyHeader.s_size   = sizeof(v12::BodyHeader);
	pItem->s_bodyHeader.s_timestamp = timestamp;
	pItem->s_bodyHeader.s_sourceId = source;
	pItem->s_bodyHeader.s_barrier = barrier;
	
	if (payloadSize) {
		memcpy(pItem->s_body, pBody, payloadSize);
	}
	setBodyCursor(pItem->s_body + payloadSize);
	updateSize();
  
}
/**
 * destructor
 */
CRingFragmentItem::~CRingFragmentItem() {}

/*------------------------------------------------------------------------------
 * getters:
 */

/**
 * Return the current object's timestamp.
 *
 * @return uint64_t
 */
uint64_t
CRingFragmentItem::timestamp() const
{
  return getEventTimestamp();
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
  return getSourceId();
}
/**
 * return the size of thefragment payload
 * 
 * @return size_t
 */
size_t
CRingFragmentItem::payloadSize() 
{
	return getBodySize();
	
}
/**
 * Return a const pointer to the payload.
 *
 * @return const void*
 */
void*
CRingFragmentItem::payloadPointer()
{
	return getBodyPointer();
  

}
/**
 * return the barrier type:
 *
 * @return int32_t
 */
uint32_t
CRingFragmentItem::barrierType() const
{
  return getBarrierType();
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
  CRingFragmentItem* This = const_cast<CRingFragmentItem*>(this);
  
  out << typeName() << ':' << std::endl;
  out << "Fragment timestamp:    " << timestamp()   << std::endl;
  out << "Source ID         :    " << source()      << std::endl;
  out << "Payload size      :    " << This->payloadSize() << std::endl;
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
	if (This->payloadSize() % perLine) {
		out << std::endl;		// if needed a trailing endl.
	}
	

  return out.str();
}

///////////////
// The methods below involve side-casting to v12::CRingItem. While
// this class is not in our inheritance hierarchy (and to put it there will result
// in an inheritance diamond of death leading to ::CRingItem), the data structures
// used to store our ring item and our mutual derivation from ::CRingItem which
// provides services needed to get at stuff we really need.
//
// You'll see fully qualified method names used -- this is to override the methods
// that otherwise will get selected from our virtual method table.
//

/**
 * getBodySize
 *    @return size_t number of bytes in the body. .in this case payload size.
 */
size_t
CRingFragmentItem::getBodySize() const
{
	const v12::CRingItem* pThis = reinterpret_cast<const v12::CRingItem*>(this);
	
	return pThis->v12::CRingItem::getBodySize();
}
/**
 * getBodyPointer [const]
 *    @return void* [const] - pointer to our body (the payload).
 */
const void*
CRingFragmentItem::getBodyPointer() const
{
	const v12::CRingItem* pThis = reinterpret_cast<const v12::CRingItem*>(this);
	return pThis->v12::CRingItem::getBodyPointer();
}
void*
CRingFragmentItem::getBodyPointer()
{
	v12::CRingItem* pThis = reinterpret_cast<v12::CRingItem*>(this);
	return pThis->v12::CRingItem::getBodyPointer();
}
/**
 * hasBodyHeader
 *    @return bool - true - we actually always have one
 */
bool
CRingFragmentItem::hasBodyHeader() const
{
	return true;
}
/**
 * setBodyHeader
 *    If necessary space is created to hold a new body header.
 *    otherwise the contents of the existing one is replaced.
 * @param timestamp -event fragment timestamp.
 * @param sourcdeId - event fragment soure id.
 * @param barrierType - event fragment barrier type code.
 */
void
CRingFragmentItem::setBodyHeader(
	uint64_t timestamp, uint32_t sourceId, uint32_t barrierType
)
{
	v12::CRingItem* pThis = reinterpret_cast<v12::CRingItem*>(this);
	pThis->v12::CRingItem::setBodyHeader(timestamp, sourceId, barrierType);
}
/**
 * getBodyHeader
 *    @return void* pointer to the body header.
 *    @note CRingFragmentItem objects always have  body header.
 */
void*
CRingFragmentItem::getBodyHeader() const
{
	const v12::CRingItem* pThis = reinterpret_cast<const v12::CRingItem*>(this);
	return pThis->v12::CRingItem::getBodyHeader();
}
// get items from the body header.

uint64_t
CRingFragmentItem::getEventTimestamp() const
{
	const v12::CRingItem* pThis = reinterpret_cast<const v12::CRingItem*>(this);
	return pThis->v12::CRingItem::getEventTimestamp();
}
uint32_t
CRingFragmentItem::getSourceId() const
{
	const v12::CRingItem* pThis = reinterpret_cast<const v12::CRingItem*>(this);
	return pThis->v12::CRingItem::getSourceId();
}
uint32_t
CRingFragmentItem::getBarrierType() const
{
	const v12::CRingItem* pThis = reinterpret_cast<const v12::CRingItem*>(this);
	return pThis->v12::CRingItem::getBarrierType();
}

}                          // v12 namespace.