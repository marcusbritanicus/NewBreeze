/*
 *  Copyright (C) 2008-2010  Lorenzo Bettini, http://www.lorenzobettini.it
 *  License: See COPYING file that comes with this distribution
 */

#pragma once

#include <QComboBox>
#include "Qt4SyntaxHighlighter.h"

/**
  * A subclass of QComboBox which provides functionalities for showing
  * and selecting the language definition files of Source-Highlight.
  */
class LanguageComboBox : public QComboBox
{
          Q_OBJECT
public:
    /**
     * Initializes the combo box with the .lang files retrieved in the
     * specified path.
     * @param path where to look for .lang files (if empty, uses the data dir
     * path of source-highlight)
     */
    LanguageComboBox(const QString &path = "");

    /**
     * @param _highlighter the highlighter object; this will be used only for initialization
     * @deprecated use the other constructor, which permits not needing a Qt4SyntaxHighlighter
     * only for initialization
     */
    LanguageComboBox(Qt4SyntaxHighlighter *_highlighter);

public slots:
    /**
     * Sets the language in the combo
     * @param lang the language name; if the language is not part of the
     * combo list, nothing happens
     */
    void setCurrentLanguage(const QString &lang);

    /**
      * @return the language definition file name currently selected
      */
    const QString getCurrentLanguage() const;

    /**
     * Reloads the contents of the combo box by using the specified
     * path for searching for .lang files.
     * @param path
     */
    void reload(const QString &path);
};
