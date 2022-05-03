/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt
*/
/**
 * @file CDataFormatItem.h
 * @brief Implement the CDataFormatItem a class for producing data format
 *        ring items.
 * @author Ron Fox<fox@nscl.msu.edu>
 */

#include "CDataFormatItem.h"
#include "DataFormat.h"

#include <sstream>
#include <stdexcept>

namespace v12 {
/*-----------------------------------------------------------------------------
 * Canonicals
 *---------------------------------------------------------------------------*/

/**
 * constructor
 *
 *   Constructs a data format ring item whose payload contains the current
 *   data format major and minor versions encoded in DataFormat.h
 */
CDataFormatItem::CDataFormatItem() :
    ::CDataFormatItem()
{
    v12::pDataFormat pItem = reinterpret_cast<v12::pDataFormat>(getItemPointer());
    pItem->s_header.s_type = v12::RING_FORMAT;
    pItem->s_majorVersion  = v12::FORMAT_MAJOR;
    pItem->s_minorVersion  = v12::FORMAT_MINOR;
    pItem->s_empty          = sizeof(uint32_t);
    
    // Set the cursor and get the header size field properly set:
    
    setBodyCursor(pItem+1);
    updateSize();
}

CDataFormatItem::~CDataFormatItem() {}
/*----------------------------------------------------------------------------
 * Getters.
 */

/**
 * major
 *
 * Returns the format major revision level.
 *
 * @return uint16_t
 */
uint16_t
CDataFormatItem::getMajor() const
{
    
    const v12::DataFormat* pItem =
        reinterpret_cast<const v12::DataFormat*>(getItemPointer());
    return pItem->s_majorVersion;
}
/**
 * minor
 *
 * Returns the format minor version.
 *
 * @return uint16_t
 */
uint16_t
CDataFormatItem::getMinor() const
{
   
    const v12::DataFormat* pItem =
        reinterpret_cast<const v12::DataFormat*>(getItemPointer());
    return pItem->s_minorVersion;    
}
/*----------------------------------------------------------------------------
 * object methods:
 *--------------------------------------------------------------------------*/
/**
 * typeName
 *
 * @return std::string the data type  as a string.
 */
std::string
CDataFormatItem::typeName() const
{
    return std::string("Ring Item format version");
}
/**
 * toString
 *
 * @return std::string - text version of the body of the event.
 */
std::string
CDataFormatItem::toString() const
{
    CDataFormatItem* This = const_cast<CDataFormatItem*>(this);
    pDataFormat pItem = reinterpret_cast<pDataFormat>(This->getItemPointer());
    std::ostringstream out;
    
    out << "Ring items formatted for: " << pItem->s_majorVersion << '.'
        << pItem->s_minorVersion << std::endl;
        
    return out.str();
}

// Body header methods:  Note there is never a body header for this item type:

/**
 * getBodyHeader
 *   @return void* nullptr
 */
void*
CDataFormatItem::getBodyHeader() const
{
    return nullptr;
}


/**
 * setBodyHeader
 *
 *   throw std::logic_error - data format items never have body headers.
 */
void
CDataFormatItem::setBodyHeader(uint64_t ts, uint32_t sid, uint32_t bar)
{
    throw std::logic_error("v12 format items never have a body header");
}


}                                   // v12 namespace.