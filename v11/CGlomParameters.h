#ifndef V11_CGLOMPARAMETERS_H
#define V11_CGLOMPARAMETERS_H
/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt
*/

/**
 * @file CGlomParameters.h (v11)
 * @brief  Encapsulates a ring item that contains glom parametrs.
 * @author  Ron Fox <fox@nscl.msu.edu>
 */

#include <CGlomParameters.h>
namespace ufmt {
    namespace v11 {
    /**
     * @class CGlomParameters
     *
     * Encapsulates a ring item of type EVB_GLOM_INFO.  The structure of this ring
     * item is given by the GlomParameters struct.  No body header is required for
     * this type..though the mbz field is present in case later we decide that
     * was a mistake (e.g. cascaded event building we may have gloms with different
     * parameters at different levels and knowing that by assigning each glom an
     * event source id may be needed).
     */
    class CGlomParameters : public ::ufmt::CGlomParameters
    {

        
    public:
        CGlomParameters(uint64_t interval, bool isBuilding, TimestampPolicy policy);
        virtual ~CGlomParameters();
    private:
        CGlomParameters(const CGlomParameters& rhs);
        CGlomParameters(const CRingItem& rhs) ;
        
        CGlomParameters& operator=(const CGlomParameters& rhs);
        int operator==(const CGlomParameters& rhs) const;
        int operator!=(const CGlomParameters& rhs) const;
        
        // Selectors:
    public:
    uint64_t coincidenceTicks() const;
    bool     isBuilding() const;
    TimestampPolicy timestampPolicy() const;
    
    // Object methods:
    public:
        virtual std::string typeName() const;
        virtual std::string toString() const;
        
    // Implementation of pure virtual methods from the base class:
    
    virtual bool  hasBodyHeader() const;
    virtual void* getBodyHeader() const;
    virtual void setBodyHeader(uint64_t timestamp, uint32_t sourceId,
                            uint32_t barrierType = 0);
    

    };

    }                                // v11 namespace
}
#endif
