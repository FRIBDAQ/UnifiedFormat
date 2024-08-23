/**

#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2013.
#
#    You may use this software under the terms of the GNU public license
#    (GPL).  The terms of this license are described at:
#
#     http://www.gnu.org/licenses/gpl.txt
#
#    Author:
#            Ron Fox
#            NSCL
#            Michigan State University
#            East Lansing, MI 48824-1321

##
# @file   CAbnormalEndItem.cpp
# @brief  Implements the abnormal end run ring item class.
# @author <fox@nscl.msu.edu>
*/

#include  "CAbnormalEndItem.h"
#include "DataFormat.h"
#include <stdexcept>
namespace ufmt {
    namespace v11 {
    /**
     * default constructor
     *    Create the ring item.
     */
    CAbnormalEndItem::CAbnormalEndItem() :
        ::ufmt::CAbnormalEndItem()
    {
        v11::pRingItem pItem = reinterpret_cast<v11::pRingItem>(getItemPointer());
        pItem->s_header.s_type = v11::ABNORMAL_ENDRUN;
        uint32_t* p = reinterpret_cast<uint32_t*>(getBodyCursor());
        *p++ = 0;                         // V11 style.
        setBodyCursor(p);
        updateSize();
    }
        
    /**
     * destructor is null.
     */
    CAbnormalEndItem::~CAbnormalEndItem() {}


    /**
     * There is no body header in a v11 Abnormal End item
    */
    bool
    CAbnormalEndItem::hasBodyHeader() const
    {
        return false;
    }

    void*
    CAbnormalEndItem::getBodyHeader() const
    {
        return nullptr;
    }

    void
    CAbnormalEndItem::setBodyHeader(
        uint64_t timestamp, uint32_t sourceId,
        uint32_t barrierType
    )
    {
        throw std::logic_error("Version 11 abnormal end ring items don't have a body header");
    }

    /* Formatting interface */

    /**
     * typeName
     *    Returns a textual name of the item typ
     * @return std::string
     */
    std::string
    CAbnormalEndItem::typeName() const
    {
        return "Abnormal End";
    }
    
    }
}