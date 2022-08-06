/**
 * NBFunction.hpp - NBFunction.cpp header
 **/

#pragma once

#include "Global.hpp"

/** Terminal */
QStringList getTerminal();

/** If this is an appiamge, get its mount path */
QString getAppImagePath();

/** Search if a string is a part of any of the strings in a list */
bool stringInStringList( QStringList, QString );
