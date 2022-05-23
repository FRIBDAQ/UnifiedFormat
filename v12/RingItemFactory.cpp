/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     FRIB
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  RingItemFactory.cpp (v12)
 *  @brief: Implement v12::RingItemFactory.
 */
#include "RingItemFactory.h"

#include "CRingItem.h"
#include <CAbnormalEndItem.h>
#include <CGlomParameters.h>
#include <CDataFormatItem.h>
#include <CPhysicsEventItem.h>
#include <CRingFragmentItem.h>
#include <CRingPhysicsEventCountItem.h>
#include <CRingScalerItem.h>
#include <CRingTextItem.h>
#include <CUnknownFragment.h>
#include <CRingStateChangeItem.h>
#include <DataFormat.h>
#include "DataFormat.h"

#include <string.h>

namespace v12 {

/**
 * makeRingItem.
 *    Create a v12:: ring item:
 *  @param type  - type of item.
 *  @param maxBody - maximum body size.
 *  @return ::CRingItem* - pointer to the newly created ring item.
 */
::CRingItem*
RingItemFactory::makeRingItem(uint16_t type, size_t maxbody)
{
    return new v12::CRingItem(type, maxbody);
}
/**
 * makeRingItem
 *    Make  a ring item with a body header.
 * @param type - item type.
 * @param timestamp - body header timestamp
 * @param sourceId - body header source id.
 * @param maxbody  - bound on size (capacity).
 * @param barrierType - body header barrier type.
 *  @return ::CRingItem* - pointer to the newly created ring item.
 */
::CRingItem*
RingItemFactory::makeRingItem(
    uint16_t type, uint64_t timestamp, uint32_t sourceId,
    size_t maxBody, uint32_t barrierType
)
{
    return new v12::CRingItem(type, timestamp, sourceId, maxBody, barrierType);    
}
/**
 * makeRingItem
 *    Like a copy constructor.
 * @param rhs - ring item we're copying.
 *  @return ::CRingItem* - pointer to the newly created ring item.
 */
::CRingItem*
RingItemFactory::makeRingItem(
    const ::CRingItem& rhs
)
{
    v12::CRingItem* pResult = new v12::CRingItem(rhs.type(), rhs.size());
    memcpy(pResult->getItemPointer(), rhs.getItemPointer(), rhs.size());
    return pResult;
}
/**
 * makeRingItem
 *   In this case from a raw ring item.
 * @param rhs - ring item we're copying.
 * @return ::CRingItem*
 */
::CRingItem*
RingItemFactory::makeRingItem(
    const ::RingItem* rhs
)
{
    v12::CRingItem* pResult = new v12::CRingItem(
        rhs->s_header.s_type, rhs->s_header.s_size
    );
    memcpy(pResult->getItemPointer(), rhs, rhs->s_header.s_size);
    return pResult;
}

}