#ifndef CDATAFORMATITEM_H
#define CDATAFORMATITEM_H
/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt
*/

/**
 * @file CDataFormatItem.h
 * @brief Define the CDataFormatItem a class for producing data format ring items.
 * @author Ron Fox<fox@nscl.msu.edu>
 */

#include "CRingItem.h"            // Base class.
#include <typeinfo>


/**
 * @class CDataFormatItem
 *
 * This class encapsulates ring items of the type DataFormat (RING_FORMAT).
 * It is used to provide an indicator to the user of the format of data that is
 * being inserted into a ring.  This can be used by data decoders to determine
 * how to handle the specific data types.  If a ring does not ever get a
 * RING_FORMAT item, you can assume the format to be for NSCLDAQ-10.* as this
 * ring type was added in NSCLDAQ 11.0.
 * The ring format will contain the major and minor versions of the
 * lowest NSCLDAQ version that can process this data format.
 */
class CDataFormatItem : public CRingItem
{
    // Canonical methods:
public:
    CDataFormatItem();
    virtual ~CDataFormatItem();
    
    CDataFormatItem(const CDataFormatItem& rhs);
    CDataFormatItem(const CRingItem& rhs) ;
    
private:
    CDataFormatItem& operator=(const CDataFormatItem& rhs);
    CDataFormatItem& operator=(const CRingItem& rhs) ;

    int operator==(const CDataFormatItem& rhs) const;
    int operator!=(const CDataFormatItem& rhs) const;
    
public:
    // Getters (these are useful when the item was created from a RingItem)
    
    uint16_t getMajor() const;
    uint16_t getMinor() const;

    
    // object methods:

    virtual std::string typeName() const;
    virtual std::string toString() const;
    
    // Required virtual methods: pure virtual in the base class.
    
    virtual void* getBodyHeader() const;
    virtual void setBodyHeader(uint64_t timestamp, uint32_t sourceId,
                         uint32_t barrierType = 0);
    
    // Private utilities
private:
    void init();
    
};

#endif
