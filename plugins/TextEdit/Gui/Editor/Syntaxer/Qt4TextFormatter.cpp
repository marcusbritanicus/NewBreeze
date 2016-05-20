/*
 *  Copyright (C) 2008-2010  Lorenzo Bettini, http://www.lorenzobettini.it
 *  License: See COPYING file that comes with this distribution
 */

#include "Qt4TextFormatter.h"
#include "Qt4SyntaxHighlighter.h"
#include "QtColorMap.h"

#include <srchilite/formatterparams.h>

//#define SHOW_DEBUG_INFO 1

#ifdef SHOW_DEBUG_INFO
#include <iostream>
#define DEBUG_INFO(x,y) std::cout << x << y << std::endl;
#else
#define DEBUG_INFO(x,y)
#endif

/// used for translation into source-highlight style format
static QtColorMapRGB colorMapRGB;

Qt4TextFormatter::Qt4TextFormatter(const std::string &elem_) :
    TextFormatter(elem_), qSyntaxHighlighter(0) {
}

Qt4TextFormatter::~Qt4TextFormatter() {
}

void Qt4TextFormatter::setQSyntaxHighlighter(
        QSyntaxHighlighter *qSyntaxHighlighter_) {
    qSyntaxHighlighter
            = dynamic_cast<Qt4SyntaxHighlighter *> (qSyntaxHighlighter_);
}

void Qt4TextFormatter::format(const std::string &s,
        const srchilite::FormatterParams *params) {

    qSyntaxHighlighter->formatString(params->start, s.size(), textFormat);

    DEBUG_INFO (s + "\n", toString().toStdString());
}

void Qt4TextFormatter::setForegroundColor(const QColor &color) {
    textFormat.setForeground(QBrush(color));
    foregroundColor = color;
}

void Qt4TextFormatter::setBackgroundColor(const QColor &color) {
    textFormat.setBackground(QBrush(color));
    backgroundColor = color;
}

const QString Qt4TextFormatter::toString() const {
    QString s = QString(getElem().c_str()) + ": ";
    const QTextCharFormat & format = getQTextCharFormat();
    if (format.fontWeight() == QFont::Bold)
        s += "BOLD ";
    if (format.fontItalic())
        s += "ITALIC ";
    if (format.fontUnderline())
        s += "UNDERLINE ";
    if (getForegroundColor().isValid())
        s += "foreground: " + format.foreground().color().name();
    else
        s += "no foreground";
    if (getBackgroundColor().isValid())
        s += " background: " + format.background().color().name();
    else
        s += " no background";
    s += "\n";

    return s;
}

const QString Qt4TextFormatter::toSourceHighlightStyleString() const {
    QString s = QString(getElem().c_str()) + " ";
    const QTextCharFormat & format = getQTextCharFormat();
    std::string color;

    if (getForegroundColor().isValid()) {
        color = colorMapRGB.getColor(
                format.foreground().color().name().toStdString());
        if (color[0] == '#')
            s += "\"" + QString(color.c_str()) + "\" ";
        else
            s += QString(color.c_str()) + " ";
    }

    if (getBackgroundColor().isValid()) {
        s += "bg:";
        color = colorMapRGB.getColor(
                format.background().color().name().toStdString());
        if (color[0] == '#')
            s += "\"" + QString(color.c_str()) + "\" ";
        else
            s += QString(color.c_str()) + " ";
    }

    QString styleConstants;
    if (format.fontWeight() == QFont::Bold)
        styleConstants += "b";
    if (format.fontItalic())
        styleConstants += (styleConstants.size() ? ", " : "") + QString("i");
    if (format.fontUnderline())
        styleConstants += (styleConstants.size() ? ", " : "") + QString("u");
    if (isMonospace())
        styleConstants += (styleConstants.size() ? ", " : "") + QString("f");

    return s + styleConstants + ";";
}
