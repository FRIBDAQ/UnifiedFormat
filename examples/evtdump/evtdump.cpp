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

#include "cmdline.h"
#include <string>
#include <NSCLDAQFormatFactorySelector.h>
#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <unistd.h>

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
        int         skipCount  = args.skip_arg;
        int         dumpCount  = args.count_arg;
        std::string sampledItems = args.sample_arg;
        std::string excludeItems = args.exclude_arg;
        int         scalerBits = args.scaler_width_arg;
        FormatSelector::SupportedVersions defaultVersion = mapVersion(args.format_arg);
        
    }
    catch (std::exception& e) {
        std::cerr << e.what();
        cmdline_parser_print_help();
        std::exit(EXIT_FAILURE);
    }
    
    std::exit(EXIT_SUCCESS);
}
