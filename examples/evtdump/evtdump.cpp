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

/** @file:  evtdump.cpp
 *  @brief: Main program to use the format library to do event file dumping
 */
#include <fmtconfig.h>
#ifdef HAVE_NSCLDAQ
#error "Have nscldaq"
#endif
#include "cmdline.h"
#include <string>
#include <NSCLDAQFormatFactorySelector.h>
#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <unistd.h>

#include "DataSource.h"
#include "FdDataSource.h"
#include "StreamDataSource.h"
#ifdef HAVE_NSCLDAQ
#include "RingDataSource.h"
#include <CRemoteAccess.h>
#include <CRingBuffer.h>
#include <Exception.h>
#endif
#include <memory>

#include <URL.h>

#include <fstream>
#include <vector>
#include <map>
#include <DataFormat.h>
#include <algorithm>
#include <RingItemFactoryBase.h>

// These are headers for the abstrct ring items we can get back from the factory.
// As new ring items are added this set of #include's must be updated as well
// as the switch statement in the dumpItem  method.
//

#include <CRingItem.h>
#include <CAbnormalEndItem.h>
#include <CDataFormatItem.h>
#include <CGlomParameters.h>
#include <CPhysicsEventItem.h>
#include <CRingFragmentItem.h>
#include <CRingPhysicsEventCountItem.h>
#include <CRingScalerItem.h>
#include <CRingTextItem.h>
#include <CRingStateChangeItem.h>
#include <CUnknownFragment.h>


// End of ring item type headers.

// Map of exclusion type strings to type integers:

static std::map<std::string, uint32_t> TypeMap = {
    {"BEGIN_RUN", BEGIN_RUN}, {"END_RUN", END_RUN}, {"PAUSE_RUN", PAUSE_RUN},
    {"RESUME_RUN", RESUME_RUN},
    {"ABNORMAL_ENDRUN", ABNORMAL_ENDRUN},
    {"PACKET_TYPES", PACKET_TYPES}, {"MONITORED_VARIABLES", MONITORED_VARIABLES},
    {"RING_FORMAT", RING_FORMAT},
    {"PERIODIC_SCALERS", PERIODIC_SCALERS}, {"INCREMENTAL_SCALERS", INCREMENTAL_SCALERS},
    {"TIMESTAMPED_NONINCR_SCALERS", TIMESTAMPED_NONINCR_SCALERS},
    {"PHYSICS_EVENT", PHYSICS_EVENT}, {"PHYSICS_EVENT_COUNT", PHYSICS_EVENT_COUNT},
    {"EVB_FRAGMENT", EVB_FRAGMENT}, {"EVB_UNKNOWN_PAYLOAD", EVB_UNKNOWN_PAYLOAD},
    {"EVB_GLOM_INFO", EVB_GLOM_INFO}
};
/**
 * tokenize
 *   Shamelessly stolen from https://www.techiedelight.com/split-string-cpp-using-delimiter/
 * @param str - string to split up.
 * @param delim - delimimeter on which to split the string.
 * @return std::vector<string> - not that in the original, this is a parameter
 */
static std::vector<std::string>
tokenize(std::string const &str, const char delim)
{
    std::vector<std::string> out;
    size_t start;
    size_t end = 0;
 
    while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
    {
        end = str.find(delim, start);
        out.push_back(str.substr(start, end - start));
    }
    return out;
}
/**
 * dumpItem
 *    Dump an item to stdout.
 *    - Based on the item type, use the factory to get a new item using the
 *      same data for the appropriate type.
 *    - Use the toString method of that item to get the string to dump.
 *    - Dump the item with a "--------" trailer.
 *  @param pItem - pointer to the item.
 *  @param factory - reference to the factory appropriate to the format.
 *  @note this method is rather long but this is only due to the switch statement
 *        that must handle every possible ring item type in DataFormat.h
 *        The actual code is really quite simple (I think).
 *  @note the use of std::unique_Ptr to ensure that temporary specific ring item
 *        objects are automatically deleted.
 */
