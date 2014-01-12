/*
    *
    * NBCLParser.hpp - NBArgParser.cpp header
    *
*/

#ifndef NBARGPARSER
#define NBARGPARSER

#include <sys/utsname.h>

#include <Global.hpp>
#include <optionparser.hpp>

enum  optionIndex {
	UNKNOWN,
	HELP,
	USAGE,
	VERSION,
	SETTINGS,
	FORCENEW,
	NORMAL
};

int NBArgParser( int argc, char** argv );

#endif
