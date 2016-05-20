/*
 *  Copyright (C) 2008-2010  Lorenzo Bettini, http://www.lorenzobettini.it
 *  License: See COPYING file that comes with this distribution
 */

#include <QTextStream>
#include <QFile>

#include "TextEditHighlighted.h"
#include "LanguageComboBox.h"
#include "StyleComboBox.h"
#include "SourceHighlightExceptionBox.h"

#include <srchilite/parserexception.h>
#include <srchilite/ioexception.h>
#include <srchilite/instances.h>

TextEditHighlighted::TextEditHighlighted() :
    highlighter(0), languageComboBox(0), styleComboBox(0), styleFile(
            "default.style") {
    // by default we use C++ as source language
    setHighlighter("cpp.lang");
}

TextEditHighlighted::TextEditHighlighted(QWidget *parent) :
    QTextEdit(parent), highlighter(0), languageComboBox(0), styleComboBox(0),
            styleFile("default.style") {
    // by default we use C++ as source language
    setHighlighter("cpp.lang");
}

// TEXINFOINCLUDE
void TextEditHighlighted::setHighlighter(const QString &langFile) {
    bool errorOnLangFile = false;

    // before removing this highlighter, make sure the language definition
    // file can be loaded.
    try {
        srchilite::Instances::getLangDefManager()->getHighlightState(
                langFile.toStdString());
    } catch (const srchilite::ParserException &pe) {
        SourceHighlightExceptionBox::showMessageBox(pe, this);
        errorOnLangFile = true;
    } catch (const srchilite::IOException &ie) {
        SourceHighlightExceptionBox::showMessageBox(ie, this);
        errorOnLangFile = true;
    } catch (const std::exception &e) {
        SourceHighlightExceptionBox::showMessageBox(e, this);
        errorOnLangFile = true;
    }

    if (!errorOnLangFile) {
        // remove the previous highlighter (which also disconnects it from
        // the current editor, automatically)
        // otherwise there'll be more highlighters for the same doc!
        delete highlighter;

        // set Qt4SyntaxHighlighter for highlighting context
        highlighter = new Qt4SyntaxHighlighter(document());
        highlighter->init(langFile, styleFile);
        changeColors(highlighter->getForegroundColor(),
                highlighter->getBackgroundColor());
    }
}

void TextEditHighlighted::changeColors(const QString &fgColor,
        const QString &bgColor) {
    if (fgColor == "" && bgColor == "")
        return;

    QPalette p = palette();

    if (fgColor != "")
        p.setColor(QPalette::Text, fgColor);
    else
        p.setColor(QPalette::Text, "black");

    if (bgColor != "")
        p.setColor(QPalette::Base, bgColor);
    else
        p.setColor(QPalette::Base, "white");

    setPalette(p);
}

void TextEditHighlighted::connectLanguageComboBox(LanguageComboBox *lcb) {
    languageComboBox = lcb;
    connect(languageComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(
            changeHighlightingLanguage(QString)));
    connect(this, SIGNAL(changedHighlightingLanguage(QString)),
            languageComboBox, SLOT(setCurrentLanguage(QString)));
}

void TextEditHighlighted::connectStyleComboBox(StyleComboBox *lcb) {
    styleComboBox = lcb;
    connect(styleComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(
            changeHighlightingStyle(QString)));
    connect(this, SIGNAL(changedHighlightingStyle(QString)), styleComboBox,
            SLOT(setCurrentStyle(QString)));
}

void TextEditHighlighted::changeHighlightingLanguage(const QString &newLang) {
    // avoid to switch language if it's just the same
    if (newLang == highlighter->getLangFile())
        return;

    setHighlighter(newLang);

    emit changedHighlightingLanguage(newLang);
}

// TEXINFOINCLUDE
void TextEditHighlighted::changeHighlightingStyle(const QString &newStyle) {
    // avoid to switch language if it's just the same
    if (newStyle.isEmpty() || newStyle == highlighter->getFormattingStyle())
        return;

    try {
        // this will also rehighlight the contents
        highlighter->setFormattingStyle(newStyle);
    } catch (const srchilite::ParserException &pe) {
        SourceHighlightExceptionBox::showMessageBox(pe, this);
    } catch (const std::exception &e) {
        SourceHighlightExceptionBox::showMessageBox(e, this);
    }

    changeColors(highlighter->getForegroundColor(),
            highlighter->getBackgroundColor());

    styleFile = newStyle;

    emit changedHighlightingStyle(newStyle);
}
// TEXINFOINCLUDE

const QString TextEditHighlighted::loadFile(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return file.errorString();
    }

    // use the file name to detect the source language for highlighting
    const QString langDefFile = getLangDefFileFromFileName(fileName);

    // make sure possible previous content is now removed
    clear();

    // if the highlight could be detected then set it now
    if (langDefFile != "") {
        changeHighlightingLanguage(langDefFile);
    }

    QTextStream in(&file);
    setPlainText(in.readAll());

    return ""; // no error
}

void TextEditHighlighted::changeFileName(const QString &fileName) {
    if (fileName.isEmpty())
        return;

    // use the file name to detect the source language for highlighting
    const QString langDefFile = getLangDefFileFromFileName(fileName);

    // if the highlight could be detected then set it now
    if (langDefFile != "") {
        changeHighlightingLanguage(langDefFile);
    }
}

const QString TextEditHighlighted::getLangDefFileFromFileName(
        const QString &fileName) {
    if (fileName.isEmpty())
        return "";

    // use the file name to detect the source language for highlighting
    QString langDefFile;

    try {
        langDefFile
                = srchilite::Instances::getLangMap()->getMappedFileNameFromFileName(
                        fileName.toStdString()).c_str();
    } catch (const srchilite::ParserException &pe) {
        SourceHighlightExceptionBox::showMessageBox(pe, this);
    } catch (const srchilite::IOException &ie) {
        SourceHighlightExceptionBox::showMessageBox(ie, this);
    } catch (const std::exception &e) {
        SourceHighlightExceptionBox::showMessageBox(e, this);
    }

    return langDefFile;
}
