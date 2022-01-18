/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  CRingItem.h
 *  @brief: Abstract base class of the CRingItem:
 */
#ifndef ABSTRACTRINGITEM_H
#define ASBTRACTRINGITEM_H
#include <unistd.h>
#include <stdint.h>
#include <string>


struct _RingItem;
typedef _RingItem RingItem, *pRingItem;
class CRingBuffer;

static const uint32_t CRingItemStaticBufferSize=8192;
static const uint32_t CRingItemFromRawSlop = 1024;

/**
 * @class CRingItem
 *    Abstract base class that provides a ring item.
 *    Construction of concrete items, for the most part
 *    is supposed to come through factories rather than direct user
 *    code construction... however that is also supported-- for the specific
 *    concrete classes.
 *  @note this base class will do storage management.
 */
class CRingItem {
protected:    
      pRingItem   m_pItem;
      uint32_t    m_storageSize;
      uint8_t     m_staticBuffer[CRingItemStaticBufferSize + 100];
      void*       m_pCursor;

public:
      CRingItem(uint16_t type, size_t maxBody = CRingItemStaticBufferSize - 10);
      
      CRingItem(const CRingItem& rhs);
      CRingItem(pRingItem pItem);
      
    
      virtual ~CRingItem();
private:                                   // Don't allow these vestigial
      CRingItem& operator=(const CRingItem& rhs);
      int operator==(const CRingItem& rhs) const;
      int operator!=(const CRingItem& rhs) const;
    
    
    
      // Selectors:
    
public:
      size_t getStorageSize() const;
      size_t getBodySize()    const;
      virtual const void*  getBodyPointer() const;
      virtual void* getBodyPointer();
      virtual void*  getBodyCursor();
      pRingItem  getItemPointer();
      const RingItem*  getItemPointer() const;
      uint32_t type() const;
      uint32_t size() const;
      virtual bool mustSwap() const;
      virtual bool hasBodyHeader() const;
      virtual void* getBodyHeader() const = 0;
      virtual uint64_t getEventTimestamp() const;
      virtual uint32_t getSourceId() const;
      virtual uint32_t getBarrierType() const;
      
// Mutators:

public:
      virtual void setBodyHeader(uint64_t timestamp, uint32_t sourceId,
                         uint32_t barrierType = 0) = 0;
      virtual void setBodyCursor(void* pNewCursor);
    
      // Object actions:
    
      virtual void commitToRing(CRingBuffer& ring);
      virtual void updateSize();            /* Set the header size given the cursor. */
      
    
      // Virtual methods that all ring items must provide:
    
      virtual std::string typeName() const; // Textual type of item.
      virtual std::string toString() const; // Provide string dump of the item.
      virtual void* appendBodyData(const void* pSrc, uint32_t nBytes);
          
protected:
      void newIfNecessary(uint32_t newSize);
      void deleteIfNecessary();
      void copyIn(const CRingItem& rhs);
      void throwIfNoBodyHeader(std::string msg) const;
      static void* fillRingHeader(pRingItem p, uint32_t size, uint32_t type);

};

#endif