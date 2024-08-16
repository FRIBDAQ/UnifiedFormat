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

/** @file:  RingItemFactory.h (v11).
 *  @brief: RingItem factory class for version 11 items.
 */
#ifndef V11_RINGITEMFACTORY
#define V11_RiNGITEMFACTORY

#include <RingItemFactoryBase.h>

namespace ufmt {
    namespace v11 {
        
    /**
     * @class v11::RingItemFactory
     *    Provides factory methods for v11 ring items.
     */
    class RingItemFactory : public ::ufmt::RingItemFactoryBase
    {
    public:
        virtual ::ufmt::CRingItem* makeRingItem(uint16_t type, size_t maxBody) ;
        virtual ::ufmt::CRingItem* makeRingItem(uint16_t type, uint64_t timestamp, uint32_t sourceId,
                size_t maxBody, uint32_t barrierType = 0 ) ;
        virtual ::ufmt::CRingItem* makeRingItem(const ::ufmt::CRingItem& rhs) ;
        virtual ::ufmt::CRingItem* makeRingItem(const ::ufmt::RingItem* pRawRing) ;

    #ifdef HAVE_NSCLDAQ    
        virtual ::ufmt::CRingItem* getRingItem(::ufmt::CRingBuffer& ringbuf) ;
    #endif    
        virtual ::ufmt::CRingItem* getRingItem(int fd) ;
        virtual ::ufmt::CRingItem* getRingItem(std::istream& in) ;
        
        virtual std::ostream& putRingItem(const ::ufmt::CRingItem* pItem, std::ostream& out) ;
        virtual void putRingItem(const ::ufmt::CRingItem* pItem, int fd) ;
    #ifdef HAVE_NSCLDAQ    
        virtual void putRingItem(const ::ufmt::CRingItem* pItem, ::ufmt::CRingBuffer& ringbuf) ;
    #endif
        virtual ::ufmt::CAbnormalEndItem* makeAbnormalEndItem() ;
        virtual ::ufmt::CAbnormalEndItem* makeAbnormalEndItem(const ::ufmt::CRingItem& rhs) ;

        virtual ::ufmt::CDataFormatItem* makeDataFormatItem() ;
        virtual ::ufmt::CDataFormatItem* makeDataFormatItem(const ::ufmt::CRingItem& rhs) ;

        virtual ::ufmt::CGlomParameters* makeGlomParameters(
            uint64_t interval, bool isBuilding, uint16_t policy
        )  ;
        virtual ::ufmt::CGlomParameters* makeGlomParameters(const ::ufmt::CRingItem& rhs) ;

        virtual ::ufmt::CPhysicsEventItem* makePhysicsEventItem(size_t maxBody) ;
        virtual ::ufmt::CPhysicsEventItem* makePhysicsEventItem(
            uint64_t timestamp, uint32_t source, uint32_t barrier, size_t maxBody
        ) ;
        virtual ::ufmt::CPhysicsEventItem* makePhysicsEventItem(const ::ufmt::CRingItem& rhs) ;

        virtual ::ufmt::CRingFragmentItem* makeRingFragmentItem(
            uint64_t timestamp, uint32_t source, uint32_t payloadSize,
            const void* payload, uint32_t barrier=0
        ) ;
        virtual ::ufmt::CRingFragmentItem* makeRingFragmentItem(const ::ufmt::CRingItem& rhs) ;


        virtual ::ufmt::CRingPhysicsEventCountItem* makePhysicsEventCountItem(
            uint64_t count, uint32_t timeoffset, time_t stamp,
        int divisor=1
        ) ;
        virtual ::ufmt::CRingPhysicsEventCountItem* makePhysicsEventCountItem(const ::ufmt::CRingItem& rhs) ;

        virtual ::ufmt::CRingScalerItem* makeScalerItem(size_t numScalers) ;
        virtual ::ufmt::CRingScalerItem* makeScalerItem(
            uint32_t startTime,
            uint32_t stopTime,
            time_t   timestamp,
            std::vector<uint32_t> scalers,
            bool                  isIncremental = true,
            uint32_t              sid = 0,
            uint32_t              timeOffsetDivisor = 1
        ) ;
        virtual ::ufmt::CRingScalerItem* makeScalerItem(const ::ufmt::CRingItem& rhs) ;

        virtual ::ufmt::CRingTextItem* makeTextItem(
            uint16_t type,
                    std::vector<std::string> theStrings
        ) ;
        virtual ::ufmt::CRingTextItem* makeTextItem(
            uint16_t type,
            std::vector<std::string> theStrings,
            uint32_t                 offsetTime,
            time_t                   timestamp, uint32_t divisor=1
        ) ;
        virtual ::ufmt::CRingTextItem* makeTextItem(const ::ufmt::CRingItem& rhs) ;

        virtual ::ufmt::CUnknownFragment* makeUnknownFragment(
            uint64_t timestamp, uint32_t sourceid, uint32_t barrier,
            uint32_t size, void* pPayload
        ) ;
        virtual ::ufmt::CUnknownFragment* makeUnknownFragment(const ::ufmt::CRingItem& rhs) ;

        virtual ::ufmt::CRingStateChangeItem* makeStateChangeItem(
            uint32_t itemType, uint32_t runNumber,
            uint32_t timeOffset,
            time_t   timestamp,
            std::string title
        ) ;
        virtual ::ufmt::CRingStateChangeItem* makeStateChangeItem(const ::ufmt::CRingItem& rhs) ;
    private:
        std::vector<std::string> marshallStrings(const void* p);
        
    };
    }

}
#endif