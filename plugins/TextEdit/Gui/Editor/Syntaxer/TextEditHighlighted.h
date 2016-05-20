/*
 *  Copyright (C) 2008-2010  Lorenzo Bettini, http://www.lorenzobettini.it
 *  License: See COPYING file that comes with this distribution
 */

#pragma once

#include <QTextEdit>

#include "Qt4SyntaxHighlighter.h"

class LanguageComboBox;
class StyleComboBox;

/**
  * A specialized QTextEdit that uses a Qt4SyntaxHighlighter for
  * highlighting its contents.
  *
  * If connected to a LanguageComboBox, it automatically refreshes
  * its highlighted contents when the language definition file changes.
  * If connected to a StyleComboBox, it automatically refreshes
  * its highlighted contents when the style changes.
  */
class TextEditHighlighted : public QTextEdit
{
    Q_OBJECT

private:
    /// the highlighter object
    Qt4SyntaxHighlighter *highlighter;

    /// the (possible) LanguageComboBox for selecting languages
    LanguageComboBox *languageComboBox;

    /// the (possible) StyleComboBox for selecting languages
    StyleComboBox *styleComboBox;

    /// the style file of source-highlight for highlighting (default: "default.style")
    QString styleFile;

public:
    TextEditHighlighted();
    TextEditHighlighted(QWidget * parent);

    /**
      * Connects a LanguageComboBox so that, if the language is changed
      * it automatically rehighlights the contents of the textedit.
      * @param lcb the LanguageComboBox
      */
    void connectLanguageComboBox(LanguageComboBox *lcb);

    /**
      * Connects a StyleComboBox so that, if the style is changed
      * it automatically rehighlights the contents of the textedit.
      * @param lcb the LanguageComboBox
      */
    void connectStyleComboBox(StyleComboBox *lcb);

    Qt4SyntaxHighlighter *getHighlighter() const {
        return highlighter;
    }

    /**
      * Sets an highlighter for the specified language definition file
      * @param langFile
      */
    void setHighlighter(const QString &langFile);

    /**
      * Opens the file specified by fileName.
      * It uses the fileName to detect the source language and automatically
      * selects the corresponding syntax highlighting.
      *
      * @param fileName the name of the file to open (complete path)
      * @return a string representing the error. If successful returns the empty string
      */
    const QString loadFile(const QString &fileName);

    /**
      * Changes the foreground and background color.
      * @param fgColor the foreground (if empty, the color is not changed)
      * @param bgColor the background (if empty, the color is not changed)
      */
    void changeColors(const QString &fgColor, const QString &bgColor);

public slots:
      /**
        * Changes the highlighting according to the new specified language
        * @param newLang the new language definition
        */
     void changeHighlightingLanguage(const QString &newLang);

      /**
        * Changes the highlighting style according to the new specified style file
        * @param newStyle the new style
        */
     void changeHighlightingStyle(const QString &newStyle);

     /**
       * If the file name changes, then we check whether we need to change
       * the language definition for highlighting
       * @param fileName the new file name
       */
     void changeFileName(const QString &fileName);

     /**
      * Returns the the lang def file name by using the file name for detecting
      * the syntax of the file (e.g., <em>foo.cpp</em> brings to <em>cpp.lang</em>,
      * <em>ChangeLog</em> brings to <em>changelog.lang</em>).
      * This method already catches possible exceptions thrown from the source-highlight
      * library and show them in a message box.
      *
      * @param filename
      * @return the lang def file name or the empty string if no mapping exists
      */
     const QString getLangDefFileFromFileName(const QString &filename);

signals:
      /**
        * This signal is emitted when the highlighting language definition changed
        * @param newLang the new language definition
        */
     void changedHighlightingLanguage(const QString &newLang);

     /**
        * This signal is emitted when the highlighting style changed
        * @param newLang the new language definition
        */
     void changedHighlightingStyle(const QString &newLang);

};
