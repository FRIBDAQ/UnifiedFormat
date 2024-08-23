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

#include "CAbnormalEndItem.h"
#include "DataFormat.h"
#include <stdexcept>


namespace ufmt {
    namespace v12 {

    /**
     * default constructor
     *    Create the ring item.
     */
    CAbnormalEndItem::CAbnormalEndItem() : ::ufmt::CAbnormalEndItem()
    {
        v12::pAbnormalEndItem p =
            reinterpret_cast<v12::pAbnormalEndItem>(getItemPointer());
        p->s_header.s_type = v12:: ABNORMAL_ENDRUN;
        p->s_empty         = sizeof(uint32_t);
        setBodyCursor(p+1);
        updateSize();                  // Sets p->s_header.s_size correctly for us.
        
    }
    /**
     * destructor is null.
     */
    CAbnormalEndItem::~CAbnormalEndItem() {}

    // Body header stuff:


    /**
     * getBodyHeader
     *    @return void* - nullptr - there's never a body header.
     */
    void*
    CAbnormalEndItem::getBodyHeader() const
    {
        return nullptr;
    }
    /**
     * setBodyHeader
     *   @throw std::logic_error - v12::AbnormalEndItem's never have a body header.
     */
    void
    CAbnormalEndItem::setBodyHeader(uint64_t timestamp, uint32_t sid, uint32_t bid)
    {
        throw std::logic_error("V12 Abnormal end items never have body headers");
    }
    /* Formatting interface */

    /**
     * typeName
     *    Returns a textual name of the item typ
     * @return ::std::string
     */
    ::std::string
    CAbnormalEndItem::typeName() const
    {
        return "Abnormal End";
    }
    

    }                    // v12 namespace
}