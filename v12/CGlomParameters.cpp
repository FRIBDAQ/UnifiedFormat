/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt
*/

/**
 * @file CGlomParameters.cpp
 * @brief  Encapsulates a ring item that contains glom parameters.
 * @author  Ron Fox <fox@nscl.msu.edu>
 */
#include "CGlomParameters.h"
#include "DataFormat.h"
#include <sstream>
#include <stdexcept>

namespace ufmt {
    namespace v12 {


   

    /*-------------------------------------------------------------------------
    * Canonical methods
    *-----------------------------------------------------------------------*/

    /**
     * constructor
     *
     * This is the 'normal' constructor.  It builds a fully fledged glom parameters
     * data item.
     *
     * @param interval   - Number of ticks in the event building coincidence window.
     * @param isBuilding - If true the output of glom is built rather than just
     *                     ordered/formatted events.
     * @param policy     - The timestamp policy used by glom.
     */
    CGlomParameters::CGlomParameters(
        uint64_t interval, bool isBuilding, TimestampPolicy policy
    ) :
        ::ufmt::CGlomParameters(interval, isBuilding, policy)
    {
        // Fill in the body of the item:
        
        pGlomParameters pItem = reinterpret_cast<pGlomParameters>(getItemPointer());
        pItem->s_header.s_type    = v12::EVB_GLOM_INFO;
        pItem->s_coincidenceTicks = interval;
        pItem->s_isBuilding       = (isBuilding ? 0xffff : 0);
        pItem->s_timestampPolicy  = policy;
        
        // Set the insertion cursor...and compute the final item size.
        
        setBodyCursor(pItem+1);
        updateSize();
    }
    /**
     * destructor
     */
    CGlomParameters::~CGlomParameters()
    {
        
    }

    /*----------------------------------------------------------------------------
    * Selectors
    *--------------------------------------------------------------------------*/

    /**
     * coincidenceTicks
     *
     * @return uint64_t - the number of ticks glom used in its coincidence window
     *                    this is meaningful, however only if isBuilding() returns
     *                    true.
     */
    uint64_t
    CGlomParameters::coincidenceTicks() const
    {
        
        const GlomParameters* pItem =
            reinterpret_cast<const GlomParameters*>(getItemPointer());
        
        return pItem->s_coincidenceTicks;

        
    }
    /**
     * isBuilding
     *
     * @return bool - true if glom is glueing event fragments together.
     */
    bool
    CGlomParameters::isBuilding() const
    {
        
        const GlomParameters* pItem =
            reinterpret_cast<const GlomParameters*>(getItemPointer());
        
        return pItem->s_isBuilding;
    }
    /**
     * timestampPolicy
     *
     * @return CGlomParameters::TimestampPolicy - the timestamp policy from
     *         the ring item.
     */
    CGlomParameters::TimestampPolicy
    CGlomParameters::timestampPolicy() const
    {
        
        const GlomParameters* pItem =
            reinterpret_cast<const GlomParameters*>(getItemPointer());
            
        return static_cast<TimestampPolicy>(pItem->s_timestampPolicy);
    }
    /*---------------------------------------------------------------------------
    * Object methods
    *-------------------------------------------------------------------------*/

    /**
     * typeName
     *
     * @return std::string - textual version of the ring type.
     */
    std::string
    CGlomParameters::typeName() const
    {
        return std::string("Glom Parameters");
    }
    

    /**
     * getBodyHeader
     *    @return void* nullptr - there's never a body header for v12.
     */
    void*
    CGlomParameters::getBodyHeader() const
    {
        return nullptr;
    }
    /**
     * setBodyHeader
     *    @throw std::logic_error.
     */
    void
    CGlomParameters::setBodyHeader(uint64_t ts, uint32_t sid, uint32_t bar)
    {
        throw std::logic_error("V12 Glom parameters never have body headers");
    }

    }      // v12 namespace.
}