static void
dumpItem(CRingItem* pItem, RingItemFactoryBase& factory) {
    std::string dumpText;
 
    // Note that the switch statement here assumes that if you have a ring item type
    // the factory can generate it... this fails if the wrong version of the factory
    // is used for the event file.
    
    switch(pItem->type()) {
        case BEGIN_RUN:
        case END_RUN:
        case PAUSE_RUN:
        case RESUME_RUN:
            {
                std::unique_ptr<CRingStateChangeItem> p(factory.makeStateChangeItem(*pItem));
                dumpText = p->toString();
            }
            break;
        case ABNORMAL_ENDRUN:
            {
                std::unique_ptr<CAbnormalEndItem> p(factory.makeAbnormalEndItem(*pItem));
                dumpText = p->toString();
            }
            break;
        case PACKET_TYPES:
        case MONITORED_VARIABLES:
            {
                std::unique_ptr<CRingTextItem> p(factory.makeTextItem(*pItem));
                dumpText = p->toString();
            }
            break;
        case RING_FORMAT:
            {
                try {
                    std::unique_ptr<CDataFormatItem> p(factory.makeDataFormatItem(*pItem));
                    dumpText = p->toString();
                }
                catch (std::bad_cast e) {
                    throw std::logic_error(
                        "Unable to dump a data format item.. likely you've specified the wrong --format"
                    );
                }
            }
            break;
        case PERIODIC_SCALERS:
        // case INCREMENTAL_SCALERS:        // Same value as PERIODIC_SCALERS.
        case TIMESTAMPED_NONINCR_SCALERS:
            {
                std::unique_ptr<CRingScalerItem> p(factory.makeScalerItem(*pItem));
                dumpText = p->toString();
            }
            break;
        case PHYSICS_EVENT:
            {
                std::unique_ptr<CPhysicsEventItem> p(factory.makePhysicsEventItem(*pItem));
                dumpText = p->toString();
            }
            break;
        case PHYSICS_EVENT_COUNT:
            {
                std::unique_ptr<CRingPhysicsEventCountItem> p(factory.makePhysicsEventCountItem(*pItem));
                dumpText = p->toString();
            }
            break;
        case EVB_FRAGMENT:
            {
                std::unique_ptr<CRingFragmentItem> p(factory.makeRingFragmentItem(*pItem));
                dumpText = p->toString();
            }
            break;
        case EVB_UNKNOWN_PAYLOAD:
            {
                std::unique_ptr<CUnknownFragment> p(factory.makeUnknownFragment(*pItem));
                dumpText = p->toString();
            }
            break;
        case EVB_GLOM_INFO:
            {
                std::unique_ptr<CGlomParameters> p(factory.makeGlomParameters(*pItem));
                dumpText = p->toString();
            }
            break;
        default:
            dumpText = pItem->toString();                 // Unknown item type.
            break;
    }
 
    std::cout << "------------------------------------------\n";
    std::cout << dumpText << std::endl;
    
}
/**
 * makeExclusionList
 *    Creates a vector of the ring item types to be excluded from the dump
 *    given a comma separated list of types. A type can be a string or a positive number.
 *    If it is a string, it is translated to the type id using TypeMap.  If
 *    it is a number, it is used as is.
 *
 *    @param exclusions - string containing the exclusion list.
 *    @return std::vector<uint32_t> - items to exclude.
 *    @throw std::invalid_argument an exclusion item is not a string and is not in the
 *                  map of recognized item types.
 *                 
 */
std::vector<uint32_t>
makeExclusionList(const std::string& exclusions)
{
    std::vector<uint32_t> result;
    std::vector<std::string> words = tokenize(exclusions, ',');
    
    // Process the words into an exclusion list:
    for (auto s : words) {
        bool isInt(true);
        int intValue;
        try {
            intValue = std::stoi(s);
        }
        catch (...) {
            // failed as int
            
            isInt = false;
        }
        if (isInt) {
            result.push_back(intValue);
        } else {
            auto p = TypeMap.find(s);
            if (p != TypeMap.end()) {
                result.push_back(p->second);
            } else {
                std::string msg("Invalid item type in exclusion list: ");
                msg += s;
                throw std::invalid_argument(msg);
            }
        }
    }
    
    return result;
}

/**
 * makeDataSource
 *    - parse the URI of the source
 *    - Based on the parse create the underlying connection, stream, fd, ringbuffer
 *    - Create the correcte concrete instance of DataSource given all that.
 * @param pFact - pointer to the ring item factory to use.
 * @param strUrl   - String URI of the connection.
 * @return DataSource* - dynamically allocated data source.
 * @throw std::exception derived exception on failure -- which can come from
 *          not being able to form the underlying connection
 */
