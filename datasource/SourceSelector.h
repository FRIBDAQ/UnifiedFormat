#ifndef SOURCESELECTOR_H
#define SOURCESELECTOR_H
#include <string>
class DataSource;
class RingItemFactoryBase;

namespace ufmt {
    DataSource*
    makeDataSource(RingItemFactoryBase* pFactory, const std::string& strUrl);
}

#endif