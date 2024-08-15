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

/** @file:  NSCLDAQFormatFactorySelector.cpp
 *  @brief: Provides methods to choose between format factories.
 */
#include "NSCLDAQFormatFactorySelector.h"
#include <abstract/CDataFormatItem.h>
#include <v10/RingItemFactory.h>
#include <v11/RingItemFactory.h>
#include <v12/RingItemFactory.h>
#include <map>
#include <stdexcept>
#include <stdint.h>
/**
 * Since ring item factories have no state but must be instantiable classes
 * in order to support the polymorphism we need, we're going to maintain a map
 * indexed by version of the factories we've created.  In that way, we
 * provide a reused factory if there's s duplicate request.
 * Note this means that factories we return references to are owned by us
 * not the caller who obtains those references -- specifically callers _must_
 * not delete factories they receive from us.
 */

namespace ufmt {
    namespace FormatSelector {
        /**
         * cache of previously created ring item factories.
         */
        static std::map<SupportedVersions, ::RingItemFactoryBase*> instantiatedFactories;
        /**
         * lookup table between version numbers and the version enumerator.
         */
        static std::map<uint16_t, SupportedVersions> versionLookup = {
            {10, v10}, {11, v11}, {12, v12}
        };
        
        /**
         * createFactory [private]
         *    This is a private 'method' to actually create a factory and enter it into
         *    the cache of factories, once we know we need one.
         *  @param v - version we need a new factory for.
         *  @throw std::invalid_argument if v is not a valid, supported version.
         *      this can happen because C++ can be fast and loose babou
         */
        static void createFactory(SupportedVersions v) {
            switch (v) {
                case v10:
                    instantiatedFactories[v10] = new v10::RingItemFactory;
                    break;
                case v11:
                    instantiatedFactories[v11] = new v11::RingItemFactory;
                    break;
                case v12:
                    instantiatedFactories[v12] = new v12::RingItemFactory;
                    break;
                default:
                    throw std::invalid_argument(
                        "Invalid NSCLDAQ version instantiating a format factory"
                    );
            }
        }
        /**
         * selectFactory
         *    Given that we know the NSCLDAQ format of data items, return the
         *    factory associated with that version.  Note that if necessary
         *    a new factory is created.
         * @param version - the version we need a factory for.
         * @return RingItemFactoryBase& reference to a concrete subclass of the
         *        ring item factory base.  Note that this module retains ownershp
         *        of that object.  As such it is a, potentially fatal, error for
         *        the caller to delete this object.
         * @throw std::invalid_argument - if the version is not a valid/supported version.
         */
        ::RingItemFactoryBase&
        selectFactory(SupportedVersions version)
        {
            if (instantiatedFactories.count(version) == 0) {
                createFactory(version);
            }
            return *instantiatedFactories[version];
        }
        /**
         * selectFactory
         *    Given that we have a data format item, extract the version,
         *    convert it to a supported version code and return a suitable factory.
         *    This is really a wrapper for the prior overload of selectFactory.
         * @param item - reference to a data format item
         * @return ::RingItemFactoryBase& - see prior selectFactory comments.
         * @throw std::invalid_argument if:
         *    - The version does not correspond to a SupportedVersionsValue
         *    - The version correspo;nsds to a SupportedVersion but we don't yet
         *      know how to create an appropriate factory.
         */
        ::RingItemFactoryBase&
        selectFactory(::CDataFormatItem& item)
        {
            // we only care about the major version.
            
            uint16_t major = item.getMajor();
            if (versionLookup.count(major)  == 0) {
                throw std::invalid_argument("Format item has unrecognized version");
            } else {
                return selectFactory(versionLookup[major]);
            }
        }
        /**
         * clearCache
         *   only should be used for testing...destroys the map of existing
         *   factory instances.  This invalidates existing references hence only
         *   use in testing.
         */
        void
        clearCache()
        {
            while (!instantiatedFactories.empty()) {
                delete instantiatedFactories.begin()->second;
                instantiatedFactories.erase(instantiatedFactories.begin());
            }
        }
    }   // FormatSelector
}       // ufmt
