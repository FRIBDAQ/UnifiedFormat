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

#ifdef HAVE_NSCLDAQ
class CRingBuffer;
#endif

namespace ufmt {
    namespace v10 {
        /**
         * @class v10::RingItemFactory
         *    V10 specialization of the ring item factory
         */
        class RingItemFactory : public RingItemFactoryBase {
        public:
            // Raw ring items for 10.x:
            
            ::ufmt::CRingItem* makeRingItem(uint16_t type, size_t maxBody);
            ::ufmt::CRingItem* makeRingItem(uint16_t type, uint64_t timestamp, uint32_t sourceId,
                    size_t maxBody, uint32_t barrierType = 0 );
            ::ufmt::CRingItem* makeRingItem(const ::ufmt::CRingItem& rhs);
            ::ufmt::CRingItem* makeRingItem(const ::ufmt::RingItem* pRawRing);
    #ifdef HAVE_NSCLDAQ  
            virtual ::ufmt::CRingItem* getRingItem(::CRingBuffer& ringbuf) ;
    #endif
            virtual ::ufmt::CRingItem* getRingItem(int fd) ;
            virtual ::ufmt::CRingItem* getRingItem(::std::istream& in);
            
            virtual ::std::ostream& putRingItem(const ::ufmt::CRingItem* pItem, ::std::ostream& out) ;
            virtual void putRingItem(const ::ufmt::CRingItem* pItem, int fd) ;
    #ifdef HAVE_NSCLDAQ  
            virtual void putRingItem(const ::ufmt::CRingItem* pItem, ::CRingBuffer& ringbuf) ;
    #endif
            // abnormal end items for 10.x:
            
            virtual ::ufmt::CAbnormalEndItem* makeAbnormalEndItem() ;
            virtual ::ufmt::CAbnormalEndItem* makeAbnormalEndItem(const CRingItem& rhs) ;
            
            // Data format items for 10.x
        
            virtual ::ufmt::CDataFormatItem* makeDataFormatItem() ;
            virtual ::ufmt::CDataFormatItem* makeDataFormatItem(const ::ufmt::CRingItem& rhs);
            
            // GLom parameter items for 10.x
            
            virtual ::ufmt::CGlomParameters* makeGlomParameters(
                uint64_t interval, bool isBuilding, uint16_t policy
            );
            virtual ::ufmt::CGlomParameters* makeGlomParameters(const ::ufmt::CRingItem& rhs) ;
            
            // Physics event items:
            
            virtual ::ufmt::CPhysicsEventItem* makePhysicsEventItem(size_t maxBody) ;
            virtual ::ufmt::CPhysicsEventItem* makePhysicsEventItem(
                uint64_t timestamp, uint32_t source, uint32_t barrier,
                size_t maxBody
            ) ;
            virtual ::ufmt::CPhysicsEventItem* makePhysicsEventItem(const ::ufmt::CRingItem& rhs) ;
            
            // RingFragment items (not supported in v10):
            
            virtual ::ufmt::CRingFragmentItem* makeRingFragmentItem(
                uint64_t timestamp, uint32_t source, uint32_t payloadSize,
                const void* payload, uint32_t barrier=0
            ) ;
            virtual ::ufmt::CRingFragmentItem* makeRingFragmentItem(const ::ufmt::CRingItem& rhs) ;
        
            // Event count items.
            
            virtual ::ufmt::CRingPhysicsEventCountItem* makePhysicsEventCountItem(
                uint64_t count, uint32_t timeoffset, time_t stamp,
                int divisor=1
            );
            virtual ::ufmt::CRingPhysicsEventCountItem* makePhysicsEventCountItem(const ::ufmt::CRingItem& rhs);
            
            // Scaler items:
            
            virtual ::ufmt::CRingScalerItem* makeScalerItem(size_t numScalers);
            virtual ::ufmt::CRingScalerItem* makeScalerItem(
                uint32_t startTime,
                uint32_t stopTime,
                time_t   timestamp,
                ::std::vector<uint32_t> scalers,
                bool                  isIncremental = true,
                uint32_t              sid = 0,
                uint32_t              timeOffsetDivisor = 1
            );
            virtual ::ufmt::CRingScalerItem* makeScalerItem(const ::ufmt::CRingItem& rhs);
            
            // Text items:
            
            
            
            virtual ::ufmt::CRingTextItem* makeTextItem(
                uint16_t type,
                ::std::vector<::std::string> theStrings
            );
            virtual ::ufmt::CRingTextItem* makeTextItem(
                uint16_t type,
                ::std::vector<::std::string> theStrings,
                uint32_t                 offsetTime,
                time_t                   timestamp, uint32_t divisor=1
            );
            virtual ::ufmt::CRingTextItem* makeTextItem(const ::ufmt::CRingItem& rhs);
            
            // unknown fragments.
            
            virtual ::ufmt::CUnknownFragment* makeUnknownFragment(
                uint64_t timestamp, uint32_t sourceid, uint32_t barrier,
                uint32_t size, void* pPayload
            );
            virtual ::ufmt::CUnknownFragment* makeUnknownFragment(const ::ufmt::CRingItem& rhs);
            
            // state change items.
            
            virtual ::ufmt::CRingStateChangeItem* makeStateChangeItem(
                uint32_t itemType, uint32_t runNumber,
                uint32_t timeOffset,
                time_t   timestamp,
                ::std::string title
            );
            virtual ::ufmt::CRingStateChangeItem* makeStateChangeItem(const ::ufmt::CRingItem& rhs);
        
            
        private:
            static bool isValidTextItemType(uint32_t itemType);
            static ::std::vector<::std::string> stringsToVector(
                uint32_t nStrings, const char* pStrings
            );
            
            static bool isValidStateChangeType(uint32_t reason);
        };
            
            
            

    }                            // V10
}
#endif