/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     FRIB
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  NSCLDAQFormatFactorySelector.h
 *  @brief: Provides methods for selecting a specific formatter factory.
 */

#ifndef NSCLDAQFORMATFACTORYSELECTOR_H
#define NSCLDAQFORMATFACTORYSELECTOR_h

/**
 * Rather than a class, we can just use a namespace to provide these
 * essentially unbound functions.
 */

class RingItemFactoryBase;
class CDataFormatItem;
class CRingItem;
namespace FormatSelector {
    enum SupportedVersions {v10, v11, v12};
    
    RingItemFactoryBase& selectFactory(SupportedVersions version);
    RingItemFactoryBase& selectFactory(CDataFormatItem& item);
    
    void clearCache();                // For testing...users must not call this.
}                          // End namespace.


#endif