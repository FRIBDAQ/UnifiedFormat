/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt
*/

/**
 * @file CUnknownFragment.cpp
 * @brief Implements the CUnknownFragment calss for EVB_UNKNOWN_PAYLOAD ring items.
 * @author Ron Fox <fox@nscl.msu.edu>
 */

#include "CUnknownFragment.h"
#include "CRingFragmentItem.h"
#include "CRingItem.h"

#include "DataFormat.h"
#include <string.h>
#include <sstream>

namespace v11 {
/*-----------------------------------------------------------------------------
 * Canonical methods.
 *---------------------------------------------------------------------------*/

/**
 * constructor
 *
 * This is the primary constructor.
 *
 * @param timestamp - Ticks that identify when this fragment was triggered.
 * @param sourceId  - Id of the source that created this fragment.
 * @param barrier   - Barrier id of the fragment o 0 if this was not part of a
 *                    barrier.
 * @param size      - Number of _bytes_ in the payload.
 * @param pPayload  - Pointer to the payload.
 */
CUnknownFragment::CUnknownFragment(
    uint64_t timestamp, uint32_t sourceId, uint32_t barrier, uint32_t size,
    void* pPayload) :
       ::CUnknownFragment(timestamp, sourceId, barrier, size, pPayload)
{
    v11::pEventBuilderFragment pItem = reinterpret_cast<v11::pEventBuilderFragment>(getItemPointer());
    pItem->s_header.s_type = v11::EVB_UNKNOWN_PAYLOAD;
    pItem->s_header.s_size = sizeof(v11::RingItemHeader) + sizeof(v11::BodyHeader)
        + size;
        
    setBodyHeader(timestamp, sourceId, barrier);
    
    memcpy(pItem->s_body, pPayload, size);
    setBodyCursor(pItem->s_body + size);
    updateSize();
        
}
/**
 * destructor
 */
CUnknownFragment::~CUnknownFragment()  {}

/**
 * getBodyPointer
 *   @return (const) void* - pointer to s_body.
 */
const void*
CUnknownFragment::getBodyPointer() const
{
    const v11::EventBuilderFragment* pItem =
        reinterpret_cast<const v11::EventBuilderFragment*>(getItemPointer());
    return pItem->s_body;
}
void*
CUnknownFragment::getBodyPointer()
{
    v11::pEventBuilderFragment pItem =
        reinterpret_cast<v11::pEventBuilderFragment>(getItemPointer());
    return pItem->s_body;
}
/**
 * hasBodyHeader
 *   @return bool -true - there's always a body header.
 */
bool
CUnknownFragment::hasBodyHeader() const
{
    return true;
}

/**
 * getBodyHeder
 *   @return void* -- there's always a body header.
 */
void*
CUnknownFragment::getBodyHeader() const
{
    const v11::EventBuilderFragment* pItem =
        reinterpret_cast<const v11::EventBuilderFragment*>(getItemPointer());
    return const_cast<v11::BodyHeader*>(&(pItem->s_bodyHeader));
}
/**
 * setBodyHeader
 *   @param timestamp -event fragment timestamp
 *   @param sourceId - fragment source id
 *   @param barrier - barrier type.
 */
void
CUnknownFragment::setBodyHeader(
    uint64_t timestamp, uint32_t sourceId, uint32_t barrier
)
{
    v11::EventBuilderFragment* pItem =
        reinterpret_cast<v11::EventBuilderFragment*>(getItemPointer());
    pItem->s_bodyHeader.s_size = sizeof(v11::BodyHeader);
    pItem->s_bodyHeader.s_timestamp = timestamp;
    pItem->s_bodyHeader.s_sourceId = sourceId;
    pItem->s_bodyHeader.s_barrier = barrier;
}

/*----------------------------------------------------------------------------
 * Virtual method overrides;
 *--------------------------------------------------------------------------*/

/**
 * typeName
 *
 * @return std::string textual version of item type.
 */
std::string
CUnknownFragment::typeName() const
{
    return "Fragment with unknown payload";
}
/**
 * toString
 *    Return a string rendering of this item...we defer this to
 *    CRingFragmentItem via our usual dirty cast:
 * @return std::string
 */
std::string
CUnknownFragment::toString() const
{
    const v11::CRingFragmentItem* pThis =
        reinterpret_cast<const v11::CRingFragmentItem*>(this);
    return pThis->v11::CRingFragmentItem::toString();
}

}