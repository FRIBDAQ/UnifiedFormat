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
#include "DataFormat.h"
#include <string.h>
#include <sstream>

namespace v12 {

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
       ::v12::CRingFragmentItem(timestamp, sourceId, size, pPayload, barrier)
{
    // The only thing left to do is fill in the correct type
    
    v12::pRingItem pItem  = reinterpret_cast<v12::pRingItem>(getItemPointer());
    pItem->s_header.s_type = v12::EVB_UNKNOWN_PAYLOAD;
        
}
/**
 * destructor
 */
CUnknownFragment::~CUnknownFragment()  {}

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
}                         // v12 namespace  