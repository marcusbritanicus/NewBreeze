/*
 *  Copyright (C) 2008-2010  Lorenzo Bettini, http://www.lorenzobettini.it
 *  License: See COPYING file that comes with this distribution
 */

#include "Qt4SourceHighlightStyleGenerator.h"
#include "QtColorMap.h"

static QtColorMapRGB colorMapRGB;

QStringList Qt4SourceHighlightStyleGenerator::createStyleContents(QList<
        Qt4TextFormatter *> &formatters, const QString &bgColor) {
    QStringList l;

    if (bgColor != "") {
        std::string color = colorMapRGB.getColor(bgColor.toStdString());
        if (color[0] == '#')
            l << "bgcolor \"" + QString(color.c_str()) + "\";";
        else
            l << "bgcolor " + QString(color.c_str()) + ";";
    }

    for (QList<Qt4TextFormatter *>::const_iterator it = formatters.begin(); it
            != formatters.end(); ++it) {
        l << (*it)->toSourceHighlightStyleString();
    }

    return l;
}
