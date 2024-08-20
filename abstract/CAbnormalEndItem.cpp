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
#include <typeinfo>
#include <stdexcept>

namespace ufmt {

    /**
     * default constructor
     *    Create the ring item.
     */
    CAbnormalEndItem::CAbnormalEndItem() :
        CRingItem(ABNORMAL_ENDRUN) {}
        
    /**
     * destructor is null.
     */
    CAbnormalEndItem::~CAbnormalEndItem() {}


    /*
    * copy costructors --sort of.
    */
    CAbnormalEndItem::CAbnormalEndItem(const CAbnormalEndItem& rhs) :
        CRingItem(rhs)
    {}
    CAbnormalEndItem::CAbnormalEndItem(const CRingItem& rhs) :
        CRingItem(rhs)
    {
        if(type() != ABNORMAL_ENDRUN) {
            throw std::bad_cast();
        }
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
    /**
     * headerToString
     *    Format the header as a string - in this chase just indicate this is an abormal
     * end item.
     * 
     * @return std::string
     */
    std::string
    CAbnormalEndItem::headerToString() const {
        std::string result = typeName();
        result += "\n";
        return result;
    }
    /**
     *  bodyToString
     *    Format the body as a string -
     * 
     * @return std::string - an empty string as there is no body.
     */
    std::string
    CAbnormalEndItem::bodyToString() const {
        std::string result;
        return result;
    }
    //Base class toString is fine now.
    // Implement pure virtuals from base class:

    /**
     * getBodyHeader()
     *   @return nullptr
     */
    void*
    CAbnormalEndItem::getBodyHeader() const
    {
    return nullptr;
    }
    /**
     * setBodyHeader
     *    no-op
     */
    void
    CAbnormalEndItem::setBodyHeader(
        uint64_t timestamp, uint32_t sourceId,
        uint32_t barrierType
    )
    {

    }
}