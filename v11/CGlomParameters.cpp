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
#include "CRingItem.h"
#include <sstream>
#include <stdexcept>


namespace ufmt {

    

    namespace v11 {
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
        uint64_t interval, bool isBuilding, ::ufmt::CGlomParameters::TimestampPolicy policy
    ) :
        ::ufmt::CGlomParameters(interval, isBuilding, policy)
    {
        // Fill in the body of the item:
        
        v11::pGlomParameters pItem = reinterpret_cast<v11::pGlomParameters>(getItemPointer());
        pItem->s_header.s_type    = v11::EVB_GLOM_INFO;
        pItem->s_mbz              = 0;
        pItem->s_coincidenceTicks = interval;
        pItem->s_isBuilding       = (isBuilding ? 0xffff : 0);
        pItem->s_timestampPolicy  = policy;
        
        // Set the insertion cursor...and compute the final item size.
        
        setBodyCursor(&(pItem[1]));
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
        
        const v11::GlomParameters* pItem =
            reinterpret_cast<const v11::GlomParameters*>(getItemPointer());
        
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
        
        const v11::GlomParameters* pItem =
            reinterpret_cast<const v11::GlomParameters*>(getItemPointer());
        
        return pItem->s_isBuilding != 0;                   // True bool.
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
        
        const v11::GlomParameters* pItem =
            reinterpret_cast<const v11::GlomParameters*>(getItemPointer());
            
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
     * toSTring
     *
     * @return std::string - a textual dump of the ring item contents.
     */
    
    //  Delegate these to v11::CRingItem...
    bool
    CGlomParameters::hasBodyHeader() const
    {
        return getBodyHeader() != nullptr;
    }
    void*
    CGlomParameters::getBodyHeader() const
    {
        // We know it's only an act of needing inheritance from the abstracts
        // that makes us not a v11::CRingItem, but in fact our data structure is identical.
        
        const v11::CRingItem* pV11base = reinterpret_cast<const v11::CRingItem*>(this);
        return pV11base->CRingItem::getBodyHeader(); 
    }
    void
    CGlomParameters::setBodyHeader(uint64_t timestamp, uint32_t sourceId,
                            uint32_t barrierType)
    {
        
    }

    }
}