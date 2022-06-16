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

namespace v10 {
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
				     const void* pBody, uint32_t barrier) :
  ::CRingFragmentItem(timestamp, source, payloadSize, pBody, barrier)

{
  init(payloadSize);
	// Init has reserved space so we just need to get an item pointer and fill in the
	// fields in pEventBuilderFragment
	
	v10::pEventBuilderFragment  p =
		reinterpret_cast<v10::pEventBuilderFragment>(getItemPointer());
	p->s_timestamp = timestamp;
	p->s_sourceId = source;
	p->s_payloadSize = payloadSize;
	p->s_barrierType = barrier;
  
  copyPayload(pBody);

}

/**
 * destructor
 *
 * Base class does everything we need:
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
	const v10::EventBuilderFragment* p =
		reinterpret_cast<const v10::EventBuilderFragment*>(getItemPointer());
  return p->s_timestamp;
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
	const v10::EventBuilderFragment* p =
		reinterpret_cast<const v10::EventBuilderFragment*>(getItemPointer());
  return p->s_sourceId;
}
/**
 * return the size of thefragment payload
 * 
 * @return size_t
 */
size_t
CRingFragmentItem::payloadSize() const
{
	const v10::EventBuilderFragment* p =
		reinterpret_cast<const v10::EventBuilderFragment*>(getItemPointer());
  return static_cast<size_t>(p->s_payloadSize);
}
/**
 * Return a const pointer to the payload.
 *
 * @return const void*
 */
void*
CRingFragmentItem::payloadPointer()
{
	v10::EventBuilderFragment* p =
		reinterpret_cast<v10::EventBuilderFragment*>(getItemPointer());
  return reinterpret_cast<void*>(p->s_body);
}
/**
 * return the barrier type:
 *
 * @return int32_t
 */
uint32_t
CRingFragmentItem::barrierType() const
{
	const v10::EventBuilderFragment* p =
		reinterpret_cast<const v10::EventBuilderFragment*>(getItemPointer());
  return p->s_barrierType;
}
////////////////////////////////////////////////////////////////////////
// Virtual overrides

/**
 * hasBodyHeader
 *    @return false - v10 items have no body header.
*/
bool
CRingFragmentItem::hasBodyHeader() const
{
	return false;
}
/**
 * getBodyHeader
 *    @return nullptr - no body header in v10 items.
 */
void*
CRingFragmentItem::getBodyHeader() const
{
	return nullptr;
}
/**
 * setBodyHeader
 *    No-op.  No body header in V10 items.
 */
void
CRingFragmentItem::setBodyHeader(
    uint64_t timestamp, uint32_t sid, uint32_t barrier
  )
{
	
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
 *  TODO:  Issue #1458 - use factory method if the body seems like a ring fragment
 *         to dump the body.
 * 
 * @return std::string - the stringified version of the fragment.
 */
std::string
CRingFragmentItem::toString() const
{
  static const int perLine = 16;
  std::ostringstream out;

  out << typeName() << ':' << std::endl;
  out << "Fragment timestamp:    " << timestamp()   << std::endl;
  out << "Source ID         :    " << source()      << std::endl;
  out << "Payload size      :    " << payloadSize() << std::endl;
  out << "Barrier type      :    " << barrierType() << std::endl;

  // TODO: Issue #1458 -- see above.

  out << "- - - - - -  Payload - - - - - - -\n";
  
    out << std::hex << std::endl;
		auto pThis = const_cast<v10::CRingFragmentItem*>(this);
    uint8_t* p = reinterpret_cast<uint8_t*>(pThis->payloadPointer());
    for (int i = 0; i < payloadSize(); i++) {
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
  return sizeof(v10::EventBuilderFragment) + (payloadSize-sizeof(uint32_t)) - sizeof(RingItemHeader);
}
/**
 * copyPayload
 *
 *  Copies the payload source to our payload destination.
 *  - It is assumed the destination size is big enough.
 *  - The cursor is reset after the copy.
 *  - The size is updated after the copy.
 *  - The payload size element is used to determine the data size.
 *
 * @param pPayloadSource - Pointer to the buffer containing the payload.
 *
 */
void
CRingFragmentItem::copyPayload(const void* pPayloadSource)
{
	pEventBuilderFragment p =
		reinterpret_cast<v10::pEventBuilderFragment>(getItemPointer());
  memcpy(p->s_body, pPayloadSource, p->s_payloadSize);


}
/**
 * init
 *
 * Initialize the size and various pointers in the base class.  Ensures the buffer is big enough
 * to take the data.  This should not be done with a fragment that has any data/meaning.
 *
 *  @param size Size of payload
 */
void
CRingFragmentItem::init(size_t size)
{
  size_t n = bodySize(size);

  deleteIfNecessary();
  newIfNecessary(size);

  uint8_t* pCursor = reinterpret_cast<uint8_t*>(getBodyPointer());
  
  pCursor         += n;
  setBodyCursor(pCursor);
  updateSize();

}

}