/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  RingItemFactoryBase.h
 *  @brief: Base class for abstract factory of ring item wrapper classes.
 */
#ifndef RINGITEMFACTORYBASE_H
#define RINGITEMFACTORYBASE_H
#include <DataFormat.h>
#include <stdint.h>
#include <iostream>
// Forward definitions

class CRingBuffer;

class CRingItem;
class CAbnormalEndItem;
class CDataFormatItem;
class CGlomParameters;
class CPhysicsEventItem;
class CRingFragmentItem;
class CRingPhysicsEventCountItem;
class CRingScalerItem;
class CRingTextItem;
class CUnknownFragment;
class CRingStateChangeItem;

/**
 * RingItemFactoryBase
 *    This is an abstract base class for a ring item factory.
 *    Each ring item data format will have its factory class which
 *    can instantiate all members of the ring item class hierarchy.
 *
 *   
 */
class RingItemFactoryBase {
    CRingItem* makeRingItem(uint16_t type, size_t maxBody) = 0;
    CRingItem* makeRingItem(uint16_t type, uint64_t timestamp, uint32_t sourceId,
            size_t maxBody, uint32_t barrierType = 0 ) = 0;
    CRingItem* makeRingItem(const CRingItem& rhs) = 0;
    CRingItem* makeRingItem(const RingItem* pRawRing) = 0;
    
    
    CRingItem* getRingItem(CRingBuffer& ringbuf) = 0;
    CRingItem* getRingItem(int fd) = 0;
    CRingItem* getRingItem(std::istream& in) = 0;
    
    std::ostream& putRingItem(CRingItem* pItem, std::ostream& out) = 0;
    void putRingItem(CRingItem* pItem, int fd);
    void putRingItem(CRingItem* pItem, CRingBuffer& ringbuf) = 0;
    
    CAbnormalEndItem* makeAbnormalEndItem() = 0;
    CAbnormalEndItem* makeAbnormalEndItem(const CRingItem& rhs) = 0;
    
    CDataFormatItem* makeDataFormatItem() = 0;
    CDataFormatItem* makeDataFormatItem(const CRingItem& rhs) = 0;
    
    CGlomParameters* makeGlomParameters(
        uint64_t interval, bool isBuilding, uint16_t policy
    )  = 0;
    CGlomParameters* makeGlomParameters(const CRingItem& rhs) = 0;
    
    CPhysicsEventItem* makePhysicsEventItem(size_t maxBody) = 0;
    CPhysicsEventItem* makePhysicsEventItem(
        uint64_t timestamp, uint32_t source, uint32_t barrier, size_t maxBody
    ) = 0;
    CPhysicsEventItem* makePhysicsEventItem(const CRingItem& rhs) = 0;
    
    CRingFragmentItem* makeRingFragmentItem(
        uint64_t timestamp, uint32_t source, uint32_t payloadSize,
        const void* payload, uint32_t barrier=0
    ) = 0;
    CRingFragmentItem* makeRingFragmentItem(const CRingItem& rhs) = 0;
    
    CRingPhysicsEventCountItem* makePhysicsEventCountItem() = 0;
    CRingPhysicsEventCountItem* makePhysicsEventCountItem(
        uint64_t count, uint32_t timeOffset
    ) = 0;
    CRingPhysicsEventCountItem* makePhysicsEventCountItem(
        uint64_t count, uint32_t timeOffset
    ) = 0;
    CRingPhysicsEventCountItem* makePhysicsEventCountItem(
        uint64_t count, uint32_t timeoffset, time_t stamp,
    int divisor=1
    ) = 0;
    CRingPhysicsEventCountItem* makePhysicsEventCountItem(const CRingItem& rhs);
    
    CRingScalerItem* makeScalerItem(size_t numScalers) = 0;
    CRingScalerItem* makeScalerItem(
        uint32_t startTime,
        uint32_t stopTime,
        time_t   timestamp,
        std::vector<uint32_t> scalers,
        bool                  isIncremental = true,
        uint32_t              sid = 0,
        uint32_t              timeOffsetDivisor = 1
    ) = 0;
    CRingScalerItem* makeScalerItem(const CRingItem& rhs) = 0;
    
    CRingTextItem* makeTextItem(
        uint16_t type,
		std::vector<std::string> theStrings
    ) = 0;
    CRingTextItem* makeTextItem(
        uint16_t type,
		std::vector<std::string> theStrings,
		uint32_t                 offsetTime,
		time_t                   timestamp, uint32_t divisor=1
    ) = 0;
    CRingTextItem* makeTextItem(const CRingItem& rhs) = 0;
    
    CUnknownFragment* makeUnknownFragment(
        uint64_t timestamp, uint32_t sourceid, uint32_t barrier,
        uint32_t size, void* pPayload
    ) = 0;
    CUnknownFragment* makeUnknownFragent(const CRingItem& rhs) = 0;
    
    
    
};

#endif