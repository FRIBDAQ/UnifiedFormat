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

/** @file:  RingItemFactory.cpp (v11)
 *  @brief: Implement the V11 ring item factory.
 */
#include "RingItemFactory.h"
#include <DataFormat.h>
#include "DataFormat.h"
#include "DataFormat.h"
#include "CRingItem.h"
#include "CAbnormalEndItem.h"
#include "CDataFormatItem.h"
#include "CGlomParameters.h"
#include "CPhysicsEventItem.h"
#include "CRingFragmentItem.h"
#include "CRingPhysicsEventCountItem.h"
#include "CRingScalerItem.h"
#include "CRingTextItem.h"
#include "CUnknownFragment.h"
#include "CRingStateChangeItem.h"


#include <string.h>
#include <CRingBuffer.h>
#include <iostream>
#include <unistd.h>
#include <io.h>
#include <stdexcept>
#include <typeinfo>
#include <time.h>

namespace v11 {
/**
 * makeRingItem
 *    Create an arbitrary ring item:
 *  @param type - ring item type code.
 *  @param maxbody - maximum size of the body.
 *  @return CRingItem* pointer to dynamically allocated ring item.
 */
::CRingItem*
RingItemFactory::makeRingItem(uint16_t type, size_t maxBody)
{
    return new v11::CRingItem(type, maxBody);
}
/**
 * makeRingItem
 *   @param type - item type.
 *   @param timestamp - body header event timestamp.
 *   @param sourceId  - source id for body header.
 *   @param maxbody   - maximum swize.
 *   @param barrierType -body header barrier type.,
 *   @return ::CRingItem* pointer to dynamically allocated ring item.
 */
::CRingItem*
RingItemFactory::makeRingItem(
    uint16_t type, uint64_t timestamp, uint32_t sourceId,
    size_t maxBody, uint32_t barrierType
)
{
    return new v11::CRingItem(type, timestamp, sourceId, barrierType, maxBody);
}
/**
 * makeRingItem
 *  Create a ring item from another ring item.
 *   @param rhs - ring item we're copying.
 *   @note one use for this is as a down cast from a specific to a
 *         generic ring item.
 *   @note there is an assumption that the ring item header
 *        matches the format of the v11::RingItemHeader.
 */
::CRingItem*
RingItemFactory::makeRingItem(const ::CRingItem& rhs)
{
    v11::CRingItem* pItem =  new v11::CRingItem(rhs.type(), rhs.size());
    memcpy(pItem->getItemPointer(), rhs.getItemPointer(), rhs.size());
    uint8_t* pCursor = reinterpret_cast<uint8_t*>(pItem->getItemPointer());
    pCursor += rhs.size();
    pItem->setBodyCursor(pCursor);
    pItem->updateSize();
    return pItem;
}
/**
 * makeRingItem
 *    Make a ring item from a raw ring item.
 *  @param pRawRing - raw ring item pointer.
*/
::CRingItem*
RingItemFactory::makeRingItem(const ::RingItem* pRawRing)
{
    v11::CRingItem* pItem = new v11::CRingItem(
        pRawRing->s_header.s_type, pRawRing->s_header.s_size
    );
    memcpy(pItem->getItemPointer(), pRawRing, pRawRing->s_header.s_size);
    uint8_t* pCursor = reinterpret_cast<uint8_t*>(pItem->getItemPointer());
    pCursor += pRawRing->s_header.s_size;
    pItem->setBodyCursor(pCursor);
    pItem->updateSize();
    return pItem;
}
/**
 * getRingItem
 *     Get a ring item from a ringbuffer (we must be attached as
 *     a consumer).
 *  @param ringbuf - reference to the ring buffer.
 *  @return ::CRingItem* newly allocated ring item.
 *  @note we will block as long as needed.
 */
::CRingItem*
RingItemFactory::getRingItem(CRingBuffer& ringbuf)
{
    v11::RingItemHeader hdr;
    ringbuf.get(&hdr, sizeof(hdr));
    v11::CRingItem* pItem = new v11::CRingItem(hdr.s_type, hdr.s_size);
    size_t remaining = hdr.s_size - sizeof(v11::RingItemHeader);
    v11::pRingItem pItemStorage =
        reinterpret_cast<v11::pRingItem>(pItem->getItemPointer());
    uint8_t* p = reinterpret_cast<uint8_t*>(&(pItemStorage->s_body));
    ringbuf.get(p, remaining);
    p += remaining;
    pItem->setBodyCursor(p);
    pItem->updateSize();
    return pItem;
}
/**
 * getRingItem
 *   @param fd - file descriptor open on the source of ring items.
 *   @return ::CRingItem* points to a dynamically allocated v11 ring item.
 *   @retval nullptr - eof.
 */
::CRingItem*
RingItemFactory::getRingItem(int fd)
{
    v11::RingItemHeader hdr;
    if (io::readData(fd, &hdr, sizeof(hdr)) < sizeof(hdr)) {
        return nullptr;
    }
    v11::CRingItem* pResult = new v11::CRingItem(hdr.s_type, hdr.s_size);
    
    v11::pRingItem pRawItem =
        reinterpret_cast<v11::pRingItem>(pResult->getItemPointer());
    size_t remainingSize = hdr.s_size - sizeof(v11::RingItemHeader);
    uint8_t* p = reinterpret_cast<uint8_t*>(&(pRawItem->s_body));
    if (io::readData(fd, p, remainingSize) != remainingSize) {
        delete p;
        return nullptr;                 // EOF.
    }
    p += remainingSize;
    pResult->setBodyCursor(p);
    pResult->updateSize();
    
    return pResult;
}
/**
 * getRingItem
 *   @param in  - std::istream& open on the ring item data source.
 *   @return ::CRingITem* points to a dynamically allocated v11::CRingItem.
 *   @retval 
 */
::CRingItem*
RingItemFactory::getRingItem(std::istream& in)
{
    v11::RingItemHeader hdr;
    in.read(reinterpret_cast<char*>(&hdr), sizeof(hdr));
    if (!in) {
        return nullptr;            
    }
    v11::CRingItem* pResult = new v11::CRingItem(hdr.s_type, hdr.s_size);
    size_t remaining = hdr.s_size - sizeof(v11::RingItemHeader);
    v11::pRingItem pRawItem =
        reinterpret_cast<v11::pRingItem>(pResult->getItemPointer());
    uint8_t* pCursor = reinterpret_cast<uint8_t*>(&(pRawItem->s_body));
    in.read(reinterpret_cast<char*>(pCursor), remaining);
    if (!in) {
        delete pResult;
        return nullptr;
    }
    pResult->setBodyCursor(pCursor + remaining);
    pResult->updateSize();
    
    return pResult;
    
}
/**
 * putRingItem
 *     Put a ring item into a stream.  This blocks, if necessary
 *     until space is available.
 *  @param pItem - pointer to CRingItem.
 *  @param out   - References the std::ostream into which we put.
 *  @return std::ostream&  stream that was passed in to support chaining.
 *  @note output failures are reflected in the usual manner for
 *        std::ostream objects.
 */
std::ostream&
RingItemFactory::putRingItem(const ::CRingItem* pItem, std::ostream& out)
{
    const void* pData = pItem->getItemPointer();
    size_t bytes      = pItem->size();
    
    return out.write(reinterpret_cast<const char*>(pData), bytes);
    
}
/**
 * putRingItem
 *    Put  a ring item in to a file
 * @param pItem - Item to put.
 * @param fd - file descriptor;
 */
void
RingItemFactory::putRingItem(const ::CRingItem* pItem, int fd)
{
    const void* pData = pItem->getItemPointer();
    size_t bytes      = pItem->size();
    io::writeData(fd, pData, bytes);
}
/**
 * putRingItem
 *    Put a ring item to a ringbuffer.  BLocks until the item is fully
 *    put.
 * @param pItem - Item to put.
 * @param rbuf  - Ring buffer reference.
 */
void
RingItemFactory::putRingItem(const ::CRingItem* pItem, CRingBuffer& rbuf)
{
    rbuf.put(pItem->getItemPointer(), pItem->size());
}
/**
 * makeAbnormalEndItem.
 *   Creates a v11::CAbnormalEndItem and returns it as a base class
 *   pointer
 *  @return ::CAbnormalEndItem*
 */
::CAbnormalEndItem*
RingItemFactory::makeAbnormalEndItem()
{
    return new v11::CAbnormalEndItem;
}
/**
 * makeAbnormalEndITem
 *    Given an arbitrary ring item reference, if the type is
 *    that of an abnormal end item, return an abnormal end item
 *    generated from it.
 *  @param rhs - input ring item.
 *  @return CAbnormalEndItem*
 *  @throws std::bad_cast -if rhs is not an abnormal end item.
 */
::CAbnormalEndItem*
RingItemFactory::makeAbnormalEndItem(const ::CRingItem& rhs)
{
    if (rhs.type() == v11::ABNORMAL_ENDRUN) {
        // there are no contents to speak of so:
        
        return new v11::CAbnormalEndItem;
    } else {
        throw std::bad_cast();
    }
}
/**
 *  makeDataFormatItem.
 *    @return ::CDataFormatItem*  - actually points to a V11::CDataFormatItem.
 */
::CDataFormatItem*
RingItemFactory::makeDataFormatItem()
{
    return new CDataFormatItem;           // Has right versions.
}
/**
 * makeDataFormatItem.
 *    @param rhs - item to turn into a v11 data format item.
 *    @throws std::bad_cast if rhs is not a data format item.
 *    @return ::CDataFormatItem*
 */
::CDataFormatItem*
RingItemFactory::makeDataFormatItem(const ::CRingItem& rhs)
{
    // Require it be a data format item and of our format:
    
    if (rhs.type() == v11::RING_FORMAT) {
        const ::CDataFormatItem& fmt =
            dynamic_cast<const::CDataFormatItem&>(rhs);
        if (fmt.getMajor() != v11::FORMAT_MAJOR) {
            throw std::bad_cast();
        } else {
            return new v11::CDataFormatItem;
        }
    } else {
        throw std::bad_cast();
    }
    
}
/**
 * makeGlomParameters
 *    @param  interval - the build interval
 *    @param  isBuliding - true if building was enabled.
 *    @param  policy     - Event building timestap policy
 *                         determines how the timstamp of the final
 *                         built events are computed.
 *    @return ::CGlomParameters* - actually a pointer to v11::GlomParameters
 */
::CGlomParameters*
RingItemFactory::makeGlomParameters(
    uint64_t interval, bool isBuilding, uint16_t policy
)
{
    ::CGlomParameters::TimestampPolicy ePolicy =
        static_cast<::CGlomParameters::TimestampPolicy>(policy);
    return new CGlomParameters(interval, isBuilding, ePolicy);
}
/**
 * makeGlomParameters
 *    Given a ring item that is alleged to be a glom parameters
 *    produces a new ring item that's a v11::CGlomPolicies item
 *    and hands back a pointer to it:
 * @param rhs - the policy item.
 * @return ::CGlomParameters*
 */
::CGlomParameters*
RingItemFactory::makeGlomParameters(const ::CRingItem& rhs)
{
    if (rhs.type() == v11::EVB_GLOM_INFO) {
        const ::CGlomParameters& glom =
            dynamic_cast<const ::CGlomParameters&>(rhs);
        return new v11::CGlomParameters(
            glom.coincidenceTicks(), glom.isBuilding(), glom.timestampPolicy()
        );
    } else {
        throw std::bad_cast();
    }
}
/**
 * makePhysicsEventItem
 *    Make an empty v11 physics event item.    The caller can then
 *    use e.g. setBodyHeader, getBodyCursor and so on to fill in
 *    its contents.
 * @param maxBody - maximum size of the item.
 * @return ::CPhysicsEventItem* - actually pointing at a v11::CPhysicsEventItem.
 */
::CPhysicsEventItem*
RingItemFactory::makePhysicsEventItem(size_t maxbody)
{
    return new v11::CPhysicsEventItem(maxbody);
}
/**
 * makePhysicsEventItem
 *    Create an empty v11 physics event item with pre-filled body
 *    header.
 *  @param timestamp - event fragment timestamp to put in body header.
 *  @param source    - source id of fragment to put in body header.
 *  @param barrier   - barrier type id to put in body header.
 *  @param maxsize   - largest ring item size.
 *  @return ::CPhysicsEventItem* - actuall points to a v11::CPhysicsEventItem.
 */
::CPhysicsEventItem*
RingItemFactory::makePhysicsEventItem(
    uint64_t timestamp, uint32_t source, uint32_t barrier, size_t maxsize
)
{
    return new v11::CPhysicsEventItem(
        timestamp, source, barrier, maxsize
    );
}
/**
 * makePhysicsEventItem
 *    creates a v11::CPhysicsEventItem from a raw ring item.
 *    we assume:
 *    - The body header, if it exists in the raw item, at least
 *      begins with the structure of the v11 body header.
 *    - The Body of the event will be passed unmodified to the new
 *       item.
 *    We require that the type be v11::PHYSICS_EVENT
 *    Note that at this time all versions share common item types.
 * @param rhs - reference to ring item to use as data source.
 * @return ::CPhysicsEventItem* actuall pointing to a v11 item.
 */
::CPhysicsEventItem*
RingItemFactory::makePhysicsEventItem(const ::CRingItem& rhs)
{
    if (rhs.type() == v11::PHYSICS_EVENT) {
        v11::CPhysicsEventItem* pResult = new v11::CPhysicsEventItem(rhs.size());
        
        if (rhs.hasBodyHeader()) {
            const v11::BodyHeader* pBh =
                reinterpret_cast<const v11::BodyHeader*>(rhs.getBodyHeader());
            pResult->setBodyHeader(
                pBh->s_timestamp, pBh->s_sourceId, pBh->s_barrier
            );
        }
        // How big is the body so we can memcpy:
        
        
        const uint8_t* pBegin =
            reinterpret_cast<const uint8_t*>(rhs.getBodyPointer());
        size_t bodySize = rhs.getBodySize();
        uint8_t* p = reinterpret_cast<uint8_t*>(pResult->getBodyPointer());
        memcpy(p, pBegin, bodySize);
        p += bodySize;
        pResult->setBodyCursor(p);
        pResult->updateSize();
        
        return pResult;
        
    } else {
        throw std::bad_cast();
    }
}
/**
 * makeRingFragmentItem
 *    Create an item that contains an event fragment with arbitrary
 *    payload.
 *  @param timestamp - body header timestamp (fragment items always have
 *                     a body header).
 *  @param source    - body header source id.
 *  @param payloadSize - size of the payload.
 *  @param payload   - Pointer to the payload.
 *  @param barrier   - body header barrier id.
 *  @return ::CRingFramentItem*  - pointer to an v11::CRingFragmentItem.
 */
::CRingFragmentItem*
RingItemFactory::makeRingFragmentItem(
    uint64_t timestamp, uint32_t source,
    uint32_t payloadSize, const void* payload, uint32_t barrier
)
{
    return new v11::CRingFragmentItem(
        timestamp, source, payloadSize, payload, barrier
    );
}
/**
 * makeRingFragmentItem
 *    Create a ring fragment item from an existing ring item.
 * @param rhs - the ring item we make the fragment from.
 */
::CRingFragmentItem*
RingItemFactory::makeRingFragmentItem(const ::CRingItem& rhs)
{
    if (rhs.type() == v11::EVB_FRAGMENT) {
        const ::CRingFragmentItem& src =
            dynamic_cast<const ::CRingFragmentItem&>(rhs);
        return new v11::CRingFragmentItem(
            src.timestamp(), src.source(),
            src.payloadSize(),
            const_cast<::CRingFragmentItem&>(src).payloadPointer(),
            src.barrierType()
        );
    } else if (rhs.type() == v11::EVB_UNKNOWN_PAYLOAD) {
        return reinterpret_cast<::CRingFragmentItem*>(makeUnknownFragment(rhs));
    } else {
        throw std::bad_cast();
    }
}
/**
 * makePhysicsEventCountItem
 *    @param count - number of trigger.
 *    @param timeoffset - when in the run this occured.
 *    @param stamp   - absolute time.
 *    @param divisor - timeoffset/divisor (float) is seconds.
 *    @return ::CRingPhysicsEventCountItem*
 *                  - actually points to a v11::CRingPhysicsEventCount item.
 */
::CRingPhysicsEventCountItem*
RingItemFactory::makePhysicsEventCountItem(
    uint64_t count, uint32_t timeoffset, time_t stamp, int divisor
)
{
    return new v11::CRingPhysicsEventCountItem(
        count, timeoffset, stamp, divisor
    );
}
/**
 * makePhysicsEventCountItem
 *   @param rhs - ::CRingItem& from which we make this item.
 *   @return ::CRingPhysicsEventCountItem*
 *              - actually points to a v11::CRingPhysicsEventCount item.
 *   @throws std::bad_cast if the rhs is not an event count item.
*/
::CRingPhysicsEventCountItem*
RingItemFactory::makePhysicsEventCountItem(const ::CRingItem& rhs)
{
    if (rhs.type() == v11::PHYSICS_EVENT_COUNT) {
        const ::CRingPhysicsEventCountItem& item =
            dynamic_cast<const ::CRingPhysicsEventCountItem&>(rhs);
        return new v11::CRingPhysicsEventCountItem(
            item.getEventCount(), item.getTimeOffset(),
            item.getTimestamp(), item.getTimeDivisor()
            
        );
    } else {
        throw std::bad_cast();
    }
}
/**
 * makeScalerItem
 *     Make an empty body header-less scaler item.
 *     A body header can be added with setBodyHeader
 *   @param numScalers - number of scalers the item will need to hold.
 *   @return ::CRingScalerItem* - actually points to a v11::CRingScalerItem.
 */
::CRingScalerItem*
RingItemFactory::makeScalerItem(size_t numScalers)
{
    return new v11::CRingScalerItem(numScalers);
}
/**
 * makeScaleritem
 *    Make body header scaler item. Note that the remainder
 *    of the body header must be filled in with setBodyHeader.
 * @param startTime   - Counting interval start run time offset.
 * @param stopTime    - Counting interval stop run time offset.
 * @param timestamp   - Wall time when the scaler was generated.
 * @param scalers     - The scaler counters.
 * @param isIncremental - IF true the counters were zeroed as they were read.
 * @param sid        - source id
 * @param timeOffsetDivisor - divisor that turns start/stopTime into seconds.
 * @return ::CRingScalerItem* - actually points to a v11::CRingScalerItem.
 * 
 */
::CRingScalerItem*
RingItemFactory::makeScalerItem(
    uint32_t startTime, uint32_t stopTime, time_t timestamp,
    std::vector<uint32_t> scalers, bool isIncremental,
    uint32_t sid, uint32_t timeOffsetDivisor
)
{
    return new v11::CRingScalerItem(
        startTime, stopTime, timestamp, scalers, isIncremental,
        sid, timeOffsetDivisor
    );
}
/**
 * makeScaleritem
 *   - in this case from another ring item that must be a scaler.
 *   - Note that this version does not support converting
 *     v10::TIMESTAMPED_NONINCR_SCALERS, though v10::INCREMENTAL_SCALERS
 *     should properly convert.
 *  @param rhs - references the ring item to use to create the new
 *               scaler item.
 *  @return ::CRingScalerItem* - actually points to a v11::CRingScalerItem.
 *  @note - We assume that if there's a body header, the front end of
 *          it looks like the v11 body header (that is additional stoff
 *          get appended in e.g. v12, v13...).
 */
::CRingScalerItem*
RingItemFactory::makeScalerItem(const ::CRingItem&  item)
{
    if (item.type() == v11::PERIODIC_SCALERS) {
        const ::CRingScalerItem& scalerItem =
            dynamic_cast<const ::CRingScalerItem&>(item);
        auto scalers = scalerItem.getScalers();
        uint32_t start = scalerItem.getStartTime();
        uint32_t end   = scalerItem.getEndTime();
        uint32_t div   = scalerItem.getTimeDivisor();
        time_t clock   = scalerItem.getTimestamp();
        bool incr      = scalerItem.isIncremental();
        
        if (scalerItem.hasBodyHeader()) {
            const v11::BodyHeader* pBodyHeader =
                reinterpret_cast<const v11::BodyHeader*>(scalerItem.getBodyHeader());
            return new v11::CRingScalerItem(
                pBodyHeader->s_timestamp,
                pBodyHeader->s_sourceId,
                pBodyHeader->s_barrier,
                start, end, clock, scalers, div, incr
            );
        } else {
            // Making a body header-less item is more work:
            // Since this is the only constructor that does not make a body
            // header:
            
            v11::CRingScalerItem* result = new v11::CRingScalerItem(scalers.size());
            result->setStartTime(start);
            result->setEndTime(end);
            result->setTimestamp(clock);
            for(int i =0; i < scalers.size(); i++) {
                result->setScaler(i, scalers[i]);
            }
            // Now we have to tweak in is incremntal and the divisor:
            
            v11::pScalerItemBody pBody =
                reinterpret_cast<v11::pScalerItemBody>(result->getBodyPointer());
            pBody->s_isIncremental = incr ? 1 : 0;
            pBody->s_intervalDivisor = div;
            
            return result;
        }
    } else {
        throw std::bad_cast();
    }
}
/**
 * makeTextItem.
 *    Create a v11::CRingTextItem and return it to the caller
 *    as a pointer to a generic text item.
 * @param type - the text item type must be one of:
 *           -  v11::PACKET_TYPES
 *           -  v11::MONITORED_VARIABLES
 * @param theStrings - Vector of the strings to encapsulate in the
 *                item
 * @return ::CRingTextItem* - actually points to a v11::CRingTextItem
 *                this item was created with new.
 * @throws std::invalid_argument if type is not valid.
 * @note The returned pointer was created using new and
 *       when the caller is done with it, delete must be used to
 *       destroy it.
 * @ note unspecified fields are set as follows:
 *     -   Run offset is 0.
 *     -   clock time is set to the current time.
 * @note the resulting item has no body header.
 */
::CRingTextItem*
RingItemFactory::makeTextItem(
    uint16_t type, std::vector<std::string> theStrings
)
{
    return new v11::CRingTextItem(type, theStrings, 0, time(nullptr));
}
/**
 * makeTextItem
 *   @param type - text item type.
 *   @param theStrings - strings to embed into the item.
 *   @param offsetTime - Time offset into the run.
 *   @param timestamp  - clock time at which the item will say it was created.
 *   @param divisor    - Optional time divisor to convert offsetTime
 *                       into seconds
 *   @return ::CRingTextItem* - actually points to a v11::CRingTextItem
 *                this item was created with new.
 * @throws std::invalid_argument if type is not valid.
 * @note The returned pointer was created using new and
 *       when the caller is done with it, delete must be used to
 *       destroy it.
 * @note the resulting item has no body header however one can be
 *       added with the setBodyHeader method post creation.
 */
::CRingTextItem*
RingItemFactory::makeTextItem(
    uint16_t type,
    std::vector<std::string> theStrings,
    uint32_t                 offsetTime,
    time_t                   timestamp, uint32_t divisor
) 
{
    return new v11::CRingTextItem(
        type, theStrings, offsetTime, timestamp, divisor
    );
}
/**
 * makeTextItem
 *    Make a ring text item from a generic ring item that must be
 *    a text item.  Once cast to a ::CRingTextItem the virtual
 *    methods are used to pull out all the stuff needed.
 *    Note that if the original item has a body header it's
 *    inserted into the final item post construction as that's easiest.
 * @param rhs - the item we're going to turn into a v11 ring text item.
 * @return ::CRingTextItem* - actually pointing to a dynamically created
 *        v11::CRingTextItem.
 * @note - the item type must be a legitimate v11 text item type.
 * @note - The rhs must be dynamically castable to a ::CRingTextItem
 * @throws - std::invalid_argument - the type was bad.
 * @throws - std::bad_cast rhs was not castable to ::CRingTextItem.
 */
::CRingTextItem*
RingItemFactory::makeTextItem(const ::CRingItem& rhs)
{
    const ::CRingTextItem& item =
        dynamic_cast<const ::CRingTextItem&>(rhs);
    uint16_t type  =item.type();
    std::vector<std::string> strings = item.getStrings();
    uint32_t offset = item.getTimeOffset();
    uint32_t div    = item.getTimeDivisor();
    time_t   clock  = item.getTimestamp();
    
    v11::CRingTextItem* pResult = new v11::CRingTextItem(
        type, strings, offset,  clock, div
    );
    if (item.hasBodyHeader()) {
        const v11::BodyHeader* p =
            reinterpret_cast<const v11::BodyHeader*>(item.getBodyHeader());
        pResult->setBodyHeader(
            p->s_timestamp, p->s_sourceId, p->s_barrier
        );
    }
    return pResult;
    
}
/**
 * makeUnknownFragment
 *     Create an event builder fragment with a non-ring item
 *     payload.
 *  @param timestamp -event timestamp of the item (not wall clock).
 *  @param sourceid  - Data Source Id.
 *  @param barrier   - Barrier type.
 *  @param size      - Size of the payload.
 *  @param pPayload  - Pointer to the payload.
 *  @return ::CUnknownFragment* - pointer to a v11::CUnknownFragment
 */
::CUnknownFragment*
RingItemFactory::makeUnknownFragment(
    uint64_t timestamp, uint32_t sourceid, uint32_t barrier,
    uint32_t size, void* pPayload
)
{
    return new v11::CUnknownFragment(
        timestamp, sourceid, barrier, size, pPayload
    );
}
/**
 * makeUnknownFragment
 *    Same as above but sort of like a copy constructor.
 * @param rhs - the item being copied.
 * @return ::CUnknownFragment* - pointer to a v11 unknown fragment
 */
::CUnknownFragment*
RingItemFactory::makeUnknownFragment(const ::CRingItem& rhs)
{
    if( rhs.type() != v11::EVB_UNKNOWN_PAYLOAD ) {
        throw std::bad_cast();
    }
    const ::CUnknownFragment& item =
        dynamic_cast<const ::CUnknownFragment&>(rhs);
    ::CUnknownFragment& ncItem =
        const_cast<::CUnknownFragment&>(item);
    return makeUnknownFragment(
        item.timestamp(), item.source(), item.barrierType(),
        item.payloadSize(),
        ncItem.payloadPointer()
    );
    
}
/**
 * makeStateChangeItem
 *    Create a new state change item.  The resulting item has no
 *    body header, however one can be added with setBodyHeader.
 * @param itemType  - Type of the item being created.
 * @param runNumber - Run number involved in the state transition.
 * @param timeOffset - Offset into the run.
 * @param timestamp -clock time of the transition.
 * @param title    - Title of the run.
 * @note - the time divisor is 1 however there are some tricks
 *         one can play to set it to another value.
 * @return ::CRingSTateChangeItem* Acutally a V11::CRingStateChangeItem pointer.
 */
::CRingStateChangeItem*
RingItemFactory::makeStateChangeItem(
    uint32_t itemType, uint32_t runNumber,
    uint32_t timeOffset,
    time_t   timestamp,
    std::string title
)
{
    return new v11::CRingStateChangeItem(
        itemType, runNumber, timeOffset, timestamp, title
    );
}
/**
 * makeStateChangeItem
 *  Sort of a copy constructor but with the base class as the
 *  source:
 * @param rhs - a ::CRingItem& which will be used as the source
 *              of data .
 * @return ::CRingStateChangeItem* actually poinging to a v11::CRingStateChangeItem.
 * @throw std::invalid_argument  If any of the following:
 *     * The underlying type is not a valid v11 state change item type.
 *     * The title in the rhs won't fit in the item being created.
 *        this is possible if the rhs in a newer version than v11.
 * @throw std::bad_cast, rhs can't be dynamic cast to a
 *      ::CRingStateChangeItem.
 */
::CRingStateChangeItem*
RingItemFactory::makeStateChangeItem(const ::CRingItem& rhs)
{
    const ::CRingStateChangeItem& item =
        dynamic_cast<const ::CRingStateChangeItem&>(rhs);
    uint16_t type = item.type();
    uint32_t run  = item.getRunNumber();
    uint32_t offset = item.getElapsedTime();
    time_t clock    = item.getTimestamp();
    std::string title = item.getTitle();
    
    ::CRingStateChangeItem* pResult =
        makeStateChangeItem(type, run, offset, clock, title);
    // If there is one, insert a body header:
    
    if (item.hasBodyHeader()) {
        const v11::BodyHeader* pHdr =
            reinterpret_cast<const v11::BodyHeader*>(item.getBodyHeader());
        pResult->setBodyHeader(
            pHdr->s_timestamp, pHdr->s_sourceId, pHdr->s_barrier
        );
    }
    return pResult;
    
}



}                             // v11
