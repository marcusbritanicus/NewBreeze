/*
 *  Copyright (C) 2008-2010  Lorenzo Bettini, http://www.lorenzobettini.it
 *  License: See COPYING file that comes with this distribution
 */

#include "Qt4TextFormatterFactory.h"
#include "Qt4TextFormatter.h"

#include <QColor>
#include <QBrush>

using namespace srchilite;

Qt4TextFormatterFactory::Qt4TextFormatterFactory() {
}

Qt4TextFormatterFactory::~Qt4TextFormatterFactory() {
}

bool Qt4TextFormatterFactory::createFormatter(const string &key,
        const string &color, const string &bgcolor,
        srchilite::StyleConstantsPtr styleconstants) {

    if (hasFormatter(key))
        return false;

    Qt4TextFormatter *formatter = new Qt4TextFormatter(key);
    addFormatter(key, TextFormatterPtr(formatter));

    // check whether we should default to monospace
    if (isDefaultToMonospace())
        formatter->setMonospace(true);

    if (styleconstants.get()) {
        for (StyleConstantsIterator it = styleconstants->begin(); it
                != styleconstants->end(); ++it) {
            switch (*it) {
            case ISBOLD:
                formatter->setBold(true);
                break;
            case ISITALIC:
                formatter->setItalic(true);
                break;
            case ISUNDERLINE:
                formatter->setUnderline(true);
                break;
            case ISFIXED:
                formatter->setMonospace(true);
                //formatter->getQTextCharFormat().setFontFixedPitch(true);
                break;
            case ISNOTFIXED:
                formatter->setMonospace(false);
                break;
            case ISNOREF:
                break;
            }
        }
    }

    if (color.size()) {
        formatter->setForegroundColor(colorMap.getColor(color).c_str());
    }

    if (bgcolor.size()) {
        formatter->setBackgroundColor(colorMap.getColor(bgcolor).c_str());
    }

    return true;
}
