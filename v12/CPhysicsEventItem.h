#ifndef V12_CPHYSICSEVENTITEM_H
#define V12_CPHYSICSEVENTITEM_H
/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#include <CPhysicsEventItem.h>		/* Base class */
#include <unistd.h>
#include <stdint.h>
#include <string>
#include <typeinfo>
namespace ufmt {
    namespace v12 {

    /**
     *  This class is a wrapper for physics events.
     *  It's mainly provided so that textual dumps
     *  can be performed as typeName and toString
     *  are the only substantive methods...everything
     *  else just delegates to the base class.
     */

    class CPhysicsEventItem : public ::ufmt::CPhysicsEventItem
    {
    public:
        CPhysicsEventItem(size_t maxBody=8192);
        CPhysicsEventItem(uint64_t ts, uint32_t sid, uint32_t barrierType, size_t maxBody=8192);
        virtual ~CPhysicsEventItem();
        
    private:
        CPhysicsEventItem(const CRingItem& rhs) ;
        CPhysicsEventItem(const CPhysicsEventItem& rhs);
        
    
        CPhysicsEventItem& operator=(const CPhysicsEventItem& rhs);
        int operator==(const CPhysicsEventItem& rhs) const;
        int operator!=(const CPhysicsEventItem& rhs) const;
    public:
        // Virtual methods that all ring items must provide:
    
        virtual std::string typeName() const;	// Textual type of item.
        virtual std::string headerToString() const; // Provide string dump of the item.
        
        size_t        getBodySize() const;
        virtual void* getBodyPointer();
        virtual const void* getBodyPointer() const;
        virtual bool  hasBodyHeader() const;
        virtual void* getBodyHeader() const;
        virtual void setBodyHeader(
            uint64_t tstamp, uint32_t sourceId, uint32_t barrierType
        );
        virtual uint64_t getEventTimestamp() const;
        virtual uint32_t getSourceId() const;
        virtual uint32_t getBarrierType() const;
    
    
    };

    }                           // v12 namespace.
}
#endif
