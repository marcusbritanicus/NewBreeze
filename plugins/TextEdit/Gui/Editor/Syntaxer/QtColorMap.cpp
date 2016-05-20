/*
 *  Copyright (C) 2008-2010  Lorenzo Bettini, http://www.lorenzobettini.it
 *  License: See COPYING file that comes with this distribution
 */

#include "QtColorMap.h"

#include <srchilite/utils.h>

QtColorMap::QtColorMap() {
    (*this)["white"] = "#FFFFFF";
    (*this)["black"] = "#000000";
    (*this)["red"] = "#FF0000";
    (*this)["darkred"] = "#990000";
    (*this)["brown"] = "#660000";
    (*this)["yellow"] = "#FFCC00";
    (*this)["cyan"] = "#66FFFF";
    (*this)["blue"] = "#3333FF";
    (*this)["pink"] = "#CC33CC";
    (*this)["purple"] = "#993399";
    (*this)["orange"] = "#FF6600";
    (*this)["brightorange"] = "#FF9900";
    (*this)["green"] = "#33CC00";
    (*this)["brightgreen"] = "#33FF33";
    (*this)["darkgreen"] = "#009900";
    (*this)["teal"] = "#008080";
    (*this)["gray"] = "#808080";
    (*this)["darkblue"] = "#000080";
}

QtColorMap::~QtColorMap() {
}

std::string QtColorMap::getColor(const std::string &color) {
    if (color[0] == '"' && color[color.size()-1] == '"') {
        return color.substr(1, color.size()-2);
    } else {
        return (*this)[color];
    }
}

QtColorMapRGB::QtColorMapRGB() {
    (*this)["#FFFFFF"] = "white";
    (*this)["#000000"] = "black";
    (*this)["#FF0000"] = "red";
    (*this)["#990000"] = "darkred";
    (*this)["#660000"] = "brown";
    (*this)["#FFCC00"] = "yellow";
    (*this)["#66FFFF"] = "cyan";
    (*this)["#3333FF"] = "blue";
    (*this)["#CC33CC"] = "pink";
    (*this)["#993399"] = "purple";
    (*this)["#FF6600"] = "orange";
    (*this)["#FF9900"] = "brightorange";
    (*this)["#33CC00"] = "green";
    (*this)["#33FF33"] = "brightgreen";
    (*this)["#009900"] = "darkgreen";
    (*this)["#008080"] = "teal";
    (*this)["#808080"] = "gray";
    (*this)["#000080"] = "darkblue";
}

QtColorMapRGB::~QtColorMapRGB() {
}

std::string QtColorMapRGB::getColor(const std::string &color) {
    const_iterator mapped = find(srchilite::Utils::toupper(color));
    if (mapped != end())
        return mapped->second;
    else
        return color;
}
