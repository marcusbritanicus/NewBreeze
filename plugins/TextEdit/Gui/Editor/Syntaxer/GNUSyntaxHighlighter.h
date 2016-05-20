/*
 *  Copyright (C) 2008-2010  Lorenzo Bettini, http://www.lorenzobettini.it
 *  License: See COPYING file that comes with this distribution
 */

#pragma once

#include <srchilite/highlightstate.h>
#include <srchilite/formattermanager.h>
#include <srchilite/sourcehighlighter.h>
#include <srchilite/formatterparams.h>
#include <srchilite/langdefmanager.h>
#include <srchilite/langmap.h>

#include "TextFormatterFactory.h"
#include "HighlightStateData.h"

/**
 * An abstract class using GNU Source-highlight library to perform syntax highlighting.
 * You must use one of the concrete classes, depending on the version of Qt that you're
 * using, e.g., Qt4SyntaxHighlighter or Qt3SyntaxHighlighter.
 */
class GNUSyntaxHighlighter {
    /// the lang file used by this highlighter
    QString langFile;

    /// the GNU Source-highlighter used for the actual highlighting
    srchilite::SourceHighlighter *sourceHighlighter;

    /**
     * this is crucial to get the starting position of the string to highlight
     * within the whole line
     */
    srchilite::FormatterParams formatterParams;

    /// the color string for foreground
    QString foregroundColor;

    /// the color string for background
    QString backgroundColor;

    /// whether we highlight read only contents
    bool readOnly;

    /// whether to default font to monospace (default true)
    bool defaultToMonospace;

protected:
    /**
     * This will be used by subclasses to generate the right table of formatters
     */
    srchilite::FormatterManager *formatterManager;

public:
    GNUSyntaxHighlighter();
    virtual ~GNUSyntaxHighlighter();

    const QString &getLangFile() const { return langFile; }

    /**
     * Given a language definition file name, returns the corresponding
     * highlight state.  This method is mainly for testing purposes.
     * @param file
     * @return the corresponding highlight state
     * @throws srchilite::ParserException
     */
    srchilite::HighlightStatePtr getHighlightState(const QString &file);

    /**
     * Given a language definition file name, initializes the Source-highlight's
     * highlighter.
     * @param file the lang file of Source-highlight
     * @throws srchilite::ParserException
     */
    void initHighlighter(const QString &file);

    /**
     * Returns the the lang def file name by using the file name for detecting
     * the syntax of the file (e.g., <em>foo.cpp</em> brings to <em>cpp.lang</em>,
     * <em>ChangeLog</em> brings to <em>changelog.lang</em>).
     *
     * @param filename
     * @return the lang def file name or the empty string if no mapping exists
     */
    const QString getLangDefFileFromFileName(const QString &filename);

    /**
     * @return the Source-highlight SourceHighlighter (can be null)
     */
    srchilite::SourceHighlighter *getHighlighter() const {
        return sourceHighlighter;
    }

    /**
     * Given a TextFormatterFactory, it uses to build the TextFormatterMap, by reading
     * a specified source-highlight style file.
     *
     * If the TextFormatterFactory already contains some formatters, then
     * simpy returns its TextFormatterMap, without rebuilding it.
     *
     * @param formatterFactory the formatter factory to create formatters
     * @param styleFile the style file name (can be a .style file or a .css file -
     * in this case it must be a css file that source-highlight is able to handle)
     * @return the TextFormatterMap
     * @throws ParserException in case of parsing error of the style file
     */
    const TextFormatterMap &getTextFormatterMap(
            TextFormatterFactory &formatterFactory,
            const QString &styleFile = "default.style");

    /**
     * @return the Source-highlight FormatterManager
     */
    srchilite::FormatterManager *getFormatterManager() const {
        return formatterManager;
    }

    /**
     * Highlights the passed line.
     * This method assumes that all the fields are already initialized (e.g.,
     * the FormatterManager).
     *
     * The passed HighlightStateData is used to configure the SourceHighlighter
     * with info like the current highlighting state and the state stack.
     * If it is null, we simply ignore it.
     *
     * This method can modify the bassed pointer and even make it NULL
     * (after deleting it).
     *
     * @param line
     * @param stateData the highlight state data to use
     * @return in case after highlighting the stack changed we return either the original
     * stateData (after updating) or a new HighlightStateData (again with the updated
     * information)
     */
    void highlightLine(const QString &line, HighlightStateData *&stateData);

    /**
     * @return the LangDefManager object used by this highlighter
     */
    srchilite::LangDefManager *getLangDefManager() const;

    /**
     * @return the LangMap dealing with language definition files.
     */
    srchilite::LangMap *getLangMap() const;

    /**
     * @return the LangMap dealing with output language definition files.
     */
    srchilite::LangMap *getOutLangMap() const;

    /**
     * @return the foreground color string (can be empty)
     */
    const QString &getForegroundColor() const {
        return foregroundColor;
    }

    /**
     * @return the background color string (can be empty)
     */
    const QString &getBackgroundColor() const {
        return backgroundColor;
    }

    void setForegroundColor(const QString &f) {
        foregroundColor = f;
    }

    void setBackgroundColor(const QString &b) {
        backgroundColor = b;
    }

    /**
     * @return whether we must highlight readonly contents
     */
    bool isReadOnly() const {
        return readOnly;
    }

    /**
     * Specifies that we highlight readonly contents.  This optimizes
     * the highlighting: being readonly, we must not keep track of
     * the state of the previous line, and this saves time and memory.
     *
     * @param r
     */
    void setReadOnly(bool r) {
        readOnly = r;
    }

    bool isDefaultToMonospace() const {
        return defaultToMonospace;
    }

    /**
     * Whether the highlighter should use monospace font
     * by default if nothing is specified about the font family
     *
     * @param d
     */
    void setDefaultToMonospace(bool d) {
        defaultToMonospace = d;
    }
};
