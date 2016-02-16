/*
    *
    * NBCLParser.hpp - NBArgParser.cpp header
    *
*/

#pragma once
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
	SYSTRAY,
	SETTINGS,
	ABOUTNB,
	LICENSE,
	FORCENEW,
	NORMAL
};

int NBArgParser( int argc, char** argv );

#endif
