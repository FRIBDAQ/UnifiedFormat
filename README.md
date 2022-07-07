# Unified Format


NSCLDAQ currently has three format versions: 10, 11, and 12.
Each format version  has its own set of class libraries for producing
and, more importantly to our users, handling data from it.

This project aims to provide a unified data format library that,
with minimal changes can be used in programs that have been written
to handle data from a specific data format.

The key elements of the class library are:

*   Item factories that can produce items from specific versions of
    NSCLDAQ.
*   Pure virtual item base classes which are specialized by each version.
*   Namespacing the specific format item types but not the base types.
    Since the user interacts with the base type interfaces, for the most part,
    they will not see the namespaces.

THey key point is that the base classes, rather than being a least common
denominator interface, provide a common multiple.  It's up to the
implementation of concrete classes to figure out what to do with interface
elements they cannot implement.

Key to the directories:

*  abstract - contains the various base classes. While functional for some
version of NSCDAQ they are not meant to actually be used.
*  v10, v11, 12 contain the support for those versions of NSCLDAQ format.
*  examples contain example programs, currently only evtdump
*  the top level contains the factory selector code.
*  docs contains the documentation (docbook) source code.

Installation instructions:
The build system is cmake which likes to do out of tree builds.  There are
currently two cmake variables:

CMAKE_INSTALL_PREFIX - governs where the package is installed.
NSCLDAQ_ROOT         - if building with NSCLDAQ (to support online use), this
                       points to an NSCLDAQ installation tree.
Here's a sample build with cd set to the cloned repository:

```
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/opt/daqformat/1.1 \
  -DNSCLDAQ_ROOT=/usr/opt/daq/11.3-027
make -j26 all && make install && make test

```