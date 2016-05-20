/*
 *  Copyright (C) 2008-2010  Lorenzo Bettini, http://www.lorenzobettini.it
 *  License: See COPYING file that comes with this distribution
 */

#pragma once

#include "TextFormatterFactory.h"

/**
 * Implementation of TextFormatterFactory to create Qt4 TextFormatter objects
 * to format text in a TextEdit.
 */
class Qt4TextFormatterFactory: public TextFormatterFactory {
public:
    Qt4TextFormatterFactory();
    virtual ~Qt4TextFormatterFactory();

    /**
     * Creates a formatter for the specific language element (identified by
     * key) with the passed style parameters
     *
     * @param key
     * @param color
     * @param bgcolor
     * @param styleconstants
     * @return false if a formatter for the specific key is already present
     */
    virtual bool createFormatter(const string &key, const string &color,
            const string &bgcolor, srchilite::StyleConstantsPtr styleconstants);;
};
