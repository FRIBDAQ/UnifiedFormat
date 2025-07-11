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
#include "CAbnormalEndItem.h"
#include "CGlomParameters.h"
#include "CDataFormatItem.h"
#include "CPhysicsEventItem.h"
#include "CRingFragmentItem.h"
#include "CRingPhysicsEventCountItem.h"
#include "CRingScalerItem.h"
#include "CRingTextItem.h"
#include "CUnknownFragment.h"
#include "CRingStateChangeItem.h"
#include <DataFormat.h>
#include "DataFormat.h"

#include <string.h>
#ifdef HAVE_NSCLDAQ    
#include <CRingBuffer.h>
#endif
#include <io.h>
#include <stdexcept>
#include <set>
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"

namespace ufmt {
    namespace v12 {

    static std::set<uint16_t> validTextItemTypes = {
            v12::PACKET_TYPES, v12::MONITORED_VARIABLES, v12::RING_FORMAT
    };

    /**
     * makeRingItem.
     *    Create a v12:: ring item:
     *  @param type  - type of item.
     *  @param maxBody - maximum body size.
     *  @return ::ufmt::CRingItem* - pointer to the newly created ring item.
     */
    ::ufmt::CRingItem*
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
     *  @return ::ufmt::CRingItem* - pointer to the newly created ring item.
     */
    ::ufmt::CRingItem*
    RingItemFactory::makeRingItem(
        uint16_t type, uint64_t timestamp, uint32_t sourceId,
        size_t maxBody, uint32_t barrierType
    )
    {
        return new v12::CRingItem(type, timestamp, sourceId, barrierType, maxBody);    
    }
    /**
     * makeRingItem
     *    Like a copy constructor.
     * @param rhs - ring item we're copying.
     *  @return ::ufmt::CRingItem* - pointer to the newly created ring item.
     */
    ::ufmt::CRingItem*
    RingItemFactory::makeRingItem(
        const ::ufmt::CRingItem& rhs
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
     * @return ::ufmt::CRingItem*
     */
    ::ufmt::CRingItem*
    RingItemFactory::makeRingItem(
        const ::ufmt::RingItem* rhs
    )
    {
        v12::CRingItem* pResult = new v12::CRingItem(
            rhs->s_header.s_type, rhs->s_header.s_size
        );
        memcpy(pResult->getItemPointer(), rhs, rhs->s_header.s_size);
        return pResult;
    }
    #ifdef HAVE_NSCLDAQ    
    /**
     * getRingItem
     *    Get a ring item from a ring buffer.
     *  @param ringbuf -references a ring buffer from which the ring item will be gotten.
     *   @param timeout - seconds to wait for data before timeout
     *  @return ::ufmt::CRingItem* - pointer to the dynamically created gotten item or nullptr if timed out.
     */
    ::ufmt::CRingItem*
    RingItemFactory::getRingItem(::CRingBuffer& ringbuf, unsigned long timeout)
    {
        v12::RingItemHeader hdr;
        if (!ringbuf.get(&hdr, sizeof(hdr), sizeof(hdr), timeout)) {
	    return nullptr;
	}
        v12::CRingItem* pResult = new CRingItem(hdr.s_type, hdr.s_size);
        
        // Read the remainder of the item:
        
        uint32_t remaining = hdr.s_size - sizeof(hdr);
        uint8_t* p = reinterpret_cast<uint8_t*>(pResult->getItemPointer());
        p += sizeof(hdr);
        
        if (remaining) {
            ringbuf.get(p, remaining, remaining);
            p += remaining;
        }
        pResult->setBodyCursor(p);
        pResult->updateSize();
        return pResult;
    }
    #endif
    /**
     * getRingItem
     *    Read a ring item from file open on a file descriptor.,
     *  @param fd - file descriptor opened on the file.
     *  @return ::ufmt::CRingItem* - pointer to the ring item that was dynamically made.
     *  @retval nullptr - eof.
     */
    ::ufmt::CRingItem*
    RingItemFactory::getRingItem(int fd)
    {
        v12::RingItemHeader hdr;
        if (fmtio::readData(fd, &hdr, sizeof(hdr)) < sizeof(hdr)) {
            return nullptr;
        }
        
        v12::CRingItem* pResult = new v12::CRingItem(hdr.s_type, hdr.s_size);
        
        uint8_t* p = reinterpret_cast<uint8_t*>(pResult->getItemPointer());
        p += sizeof(hdr);
        uint32_t remaining = hdr.s_size - sizeof(hdr);
        
        if (remaining) {
            if (fmtio::readData(fd, p, remaining) < remaining) {
                delete pResult;
                return nullptr;
            }
        }
        
        p += remaining;
        pResult->setBodyCursor(p);
        pResult->updateSize();
        return pResult;
    }
    /**
     * getRingItem
     *     Get a ring item from an std::istream
     *  @param in - the stream from which we'll consume the item.
     *  @return ::ufmt::CRingItem*  - pointer to the new ring item.
     */
    ::ufmt::CRingItem*
    RingItemFactory::getRingItem(std::istream& in)
    {
        v12::RingItemHeader hdr;
        in.read(reinterpret_cast<char*>(&hdr), sizeof(hdr));
        if (!in) {
            return nullptr;            
        }
        v12::CRingItem* pResult = new v12::CRingItem(hdr.s_type, hdr.s_size);
        size_t remaining = hdr.s_size - sizeof(v12::RingItemHeader);
        v12::pRingItem pRawItem =
            reinterpret_cast<v12::pRingItem>(pResult->getItemPointer());
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
     *    Put a ring item to an std::ostream.
     * @param pItem  - pointer to the item to put.
     * @paraqm out - reference to the ostream
     * @return std::ostream&  - reference to the output stream that we put the item to
     *               errors must be checked by the caller.
     */
    std::ostream&
    RingItemFactory::putRingItem(const ::ufmt::CRingItem* pItem, std::ostream& out)
    {
        size_t n = pItem->size();
        const void* p = pItem->getItemPointer();
        out.write(reinterpret_cast<const char*>(p), n);
        return out;
    }
    /**
     * putRingItem
     *   Put a ring item to a file open on an fd:
     *
     *   @param pItem - pointer to the item t put.
     *   @param fd    - File descriptor open on the file.
     *   @throw errors from fmtio::writeData are thrown as exceptions.
     */
    void
    RingItemFactory::putRingItem(const ::ufmt::CRingItem* pItem, int fd)
    {
        size_t n = pItem->size();
        const void* p = pItem->getItemPointer();
        fmtio::writeData(fd, p, n);
    }
    #ifdef HAVE_NSCLDAQ    
    /**
     * putRingItem
     *    Put a ring item to a ring buffer.
     *
     * @param pItem - pointer to the itme.
     * @param ring  - reference the ring buffer object.
     */
    void
    RingItemFactory::putRingItem(const ::ufmt::CRingItem* pItem, ::CRingBuffer& ringbuf)
    {
        const void* p = pItem->getItemPointer();
        size_t      n = pItem->size();
        
        ringbuf.put(p, n);
    }
    #endif
    /**
     *  makeAbnormalEndItem
     *     Create an abnormal end run item:
     *  @return - pointer to a new abnormal end item.
     */
    ::ufmt::CAbnormalEndItem*
    RingItemFactory::makeAbnormalEndItem()
    {
        return new v12::CAbnormalEndItem();
    }
    /**
     * makeAbnormalEndItem
     *    Create an abnormal end item from an undifferentiated ring item object.
     *    The If the ring item type is not v12::ABNORMAL_ENDRUN
     *    std::bad_cast is thrown.
     * @param rhs - const reference to the ring item we're 'casting'.
     * @return ::CAbnormalEndItem* - pointer to new abnormal end item.
     * @throw std::bad_cast.
     */
    ::ufmt::CAbnormalEndItem*
    RingItemFactory::makeAbnormalEndItem(const ::ufmt::CRingItem& rhs)
    {
        if (rhs.type() != v12::ABNORMAL_ENDRUN) {
            throw std::bad_cast();
        }
        if (rhs.size() != sizeof(v12::AbnormalEndItem)) {
            throw std::bad_cast();
        }
        return new v12::CAbnormalEndItem();   // All look the same.
    }
    /**
     * makeDataFormatItem.
     *    @return ::CDataFormatItem.
     *    @note   We return one for v12.0.
     */
    ::ufmt::CDataFormatItem*
    RingItemFactory::makeDataFormatItem()
    {
        return new v12::CDataFormatItem();
    }
    /**
     * makeDataFormatItem
     *    'cast' a ring item to a data format item.
     *    -  The ring item must have type v12::RING_FORMAT
     *    -  The item must also, therefore have it's major version as
     *       v12::FORMAT_MAJOR
     *   @param rhs -The item we're making a differentiated ring item from.
     *   @return CDataFormatItem* - pointer to the newly created item.
     *   @throw std::bad_cast - if one of the tests above fails.
     */
    ::ufmt::CDataFormatItem*
    RingItemFactory::makeDataFormatItem(const ::ufmt::CRingItem& rhs)
    {
        if (rhs.type() != v12::RING_FORMAT) {
            throw std::bad_cast();
        }
        // This can throw as well:
        
        const v12::DataFormat* pItem =
            reinterpret_cast<const v12::DataFormat*>(rhs.getItemPointer());
        
        if (pItem->s_majorVersion != v12::FORMAT_MAJOR) {
            throw std::bad_cast();
        }
        return new v12::CDataFormatItem();        // No actual differentiation.
    }
    /**
     * makeGlomParameters
     *    Make a glom parameters ring item from the actual parameters.
     * @param interval - build interval.
     * @param isBuilding - True if glom is building events
     * @param policy    - Timestamp policy which is one of
     *      *  v12::GLOM_TIMESTAMP_FIRST  output timestamp from first fragment
     *      *  v12::GLOM_TIMESTAMP_LAST   Output timestamp  from last fragment.
     *      *  v12::GLOM_TIMESTAMP_AVERAGE Output timestamp average of all fragments.
     * @return ::CGlomParameters* - pointer to a newly created glom parameters item.
     */
    ::ufmt::CGlomParameters*
    RingItemFactory::makeGlomParameters(
        uint64_t interval, bool isBuilding, uint16_t policy
    )
    {
        // Validate the policy:
        
        ::ufmt::CGlomParameters::TimestampPolicy policySelector;
        switch (policy) {
            case v12::GLOM_TIMESTAMP_FIRST:
                policySelector = ::ufmt::CGlomParameters::first;
                break;
            case v12::GLOM_TIMESTAMP_LAST:
                policySelector = ::ufmt::CGlomParameters::last;
                break;
            case v12::GLOM_TIMESTAMP_AVERAGE:
                policySelector = ::ufmt::CGlomParameters::average;
                break;
            default:
                throw std::invalid_argument("Invalid timestamp policy value");
        }
        return new v12::CGlomParameters(interval, isBuilding, policySelector);
    }
    /**
     * makeGlomParameters
     *    Create a CGlomParameters object from some undifferentiated ring item.
     * @param rhs - reference to the source item.
     * @throw std::bad_cast if: rhs is not a v12::EVB_GLOM_INFO type item.
     *             or if the size of the item is not sizeof(v12::GlomParametrs).
     */
    ::ufmt::CGlomParameters*
    RingItemFactory::makeGlomParameters(const ::ufmt::CRingItem& rhs)
    {
        if (rhs.type() != EVB_GLOM_INFO) {
            throw std::bad_cast();
        }
        if (rhs.size() != sizeof(v12::GlomParameters)) {
            throw std::bad_cast();
        }
        
        const v12::GlomParameters* pItem =
            reinterpret_cast<const v12::GlomParameters*>(rhs.getItemPointer());
        
        
        // Make the new item:
        
        return new v12::CGlomParameters(
            pItem->s_coincidenceTicks, pItem->s_isBuilding,
            static_cast<::ufmt::CGlomParameters::TimestampPolicy>(pItem->s_timestampPolicy)
        );
    }
    /**
     * makePhysicsEventItem
     *    Create a physics event item.  This will return a pointer to an
     *    item without a body header that you have to fill by getting the body
     *    cursor, resetting it when done an invoking updateSize() to set the size.
     * @param maxbody - the maximum size of the body that you'll be able to fill without
     *                   'bad' things happening.
     * @return ::CPhysicsEventItem* - pointer to the newly created item.
     */
    ::ufmt::CPhysicsEventItem*
    RingItemFactory::makePhysicsEventItem(size_t maxBody)
    {
        return new v12::CPhysicsEventItem(maxBody);
    }
    /**
     * makePhysicsEventITem
     *    Same as above but the empty item has a body header determined by the
     *    parameters:
     * @param timestamp - body header timestamp.
     * @param source    - Body header source id.
     * @param barrier   - Barrier type (usually 0).
     * @param maxBody   - Size of largest possible body.
     * @return ::CPhysicsEventItem* - pointer to the newly created item.
     */
    ::ufmt::CPhysicsEventItem*
    RingItemFactory::makePhysicsEventItem(
        uint64_t timestamp, uint32_t source, uint32_t barrier, size_t maxBody
    )
    {
        return new v12::CPhysicsEventItem(
            timestamp, source, barrier, maxBody
        );
    }
    /**
     * makePhysicsEventItem
     *    Make an item from the contents of an undifferentiated ring item.
     *    - the source type must be v12::PHYSICS_EVENT.
     * @param rhs - Reference to the source item.
     * @return ::CPhysicsEventItem*
     * @throw std::bad_cast if the source item is not a physics item.
     */
    ::ufmt::CPhysicsEventItem*
    RingItemFactory::makePhysicsEventItem(const ::ufmt::CRingItem& rhs)
    {
        if (rhs.type() != v12::PHYSICS_EVENT) {
            throw std::bad_cast();
        }
        v12::CPhysicsEventItem* pResult = new v12::CPhysicsEventItem(rhs.size());
        
        uint8_t* pDest = reinterpret_cast<uint8_t*>(pResult->getItemPointer());
        
        const uint8_t* pSrc = reinterpret_cast<const uint8_t*>(rhs.getItemPointer());
        memcpy(pDest, pSrc, rhs.size());
        pDest += rhs.size();
        pResult->setBodyCursor(pDest);
        pResult->updateSize();
        
        return pResult;
    }
    /**
     * makeRingFragmentItem
     *    Create a ring fragment item from parameters:
     *  @param timestamp - body header timestamp.
     *  @param source    - body header source id.
     *  @param payloadsize - size of the fragment payload
     *  @param payload - pointer to the data to put in the payload.
     *  @param barrier   - Body header barrier type.
     *  @return ::CRingFragmentItem*
     */
    ::ufmt::CRingFragmentItem*
    RingItemFactory::makeRingFragmentItem(
        uint64_t timestamp, uint32_t source, uint32_t payloadSize,
        const void* payload, uint32_t barrier
    )
    {
        return new v12::CRingFragmentItem(timestamp, source, payloadSize, payload, barrier);
    }
    /**
     * makeRingFragmentItem
     *    Crete a ring fragment item from an undifferentiated ring item.
     * @param rhs - reference to the source item.
     * @return ::CRingFragmentItem*
     */
    ::ufmt::CRingFragmentItem*
    RingItemFactory::makeRingFragmentItem(const ::ufmt::CRingItem& rhs)
    {
        if ((rhs.type() != v12::EVB_FRAGMENT) && (rhs.type() != v12::EVB_UNKNOWN_PAYLOAD)) {
            throw std::bad_cast();
        }
        if (rhs.size() < sizeof(v12::EventBuilderFragment)) {
            throw std::bad_cast();
        }
        const v12::EventBuilderFragment* pItem =
            reinterpret_cast<const v12::EventBuilderFragment*>(rhs.getItemPointer());
        return makeRingFragmentItem(
            pItem->s_bodyHeader.s_timestamp, pItem->s_bodyHeader.s_sourceId,
            rhs.size() - sizeof(v12::EventBuilderFragment), pItem->s_body,
            pItem->s_bodyHeader.s_barrier
        );
    }
    /**
     * makePhysicsEventCountItem
     *   Make one of these from the parameters
     *   @param count - triggers.
     *   @param offset - run offset.
     *   @stamp   - clock time.
     *   @param divisor - offset divisor.
     *   @return ::CRingPhysicsEventCountItem*
     */
    ::ufmt::CRingPhysicsEventCountItem*
    RingItemFactory::makePhysicsEventCountItem(
        uint64_t count, uint32_t timeoffset, time_t stamp, int divisor
    )
    {
        return new v12::CRingPhysicsEventCountItem(count, timeoffset, stamp, 0, divisor);
    }
    /**
     * makePhysicsEventCountItem
     *     Makes one by copying an undifferentiated item that's actually a count item.
     * @param rhs - references the item we're copying.
     * @return ::CRingPhysicsEventCountItem*
     * @throw std::bad_cast - wrong type or size too small.
     */
    ::ufmt::CRingPhysicsEventCountItem*
    RingItemFactory::makePhysicsEventCountItem(const ::ufmt::CRingItem& rhs)
    {
        if (rhs.type() != v12::PHYSICS_EVENT_COUNT) {
            throw std::bad_cast();
        }
        const v12::PhysicsEventCountItem* pItem =
            reinterpret_cast<const v12::PhysicsEventCountItem*>(rhs.getItemPointer());
        
        // Could have a body header:
        
        if (pItem->s_body.u_noBodyHeader.s_empty > sizeof(uint32_t)) {
            const v12::BodyHeader* pH = &(pItem->s_body.u_hasBodyHeader.s_bodyHeader);
            const v12::PhysicsEventCountItemBody* pBody = &(pItem->s_body.u_hasBodyHeader.s_body);
            
            // Make the base item:
            
            auto result = makePhysicsEventCountItem(
                pBody->s_eventCount, pBody->s_timeOffset, pBody->s_timestamp,
                pBody->s_offsetDivisor
            );
            // Add in the body header:
            
            result->setBodyHeader(pH->s_timestamp, pH->s_sourceId, pH->s_barrier);
            return result;
        } else {
            const v12::PhysicsEventCountItemBody* pBody = &(pItem->s_body.u_noBodyHeader.s_body);
            return makePhysicsEventCountItem(
                pBody->s_eventCount, pBody->s_timeOffset, pBody->s_timestamp,
                pBody->s_offsetDivisor
            );
        }
        
    }
    /**
     * makeScalerItem
     *    Make empty scaler item with just num scalers.  Object's methods must then
     *    be used to fill it in.
     * @param numScalers - number of scaler counters.
     * @return ::CRingScalerItem*
     */
    ::ufmt::CRingScalerItem*
    RingItemFactory::makeScalerItem(size_t numScalers)
    {
        return new v12::CRingScalerItem(numScalers);
    }
    /**
     * makeScalrItem
     *    Make scaler item from parameters:
     * @param startTime - Counting interval start time relative to the run start.
     * @param endTime   - COunting interval end time.
     * @param timestamp - CLock time of the end of the interval.
     * @param scalers   - Scaler values.
     * @param isIncremental - true if scalers are cleared after each read.
     * @param sid        - Original source id.
     * @param timeOffsetDivisor - startTime/timeOffsetDivisor (floating) is seconds.
     * @note A fairely empty body header is created that can be filled in completely with
     * setBodyHeader - however sid will remaina s the original source id.
     * @return ::CRingScalerItem*
     */
    ::ufmt::CRingScalerItem*
    RingItemFactory::makeScalerItem(
        uint32_t startTime,
        uint32_t stopTime,
        time_t   timestamp,
        std::vector<uint32_t> scalers,
        bool                  isIncremental,
        uint32_t              sid,
        uint32_t              timeOffsetDivisor
    )
    {
        return new v12::CRingScalerItem(
            0xffffffffffffffff, sid, 0,
            startTime,stopTime, timestamp, scalers, timeOffsetDivisor, isIncremental
        );
    }
    /**
     * makeScalerItem
     *    From an undifferentiated scaler item.
     * @param rhs - rference to a ::ufmt::CRingItem.
     * @return ::CRingScalerItem*
     */
    ::ufmt::CRingScalerItem*
    RingItemFactory::makeScalerItem(const ::ufmt::CRingItem& rhs)
    {
        if (rhs.type() != v12::PERIODIC_SCALERS) {
            throw std::bad_cast();
        }
        const v12::ScalerItem* pItem =
            reinterpret_cast<const v12::ScalerItem*>(rhs.getItemPointer());
            
        if (pItem->s_body.u_noBodyHeader.s_empty > sizeof(uint32_t)) {
            const v12::BodyHeader* pH = &(pItem->s_body.u_hasBodyHeader.s_bodyHeader);
            const v12::ScalerItemBody* pBody = &(pItem->s_body.u_hasBodyHeader.s_body);
            
            // First make a scaler item and then add the body header:
            
            std::vector<uint32_t> scalers(pBody->s_scalers, pBody->s_scalers+pBody->s_scalerCount);
            auto pResult = makeScalerItem(
                pBody->s_intervalStartOffset, pBody->s_intervalEndOffset,
                pBody->s_timestamp, scalers, pBody->s_isIncremental,
                pBody->s_originalSid, pBody->s_intervalDivisor
            );
            pResult->setBodyHeader(pH->s_timestamp, pH->s_sourceId, pH->s_barrier);
            return pResult;
        } else {
            const v12::ScalerItemBody* pBody = &(pItem->s_body.u_noBodyHeader.s_body);
            std::vector<uint32_t> scalers(pBody->s_scalers, pBody->s_scalers+pBody->s_scalerCount);
            return makeScalerItem(
                pBody->s_intervalStartOffset, pBody->s_intervalEndOffset,
                pBody->s_timestamp, scalers, pBody->s_isIncremental,
                pBody->s_originalSid, pBody->s_intervalDivisor
            );
            
        }
        
    }
    /**
     * makeTextItem
     *    Create a text item with some strings  and a type.
     *    The type must be one of v12::PACKET_TYPES, v12::MONITORED_VARIABLES
     *    or v12::RING_FORMAT
     *  @param type -Item type; see above for restrictions.
     *  @param theStrings - vector of strings to put in the item.
     *  @return ::CRingTextItem*
     *  
     */
    ::ufmt::CRingTextItem*
    RingItemFactory::makeTextItem(
        uint16_t type, std::vector<std::string> theStrings
    )
    {
        
        if (validTextItemTypes.count(type) == 0) {
            throw std::invalid_argument("Invalid text item type (v12)");
        }
        return new v12::CRingTextItem(type, theStrings);
    }
    /**
     *  makeTextItem
     *     @param type  - text item type.
     *     @param theStrings - the text strings.
     *     @param offsetTime - Time offset into the run.
     *     @param timestamp - clock time at which item was made.
     *     @param divisor   - # offsetTimeTicks in a second.
     *     @return ::CRingTextItem*
     */
    ::ufmt::CRingTextItem*
    RingItemFactory::makeTextItem(
        uint16_t type,
        std::vector<std::string> theStrings,
        uint32_t                 offsetTime,
        time_t                   timestamp,
        uint32_t divisor
    )
    {
        if (validTextItemTypes.count(type) == 0) {
            throw std::invalid_argument("Invalid text time type (v12)");
        }
        return new v12::CRingTextItem(
            type, theStrings, offsetTime, timestamp, divisor
        );
    }
    /**
     * makeTextItem
     *    Create a text item from an undifferentiated item. Note that while
     *    the factory cannot make an item with a body header, we don't assume
     *    the resulting text item has no body header.
     * @param rhs - const reference to the item that will be copy casted into a
     *       v12::CRingTextITem
     * @return ::CRingTextItem*
     */
    ::ufmt::CRingTextItem*
    RingItemFactory::makeTextItem(const ::ufmt::CRingItem& rhs)
    {
        if (validTextItemTypes.count(rhs.type()) == 0) {
            throw std::bad_cast();
        }
        const v12::TextItem* pItem =
            reinterpret_cast<const v12::TextItem*>(rhs.getItemPointer());
        if (pItem->s_body.u_noBodyHeader.s_empty > sizeof(uint32_t)) {
            const v12::BodyHeader* pH = &(pItem->s_body.u_hasBodyHeader.s_bodyHeader);
            const v12::TextItemBody* pBody = &(pItem->s_body.u_hasBodyHeader.s_body);
            auto strings = marshallStrings(pBody);
            auto result = makeTextItem(
                rhs.type(), strings, pBody->s_timeOffset, pBody->s_timestamp,
                pBody->s_offsetDivisor
            );
            result->setBodyHeader(pH->s_timestamp, pH->s_sourceId, pH->s_barrier);
            return result;
        } else {
            const v12::TextItemBody* pBody = &(pItem->s_body.u_noBodyHeader.s_body);
            auto strings = marshallStrings(pBody);
            return makeTextItem(
                rhs.type(), strings, pBody->s_timeOffset, pBody->s_timestamp,
                pBody->s_offsetDivisor
            );
        }
        
    }
    /**
     * makeUnknownFragment
     *     Create a RingFragmentItem from its parameterization with
     *     an unknown payload type.  Normal ring fragments have payloads
     *     that can be assumed to be RingItems.  These cannot be assumed
     *     to have payloads of any recognizable shape.
     * @param timestamp - body header timestamp.
     * @param sourceid  - body header source id.
     * @param barrier   - body header barrier code.
     * @param size      - size of the payload.
     * @param pPayload   - pointer to the paylaod.
     * @return ::CUnknownFragment* - dynamically allocated.
     */
    ::ufmt::CUnknownFragment*
    RingItemFactory::makeUnknownFragment(
            uint64_t timestamp, uint32_t sourceid, uint32_t barrier,
            uint32_t size, void* pPayload
    )
    {
        return reinterpret_cast<::ufmt::CUnknownFragment*> (new 
            v12::CUnknownFragment(timestamp, sourceid, barrier, size, pPayload));
    }
    /**
     * makeUnknownFragment
     *    Create an unknown fragment from a normal ring item.
     *    - The ring item must have type v12::EVB_UNKNOWN_PAYLOAD
     *    - The size of the fragment must be at least that of a
     *      ring fragment item (zero payload item e.g).
     * @param rhs - the ring itemt that's being constructe into an
     *              unknown fragment type.
     * @return ::CUnknownFragment* - unknown fragment dynamically created.
     */
    ::ufmt::CUnknownFragment*
    RingItemFactory::makeUnknownFragment(const ::ufmt::CRingItem& rhs)
    {
        if (rhs.type() != EVB_UNKNOWN_PAYLOAD) {
            throw std::bad_cast();
        }
        if (rhs.size() < sizeof(v12::EventBuilderFragment)) {
            throw std::bad_cast();
        }
        
        
        const v12::EventBuilderFragment* pItem =
            reinterpret_cast<const v12::EventBuilderFragment*>(rhs.getItemPointer());
            
        size_t payloadSize = rhs.size() - sizeof(v12::EventBuilderFragment);
        
        return makeUnknownFragment(
            pItem->s_bodyHeader.s_timestamp, pItem->s_bodyHeader.s_sourceId,
            pItem->s_bodyHeader.s_barrier,
            payloadSize, const_cast<uint8_t*>(pItem->s_body)
        );
    }

    /**
     * makeStateChangeItem
     *    Make state change item from parameterized construction.
     *  @param itemType - type of item e.g. v12::BEGIN_RUN
     *  @param runNumber - Number of the run unddergoing the transition.
     *  @param timeOffset - Offset in to the run at which the transition occured.
     *  @param timestamp - time of day at which the transition occured.
     *  @param title     - Run title.
     *  @return ::CRingSTateChangeItem* - dynamically allocated.
     * 
     */
    ::ufmt::CRingStateChangeItem*
    RingItemFactory::makeStateChangeItem(
        uint32_t itemType, uint32_t runNumber, uint32_t timeOffset, time_t timestamp,
        std::string title
    )
    {
        return new v12::CRingStateChangeItem(
            itemType, runNumber, timeOffset, timestamp, title
        );
    }
    /**
     * makeStateChangeItem
     *    Create a state change item from an undifferentiated item
     *    which must be a valid state change item.
     *     - Item must be a valid state change item.
     *     - Item must have the correct size.
     *  @param rhs - the undifferentiated Ring item we're constructing
     *        our item from.
     *  @return ::CRingStateChangeItem* - dynamically allocated.
     *  
     */
    ::ufmt::CRingStateChangeItem*
    RingItemFactory::makeStateChangeItem(const ::ufmt::CRingItem& rhs)
    {
        if (!CRingStateChangeItem::isStateChange(rhs.type())) {
            throw std::bad_cast();
        }
        // There are two valid sizes:
        
        size_t nobheaderSize = sizeof(v12::RingItemHeader) + sizeof(uint32_t) +
            sizeof(v12::StateChangeItemBody);
        size_t bodyHeaderSize = sizeof(v12::RingItemHeader) + sizeof(v12::BodyHeader) +
            sizeof(v12::StateChangeItemBody);
        if ((rhs.size() != nobheaderSize) && (rhs.size() != bodyHeaderSize)) {
            throw std::bad_cast();
        }
        
        const v12::StateChangeItemBody* pBody =
            reinterpret_cast<const v12::StateChangeItemBody*>(rhs.getBodyPointer());
        if (rhs.hasBodyHeader()) {
            return new v12::CRingStateChangeItem(
                rhs.getEventTimestamp(), rhs.getSourceId(), rhs.getBarrierType(),
                rhs.type(), pBody->s_runNumber, pBody->s_timeOffset,
                pBody->s_Timestamp, pBody->s_title, pBody->s_offsetDivisor
            );
        } else {
            return new v12::CRingStateChangeItem(
                rhs.type(), pBody->s_runNumber, pBody->s_timeOffset,
                pBody->s_Timestamp,
                pBody->s_title, pBody->s_offsetDivisor
            );
        }
        
    }
    ///////////////////////////////////// Private methds ///////////////////////

    std::vector<std::string>
    RingItemFactory::marshallStrings(const void* p)
    {
        const v12::TextItemBody* pBody = reinterpret_cast<const v12::TextItemBody*>(p);
        const char* pString = pBody->s_strings;
        std::vector<std::string> result;
        for (int i = 0; i < pBody->s_stringCount; i++) {
            std::string s(pString);
            result.push_back(s);
            pString += s.size() + 1;
        }
        return result;
    }
    //// end of v12 namespace
    }
}
