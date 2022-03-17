/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2009.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#ifndef __CRINGFRAGMENTITEM_H
#define __CRINGFRAGMENTITEM_H

#ifndef __CRINGITEM_H
#include "CRingItem.h"
#endif

#ifndef __CPPRTL_TYPEINFO
#include <typeinfo>
#ifndef __CPPRTL_TYPEINFO
#define __CPPRTL_TYPEINFO
#endif
#endif



// Forward definitions.


typedef struct _EventBuilderFragment *pEventBuilderFragment;


/**
 * class to encapsulate ring items that are actually event builder output fragments.
 */
class CRingFragmentItem : public CRingItem
{
  // Implemented canonical items:

public:
  CRingFragmentItem(uint64_t timestamp, 
		    uint32_t source, 
		    uint32_t payloadSize, 
		    const void* pBody,
		    uint32_t barrier=0);
  CRingFragmentItem(const CRingItem& rhs) ;
  CRingFragmentItem(const CRingFragmentItem& rhs);

  virtual ~CRingFragmentItem();

  CRingFragmentItem& operator=(const CRingFragmentItem& rhs);
  int operator==(const CRingFragmentItem& rhs) const;
  int operator!=(const CRingFragmentItem& rhs) const;

  // Accessor member functions:

public:
  uint64_t     timestamp() const;
  uint32_t     source() const;
  size_t       payloadSize();
  void*        payloadPointer();
  uint32_t     barrierType() const;

  // Virtual method overrides:

  virtual std::string typeName() const;
  virtual std::string toString() const;

  // private utilities:

private:
  size_t bodySize(size_t payloadSize) const;
  void   copyPayload(const void* pPayloadSource, size_t payloadSize);
  void   init(size_t size);
};

#endif
