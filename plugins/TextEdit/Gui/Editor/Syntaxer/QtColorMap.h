/*
 *  Copyright (C) 2008-2010  Lorenzo Bettini, http://www.lorenzobettini.it
 *  License: See COPYING file that comes with this distribution
 */

#pragma once

#include <map>
#include <string>

/**
 * Map for source-highlight colors into qt colors (actually, into the
 * format "#RRGGBB")
 */
class QtColorMap : public std::map<std::string, std::string> {
public:
    QtColorMap();
    ~QtColorMap();

    /**
     * Returns the color associated to the passed string (if the
     * string is already of the shape "#RRGGBB", then it returns the
     * string "#RRGGBB" itself, without the quotes)
     * @param color
     * @return the color associated to the passed string
     */
    std::string getColor(const std::string &color);
};

/**
 * Map for RGB colors (actually, into the
 * format "#RRGGBB") into source-highlight colors.  If a map does
 * not exist it simply returns the rgb color.
 *
 * @since 0.2
 */
class QtColorMapRGB : public std::map<std::string, std::string> {
public:
    QtColorMapRGB();
    ~QtColorMapRGB();

    /**
     * Returns the source-highlight color associated to the passed string
     * of the shape "#RRGGBB"; if a mapping does not exist, then it returns the
     * passed string itself). The search is NOT case sensitive.
     * @param color the color in rgb form "#RRGGBB"
     * @return the source-highlight color associated to the passed string, or
     * the passed string if there is no association.
     */
    std::string getColor(const std::string &color);
};
