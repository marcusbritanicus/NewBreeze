/*
 *  Copyright (C) 2008-2010  Lorenzo Bettini, http://www.lorenzobettini.it
 *  License: See COPYING file that comes with this distribution
 */

#pragma once

#include <QTextCharFormat>
#include <QSyntaxHighlighter>

#include "TextFormatter.h"

class Qt4SyntaxHighlighter;

/**
 * The implementation of TextFormatter for Qt4, relying on QTextCharFormat
 */
class Qt4TextFormatter: public TextFormatter {
protected:
    /**
     * The format represented by this formatter.
     */
    QTextCharFormat textFormat;

    /** the (possibly invalid) foreground color.
     * We need to keep a copy to be sure whether the color is valid or not,
     * since QTextCharFormat has always a default (valid) color */
    QColor foregroundColor;

    /// the (possibly invalid) background color
    QColor backgroundColor;

    /**
     * The reference to QSyntaxHighlighter object.
     *
     * Since this header file does not include the header file for QSyntaxHighlighter,
     * then this same class can be re-used both for Qt3 and for Qt4 (the subclasses
     * of this class will include the correct header file for QSyntaxHighlighter).
     */
    Qt4SyntaxHighlighter *qSyntaxHighlighter;

public:
    Qt4TextFormatter(const std::string &elem_ = "normal");
    virtual ~Qt4TextFormatter();

    const QTextCharFormat &getQTextCharFormat() const {
        return textFormat;
    }

    virtual void setQSyntaxHighlighter(QSyntaxHighlighter *qSyntaxHighlighter_);

    /**
     * Formats the passed string.
     *
     * @param the string to format
     * @param params possible additional parameters for the formatter (NOT USED)
     */
    void format(const std::string &s, const srchilite::FormatterParams *params);

    /**
     * @return a string representation of this formatter
     */
    const QString toString() const;

    /**
     * Starting from this formatter, it builds a string representation using
     * the syntax of source-highlight style file
     *
     * @return a string representation using the syntax of source-highlight
     * style file
     */
    const QString toSourceHighlightStyleString() const;

    bool isBold() const {
        return textFormat.fontWeight() == QFont::Bold;
    }

    void setBold(bool b) {
        textFormat.setFontWeight(b ? QFont::Bold : QFont::Normal);
    }

    bool isItalic() const {
        return textFormat.fontItalic();
    }

    void setItalic(bool i) {
        textFormat.setFontItalic(i);
    }

    bool isUnderline() const {
        return textFormat.fontUnderline();
    }

    void setUnderline(bool u) {
        textFormat.setFontUnderline(u);
    }

    bool isMonospace() const {
        return textFormat.fontFamily() == "Monospace";
    }

    void setMonospace(bool m) {
        if (m)
            textFormat.setFontFamily("Monospace");
    }

    /// @return the (possibly invalid, if not set) foreground color
    const QColor &getForegroundColor() const {
        return foregroundColor;
    }

    void setForegroundColor(const QColor &color);

    /// @return the (possibly invalid, if not set) background color
    const QColor &getBackgroundColor() const {
        return backgroundColor;
    }

    void setBackgroundColor(const QColor &color);
};
