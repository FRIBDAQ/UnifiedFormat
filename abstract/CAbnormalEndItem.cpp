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
 * toString
 *
 *   Return a nicely formatted rendition of the ring item.
 * @return std::string
 */
std::string
CAbnormalEndItem::toString() const
{
    std::string result = typeName();
    result += "\n";
    return result;
}
// Implement pure virtuals from base class:

/**
 * getBodyHeader()
 *  throws a logic error because we don't have a body header/
 */
void*
CAbnormalEndItem::getBodyHeader() const
{
    throw std::logic_error("Abnormal end items don't have body headers");
}
/**
 * setBodyHeader
 *    throws a logic error.
 */
void
CAbnormalEndItem::setBodyHeader(
    uint64_t timestamp, uint32_t sourceId,
    uint32_t barrierType
)
{
    throw std::logic_error("Abnormal end items don't have body headers");
}