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

int main(int argc, char** argv)
{
    gengetopt_args_info args;
    cmdline_parser(argc , argv, &args);
    
}
