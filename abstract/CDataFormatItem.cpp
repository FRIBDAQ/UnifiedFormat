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

namespace ufmt {
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
        CRingItem(RING_FORMAT, sizeof(DataFormat))
    {
        init();        
    }

    CDataFormatItem::~CDataFormatItem() {}
    /**
     * copy constructor
     *
     * @param rhs - The item that will serve as a template for us.
     */
    CDataFormatItem::CDataFormatItem(const CDataFormatItem& rhs) :
        CRingItem(rhs)
    {}
    /**
     * construct from generic
     * 
     *
     * Constructs from a CRingItem. If the CRingItem type is not RING_FORMAT,
     * a bad_cast is thrown.
     *
     * @param rhs Reference to the ring item that will be used to construct us.
     */
    CDataFormatItem::CDataFormatItem(const CRingItem& rhs) :
        CRingItem(rhs)
    {
        if (type() != RING_FORMAT) {
            throw std::bad_cast();
        }
    }


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
        
        const DataFormat* pItem =
            reinterpret_cast<const DataFormat*>(getItemPointer());
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
    
        const DataFormat* pItem =
            reinterpret_cast<const DataFormat*>(getItemPointer());
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

    // The data format items don't have body headers so:

    void*
    CDataFormatItem:: getBodyHeader() const
    {
    return nullptr;
    }
    void
    CDataFormatItem::setBodyHeader(uint64_t timestamp, uint32_t sourceId,
                            uint32_t barrierType)
    {

    }
    /*----------------------------------------------------------------------------
    * Private utilities.
    *--------------------------------------------------------------------------*/

    /**
     * init
     *   Fill in the ring item once the base class has been constructed.
     */
    void
    CDataFormatItem::init()
    {
        pDataFormat pBody = reinterpret_cast<pDataFormat>(getItemPointer());
        pBody->s_majorVersion = FORMAT_MAJOR;
        pBody->s_minorVersion = FORMAT_MINOR;
        
        setBodyCursor(&(pBody[1]));
        updateSize();
        
    }
}