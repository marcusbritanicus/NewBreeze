/*
 *  Copyright (C) 2008-2010  Lorenzo Bettini, http://www.lorenzobettini.it
 *  License: See COPYING file that comes with this distribution
 */

#include "TextFormatterFactory.h"

QtColorMap TextFormatterFactory::colorMap;

TextFormatterFactory::TextFormatterFactory() :
    defaultToMonospace(true) {
}

TextFormatterFactory::~TextFormatterFactory() {
}

bool TextFormatterFactory::hasFormatter(const string &key) const {
    return textFormatterMap.find(key) != textFormatterMap.end();
}

TextFormatterPtr TextFormatterFactory::getFormatter(const string &key) const {
    TextFormatterMap::const_iterator it = textFormatterMap.find(key);
    if (it != textFormatterMap.end()) {
        return it->second;
    } else {
        return TextFormatterPtr();
    }
}

void TextFormatterFactory::addFormatter(const string &key,
        TextFormatterPtr formatter) {
    textFormatterMap[key] = formatter;
}
