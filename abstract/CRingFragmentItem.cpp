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
#include "CRingItemFactory.h"

#include <string.h>
#include <sstream>
#include <stdexcept>

//Fakey body header we use to support this 'abstract' type:

#pragma pack(push, 1)
struct AbstractBodyHeader {
	uint32_t s_size;
	uint64_t s_timestamp;
	uint32_t s_sourceId;
	uint32_t s_barrierType;
};
#pragma pack(pop)

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
) : CRingItem(EVB_FRAGMENT, bodySize(payloadSize)) 
{
  init(payloadSize);
  
  // This is fine because we're constructing the body header.
 
	setBodyHeader(timestamp, source, barrier); 
  
  copyPayload(pBody, payloadSize);
  updateSize();
  
}

/**
 * constructor 
 * 
 * Given a reference to a ring item create a CRingFragment item:
 * - If the type of the ring item is not EVB_FRAGMENT, throw std::bad_cast.
 * - Otherwise create this from the ring item.
 *
 * @param item - Reference to a generic ring item.
 */
CRingFragmentItem::CRingFragmentItem(const CRingItem& rhs) :
  CRingItem(rhs)
{
  if ((type() != EVB_FRAGMENT) && (type() != EVB_UNKNOWN_PAYLOAD)) {
    throw std::bad_cast();
  }
  updateSize();

}

/**
 * Copy constructor.
 *
 * Construct this as an identical copy of an existing item.
 * 
 * @param rhs - const reference to what we are copying.
 */
CRingFragmentItem::CRingFragmentItem(const CRingFragmentItem& rhs) :
  CRingItem(rhs)
{
    updateSize();
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
		AbstractBodyHeader* pHeader = reinterpret_cast<AbstractBodyHeader*>(getBodyHeader());
		return pHeader->s_timestamp;
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
	AbstractBodyHeader* pHeader = reinterpret_cast<AbstractBodyHeader*>(getBodyHeader());
	return pHeader->s_sourceId;
}
/**
 * return the size of thefragment payload
 * 
 * @return size_t
 */
size_t
CRingFragmentItem::payloadSize() 
{
	
	size_t result =  size() - sizeof(RingItemHeader) - sizeof(AbstractBodyHeader);
	return result;
	
}
/**
 * Return a const pointer to the payload.
 *
 * @return const void*
 */
void*
CRingFragmentItem::payloadPointer()
{
	AbstractBodyHeader* pH =
        reinterpret_cast<AbstractBodyHeader*>(getBodyHeader());
    return pH+1;
  

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
 * getBarrierType
 *    Override from ringitem base class.
 * @return uint32_t the barrier type.
 */
uint32_t
CRingFragmentItem::getBarrierType() const
{
    AbstractBodyHeader* pH =
        reinterpret_cast<AbstractBodyHeader*>(getBodyHeader());
    return pH->s_barrierType;
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
   
	out << "   Does not look like a ring item";
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
/**
 * getBodyHeader
 *    This is a bit tricky - really we should never get called however
 *    fragment data inherently has a body header.
 *    We'll return a pointer just past the header for this.
 *
 *    @return void*
 */
void*
CRingFragmentItem::getBodyHeader() const
{
	  const auto p = reinterpret_cast<const RingItemHeader*>(getItemPointer());
		return const_cast<pRingItemHeader>(p+1);
}
/**
 * setBodyHeader.
 *    This is sort of stripped down compared with what concrete classes
 *    for versions with body headers can do:
 *  - If the cursor is just past the header we will fill in an 11.x style
 *    header size and count point the cursor past us and recompute the size.
 *  - If the cursor is not just past the header, we throw  logic_error
 *    exception.
 *  @param timestamp - timestamp for the body header.
 *  @param sourceid  - Sourceid
 *  @param barrierType - barrier type.
 */
void
CRingFragmentItem::setBodyHeader(
		uint64_t timestamp, uint32_t sourceId,
		uint32_t barrierType
)
{
		uint32_t* p32 = reinterpret_cast<uint32_t*>(getBodyCursor());
		pRingItemHeader ph = reinterpret_cast<pRingItemHeader>(getItemPointer());
		if (p32 == reinterpret_cast<uint32_t*>(ph+1)) {
			AbstractBodyHeader* p = reinterpret_cast<AbstractBodyHeader*>(p32);
			p->s_size = sizeof(AbstractBodyHeader);
			p->s_timestamp = timestamp;
			p->s_sourceId  = sourceId;
			p->s_barrierType = barrierType;
			
			setBodyCursor(p+1);
			updateSize();
		} else {
			throw std::logic_error("Abstract RingFragment item already has a body header");
		}
}
/**
 * hasBodyHeader
 *    We always do:
 *
 */
bool
CRingFragmentItem::hasBodyHeader() const
{
	return true;
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
	// Leave this for now -- there might be a cleaner way in terms of
	// DataFormat.h methods but that will involve a pile of casting away
	// const-ness
  return sizeof(EventBuilderFragment) + (payloadSize-sizeof(uint32_t)) -
    sizeof(RingItemHeader);
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
 * @param size           - Number of bytes in the payload:
 */
void
CRingFragmentItem::copyPayload(const void* pPayloadSource, size_t size)
{
    if (size) {        
      uint8_t* pPayload =
        reinterpret_cast<uint8_t*>(getBodyCursor());
       memcpy(pPayload, pPayloadSource, size);
			 pPayload += size;
			 setBodyCursor(pPayload);
			 updateSize();
    }
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

  deleteIfNecessary();
  newIfNecessary(size);

  uint8_t* pCursor = reinterpret_cast<uint8_t*>(getBodyPointer());
  setBodyCursor(pCursor);
  updateSize();

}