DataSource*
makeDataSource(RingItemFactoryBase* pFactory, const std::string& strUrl)
{
    // Special case the url is just "-"  then it's stdin, a file descriptor
    // data source:
    
    if (strUrl == "-") {
        return new FdDataSource(pFactory, STDIN_FILENO);   
    }
    // Parse the URI:
    
    
    URL uri(strUrl);
    std::string protocol = uri.getProto();
    
    if ((protocol == "tcp") || (protocol == "ring")) {
        // Ring buffer so:
#ifdef HAVE_NSCLDAQ
        try {
            CRingBuffer* pRing = CRingAccess::daqConsumeFrom(strUrl);
            return new RingDataSource(pFactory, *pRing);
        }
        catch (CExceptino& e) {
            throw std::invalid_argument(e.ReasonText());
        }
#else
        std::cerr <<  "This version of evtdump has not been built with ringbuffer data source support\n";
        exit(EXIT_FAILURE);
#endif
    } else {
        std::string path = uri.getPath();
        std::ifstream& in(*(new std::ifstream(path.c_str())));  // Need it to last past block.
        return new StreamDataSource(pFactory, in);
    }

}

/**
 * mapVersion
 *    Map the version we get from the command line to a factory version:
 * @param fmtIn[in] - Format the user requested.
 * @return FormatSelector::SupportedVersions - Factory version id.
 * @throw std::invalid_argument - bad format version
 * @note we should never throw because gengetopt will enforce the enum.
 */
static FormatSelector::SupportedVersions
mapVersion(enum_format fmtIn)
{
    switch (fmtIn) {
        case format_arg_v12:
            return FormatSelector::v12;
        case format_arg_v11:
            return FormatSelector::v11;
        case format_arg_v10:
            return FormatSelector::v10;
        default:
            throw std::invalid_argument("Invalid DAQ format version specifier");
    }
}
/**
 * makeSourceString
 *   Given a source string URI creates the actual one.  In this case it's a
 *   matter of mapping "" into tcp://localhost/username.
 * @param[in] srcIn - source provided by user (or not).
 * @return std::string - actual source string.
 * @note - is also used as a data source it means stdin.
 */
static std::string
makeSourceString(const char* srcIn)
{
    std::string result(srcIn);
    if (srcIn == "") {
        std::string user(getlogin());
        result = "tcp://localhost/";
        result += user;
    }
    
    return result;
}

int main(int argc, char** argv)
{
    try {
        gengetopt_args_info args;
        cmdline_parser(argc , argv, &args);
        
        // Figure out the parameters:
        
        std::string dataSource = makeSourceString(args.source_arg);
        int         skipCount  = args.skip_given ? args.skip_arg : 0;
        int         dumpCount  = args.count_arg;
        std::string excludeItems = args.exclude_arg;
        std::vector<uint32_t> exclusionList = makeExclusionList(excludeItems);
        int         scalerBits = args.scaler_width_arg;
        FormatSelector::SupportedVersions defaultVersion = mapVersion(args.format_arg);
        auto& fact = FormatSelector::selectFactory(defaultVersion);
        
        // Now we need to take the URI and the factory and create a data source:
        
        
        std::unique_ptr<DataSource> pSource(makeDataSource(&fact, dataSource));
        
        // Proces the scalerBits value into a ::CRingScalerItem::m_ScalerFormatMask
        
        uint64_t sbits = 1;
        sbits = sbits << scalerBits;
        sbits--;
        ::CRingScalerItem::m_ScalerFormatMask = sbits;
        
        // If there's a skip count skip exactly that many items:
        
        if (skipCount > 0) {
            for (int i =0; i < skipCount; i++) {
                std::unique_ptr<CRingItem> p(pSource->getItem());
                if (!p.get()) {
                    // end of data source
                    exit(EXIT_SUCCESS);
                }
            }
        }
        // Now dump the items that are not excluded and if there's a dumpCount
        // only dump that many items -- or until the end of the data source:
        
        int remaining = dumpCount;
        while(1) {
            std::unique_ptr<CRingItem> pItem(pSource->getItem());
            if (!pItem.get()) {
                exit(EXIT_SUCCESS);                          // End of source.
            }
            
            if (std::find(
                    exclusionList.begin(), exclusionList.end(), pItem->type()
                ) == exclusionList.end()) {
                // Dumpable:
                    
                dumpItem(pItem.get(), fact);
                
                // Apply any limit to the count:
                
                if (args.count_given) {
                    remaining--;
                    if(remaining <= 0) {
                        exit(EXIT_SUCCESS);
                    }
                }
            }
            
        }
        
        
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        cmdline_parser_print_help();
        std::exit(EXIT_FAILURE);
    }
    
    std::exit(EXIT_SUCCESS);
}
