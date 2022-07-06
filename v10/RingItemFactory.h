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

/** @file:  RingItemFactory.h
 *  @brief: Version 10 Ring item factory.
 */
#ifndef V10_RINGITEMFACTORY_H
#define V10_RINGITEMFACTORY_H
#include <RingItemFactoryBase.h>


namespace v10 {
    /**
     * @class v10::RingItemFactory
     *    V10 specialization of the ring item factory
     */
    class RingItemFactory : public RingItemFactoryBase {
    public:
        // Raw ring items for 10.x:
        
        ::CRingItem* makeRingItem(uint16_t type, size_t maxBody);
        ::CRingItem* makeRingItem(uint16_t type, uint64_t timestamp, uint32_t sourceId,
                size_t maxBody, uint32_t barrierType = 0 );
        ::CRingItem* makeRingItem(const ::CRingItem& rhs);
        ::CRingItem* makeRingItem(const RingItem* pRawRing);
#if NSCLDAQ_ROOT != '_'  
        virtual ::CRingItem* getRingItem(CRingBuffer& ringbuf) ;
#endif
        virtual ::CRingItem* getRingItem(int fd) ;
        virtual ::CRingItem* getRingItem(std::istream& in);
        
        virtual std::ostream& putRingItem(const ::CRingItem* pItem, std::ostream& out) ;
        virtual void putRingItem(const ::CRingItem* pItem, int fd) ;
#if NSCLDAQ_ROOT != '_'  
        virtual void putRingItem(const ::CRingItem* pItem, CRingBuffer& ringbuf) ;
#endif
        // abnormal end items for 10.x:
        
        virtual ::CAbnormalEndItem* makeAbnormalEndItem() ;
        virtual ::CAbnormalEndItem* makeAbnormalEndItem(const CRingItem& rhs) ;
        
        // Data format items for 10.x
    
        virtual ::CDataFormatItem* makeDataFormatItem() ;
        virtual ::CDataFormatItem* makeDataFormatItem(const CRingItem& rhs);
        
        // GLom parameter items for 10.x
        
        virtual ::CGlomParameters* makeGlomParameters(
            uint64_t interval, bool isBuilding, uint16_t policy
        );
        virtual ::CGlomParameters* makeGlomParameters(const CRingItem& rhs) ;
        
        // Physics event items:
        
        virtual ::CPhysicsEventItem* makePhysicsEventItem(size_t maxBody) ;
        virtual ::CPhysicsEventItem* makePhysicsEventItem(
            uint64_t timestamp, uint32_t source, uint32_t barrier,
            size_t maxBody
        ) ;
        virtual ::CPhysicsEventItem* makePhysicsEventItem(const ::CRingItem& rhs) ;
        
        // RingFragment items (not supported in v10):
        
        virtual ::CRingFragmentItem* makeRingFragmentItem(
            uint64_t timestamp, uint32_t source, uint32_t payloadSize,
            const void* payload, uint32_t barrier=0
        ) ;
        virtual ::CRingFragmentItem* makeRingFragmentItem(const ::CRingItem& rhs) ;
    
        // Event count items.
        
        virtual ::CRingPhysicsEventCountItem* makePhysicsEventCountItem(
            uint64_t count, uint32_t timeoffset, time_t stamp,
            int divisor=1
        );
        virtual ::CRingPhysicsEventCountItem* makePhysicsEventCountItem(const ::CRingItem& rhs);
        
        // Scaler items:
        
        virtual ::CRingScalerItem* makeScalerItem(size_t numScalers);
        virtual ::CRingScalerItem* makeScalerItem(
            uint32_t startTime,
            uint32_t stopTime,
            time_t   timestamp,
            std::vector<uint32_t> scalers,
            bool                  isIncremental = true,
            uint32_t              sid = 0,
            uint32_t              timeOffsetDivisor = 1
        );
        virtual ::CRingScalerItem* makeScalerItem(const CRingItem& rhs);
        
        // Text items:
        
        
        
        virtual CRingTextItem* makeTextItem(
            uint16_t type,
            std::vector<std::string> theStrings
        );
        virtual CRingTextItem* makeTextItem(
            uint16_t type,
            std::vector<std::string> theStrings,
            uint32_t                 offsetTime,
            time_t                   timestamp, uint32_t divisor=1
        );
        virtual CRingTextItem* makeTextItem(const CRingItem& rhs);
        
        // unknown fragments.
        
        virtual CUnknownFragment* makeUnknownFragment(
            uint64_t timestamp, uint32_t sourceid, uint32_t barrier,
            uint32_t size, void* pPayload
        );
        virtual CUnknownFragment* makeUnknownFragment(const CRingItem& rhs);
        
        // state change items.
        
        virtual CRingStateChangeItem* makeStateChangeItem(
            uint32_t itemType, uint32_t runNumber,
            uint32_t timeOffset,
            time_t   timestamp,
            std::string title
        );
        virtual CRingStateChangeItem* makeStateChangeItem(const CRingItem& rhs);
    
        
    private:
        static bool isValidTextItemType(uint32_t itemType);
        static std::vector<std::string> stringsToVector(
            uint32_t nStrings, const char* pStrings
        );
        
        static bool isValidStateChangeType(uint32_t reason);
    };
        
        
        

}                            // V10

#endif