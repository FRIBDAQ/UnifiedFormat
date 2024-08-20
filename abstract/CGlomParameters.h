#ifndef CGLOMPARAMETERS_H
#define CGLOMPARAMETERS_H
/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt
*/

/**
 * @file CGlomParameters.h
 * @brief  Encapsulates a ring item that contains glom parametrs.
 * @author  Ron Fox <fox@nscl.msu.edu>
 */


#include "CRingItem.h"
#include <typeinfo>
namespace ufmt {
    /**
     * @class CGlomParameters (abstract)
     *
     * Encapsulates a ring item of type EVB_GLOM_INFO.  The structure of this ring
     * item is given by the GlomParameters struct.  No body header is required for
     * this type..though the mbz field is present in case later we decide that
     * was a mistake (e.g. cascaded event building we may have gloms with different
     * parameters at different levels and knowing that by assigning each glom an
     * event source id may be needed).
     */
    class CGlomParameters : public CRingItem
    {
    public:
        // Note the enum values below _must_ match those in DataFormat.h:
        
        typedef enum _TimestampPolicy {
            first = 0, last = 1, average = 2
        } TimestampPolicy;
        
        // Canonicals:
        
    public:
        CGlomParameters(uint64_t interval, bool isBuilding, TimestampPolicy policy);
        virtual ~CGlomParameters();
        CGlomParameters(const CGlomParameters& rhs);
        CGlomParameters(const CRingItem& rhs) ;
    private:
        CGlomParameters& operator=(const CGlomParameters& rhs);
        int operator==(const CGlomParameters& rhs) const;
        int operator!=(const CGlomParameters& rhs) const;
        
        // Selectors:
    public:
    virtual uint64_t coincidenceTicks() const;
    virtual bool     isBuilding() const;
    virtual TimestampPolicy timestampPolicy() const;
    
    // Object methods:
    public:
        virtual std::string typeName() const;  // "Glom Parameters"
        virtual std::string headerToString() const;
        virtual std::string bodyToString() const;
        
        // Implementation of pure virtual methods from the base class:
        
    virtual void* getBodyHeader() const;
    virtual void setBodyHeader(uint64_t timestamp, uint32_t sourceId,
                            uint32_t barrierType = 0);
        

    };
}

#endif
