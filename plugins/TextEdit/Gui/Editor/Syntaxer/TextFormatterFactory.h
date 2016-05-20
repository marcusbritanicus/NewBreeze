/*
 *  Copyright (C) 2008-2010  Lorenzo Bettini, http://www.lorenzobettini.it
 *  License: See COPYING file that comes with this distribution
 */

#pragma once

#include <map>

#include "srchilite/formatterfactory.h"

#include "TextFormatter.h"
#include "QtColorMap.h"

using std::string;

typedef std::map<string, TextFormatterPtr> TextFormatterMap;

/**
 * Specialization of FormatterFactory to create TextFormatter objects
 * to format text in a TextEdit.
 */
class TextFormatterFactory: public srchilite::FormatterFactory {
protected:
    TextFormatterMap textFormatterMap;

    /// whether to default font to monospace (default true)
    bool defaultToMonospace;

public:
    /// the color map for source-highlight colors into RGB "#RRGGBB" values
    static QtColorMap colorMap;

    TextFormatterFactory();
    virtual ~TextFormatterFactory();

    /**
     * Checks whether a formatter for the given key is already present.  If not found,
     * then it returns an empty TextFormatterPtr
     * @param key
     * @return whether a formatter for the given key is already present
     */
    bool hasFormatter(const string &key) const;

    /**
     * Returns the formatter for the given key.
     * @param key
     * @return the formatter for the given key is already present
     */
    TextFormatterPtr getFormatter(const string &key) const;

    /**
     * Adds the formatter for the given key.
     * @param key
     * @param formatter
     */
    void addFormatter(const string &key, TextFormatterPtr formatter);

    /**
     * @return the TextFormatterMap
     */
    const TextFormatterMap &getTextFormatterMap() const {
        return textFormatterMap;
    }

    bool isDefaultToMonospace() const {
        return defaultToMonospace;
    }

    /**
     * Whether the created formatters should use monospace font
     * by default if nothing is specified about the font family
     *
     * @param d
     */
    void setDefaultToMonospace(bool d) {
        defaultToMonospace = d;
    }
};
