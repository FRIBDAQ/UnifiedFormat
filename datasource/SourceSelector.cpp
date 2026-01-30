#include <fmtconfig.h>
#include "DataSource.h"
#include "StreamDataSource.h"
#include "FdDataSource.h"
#ifdef HAVE_NSCLDAQ
#include "RingDataSource.h"
#include <CRemoteAccess.h>
#include <Exception.h>
#endif
#include <RingItemFactoryBase.h>
#include "URL.h"

#include <unistd.h>
#include <fstream>

namespace ufmt {
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
            catch (CException& e) {
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
}