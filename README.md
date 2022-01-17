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


