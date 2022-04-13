#ifndef V11_CUNKNOWNFRAGMENT_H
#define V11_CUNKNOWNFRAGMENT_H

/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt
*/

/**
 * @file CUnknownFragment.h
 * @brief Defines the CUnknownFragment calss for EVB_UNKNOWN_PAYLOAD ring items.
 * @author Ron Fox <fox@nscl.msu.edu>
 */

#include <CUnknownFragment.h>

namespace v11 {
/**
 * @class CUnknownFragment
 *
 * This class encapsulates ring items of type EVB_UNKNOWN_PAYLOAD. These are
 * event builder fragments where the payloads are pretty clearly not ring items.
 */
class CUnknownFragment : public ::CUnknownFragment
{
    // Canonical methods:
    
public:
    CUnknownFragment(uint64_t timestamp, uint32_t sourceid, uint32_t barrier,
                     uint32_t size, void* pPayload);
    virtual ~CUnknownFragment();
private:
    CUnknownFragment(const CUnknownFragment& rhs);
    CUnknownFragment(const CRingItem& rhs) ;
    
    CUnknownFragment& operator=(const CUnknownFragment& rhs);
    int operator==(const CUnknownFragment& rhs) const;
    int operator!=(const CUnknownFragment& rhs) const;
    
    
public:
    virtual const void*  getBodyPointer() const;
    virtual void* getBodyPointer();
    virtual bool hasBodyHeader() const;
    virtual void* getBodyHeader() const;
    virtual void setBodyHeader(uint64_t timestamp, uint32_t sourceId,
                         uint32_t barrierType = 0) ;
    

    std::string typeName() const;
    virtual std::string toString() const;   // Because we want the v11 one.
    
};

}
#endif
