/*
 *  Copyright (C) 2008-2010  Lorenzo Bettini, http://www.lorenzobettini.it
 *  License: See COPYING file that comes with this distribution
 */

#pragma once

#include <QSyntaxHighlighter>
#include <QTextDocument>
#include <QTextCharFormat>
#include <QMap>

#include "GNUSyntaxHighlighter.h"
#include "Qt4TextFormatter.h"

/// the map of Qt4TextFormatters
typedef QMap<QString, Qt4TextFormatter *> Qt4TextFormatterMap;

/// the iterator for Qt4TextFormatterMap
typedef QMapIterator<QString, Qt4TextFormatter *> Qt4TextFormatterMapIterator;

/**
 * An implementation of QSyntaxHighlighter using GNU Source-highlight library
 * (by relying on GNUSyntaxHighlighter provided by the common part of this library).
 *
 * You can use such highlighter with a QTextEdit, and initialize the highlighter
 * with the language definition file, e.g.,
 * @code
    QTextEdit *editor = new QTextEdit;
    Qt4SyntaxHighlighter *highlighter =
            new Qt4SyntaxHighlighter(editor->document());
    highlighter->init("java.lang");
 * @endcode
 */
class Qt4SyntaxHighlighter: public QSyntaxHighlighter,
        public GNUSyntaxHighlighter {

    /// the current style file for highlighting
    QString currentStyleFile;

protected:
    /**
     * The implementation of the virtual pure method of QSyntaxHighlighter
     */
    void highlightBlock(const QString &text);

    /**
     * Creates formatters (and use them to initialize the formatter manager),
     * by using the passed TextFormatterMap.  This can be called only after
     * the highlighter was initialized through init().
     * @param formatterMap
     */
    void setFormatters(const TextFormatterMap &formatterMap);

public:
    Qt4SyntaxHighlighter(QTextDocument *doc = 0);
    virtual ~Qt4SyntaxHighlighter();

    /**
     * Initializes this highlighter with the specified language definition file
     * @param langFile
     * @param styleFile the style file name (can be a .style file or a .css file -
     * in this case it must be a css file that source-highlight is able to handle)
     * @throws ParserException in case of parsing error of the lang file or of the style file
     */
    void init(const QString &langFile, const QString &styleFile =
            "default.style");

    /**
     * Initializes this highlighter by using the file name for detecting
     * the syntax of the file (e.g., <em>foo.cpp</em> brings to <em>cpp.lang</em>,
     * <em>ChangeLog</em> brings to <em>changelog.lang</em>).
     * @param fileName
     * @return true if we could find a lang def file for the specified filename
     */
    bool initFromFileName(const QString &fileName);

    /**
     * This function is applied to the syntax highlighter's current text block
     * (i.e. the text that is passed to the highlightBlock() method).
     *
     * The specified format is applied to the text from the start position
     * for a length of count characters
     * (if count is 0, nothing is done).
     * The formatting properties set in format are merged at display
     * time with the formatting information stored directly in the document,
     * for example as previously set with QTextCursor's functions.
     *
     * Note that this helper function will be called by the corresponding
     * TextFormatter, from Source-highglight library code, and relies on
     * the corresponding protected method of QSyntaxHighlighter: setFormat).
     */
    void formatString(int start, int count, const QTextCharFormat & format) {
        setFormat(start, count, format);
    }

    /**
     * Sets the styleFile for highlighting.  It also rehighlights the
     * contents of the document.  This must be called after the highlighter has already been
     * initialized, otherwise it does nothing.
     *
     * @param styleFile the style file name (can be a .style file or a .css file -
     * in this case it must be a css file that source-highlight is able to handle)
     * @throws ParserException in case of parsing error of the lang file or of the style file
     */
    void setFormattingStyle(const QString &styleFile);

    /// @return the current style file used for highlighting
    const QString getFormattingStyle() const {
        return currentStyleFile;
    }

    /**
     * Returns the formatter map, i.e., for each language element we have a formatter.
     * This should be called after the highlighter has already been initialized.
     * Note that the key of the map is a std::string representing the language element
     * and the associated value is a base class FormatterPtr (a shared pointer), so you need
     * to retrieve the associated actual pointer with .get() and to it cast it
     * to a Qt4TextFormatter, e.g.,:
     * @code
     * Qt4TextFormatter *formatter =
     dynamic_cast<Qt4TextFormatter *>(map["normal"].get());
     * @endcode
     * Note also that in case no formatter is associated to a specific string, then
     * .get() will return 0, so you should always check for the resulting pointer.
     * You might want to use, instead getQt4TextFormatterMap() where the values
     * are already of Qt4TextFormatter type (and the formatters are exactly the same).
     * @return the
     */
    const srchilite::FormatterMap &getFormatterMap() const {
        return formatterManager->getFormatterMap();
    }

    /**
     * Returns the formatter map of this highlighter.  It can be used
     * to change formatters; in this case, you should call rehighlight() after
     * changing formatters.
     * @return the map of Qt4TextFormatters used by this highlighter
     */
    Qt4TextFormatterMap getQt4TextFormatterMap();
};
