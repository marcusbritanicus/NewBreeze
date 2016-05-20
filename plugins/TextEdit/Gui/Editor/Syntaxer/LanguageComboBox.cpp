/*
 *  Copyright (C) 2008-2010  Lorenzo Bettini, http://www.lorenzobettini.it
 *  License: See COPYING file that comes with this distribution
 */

#include "LanguageComboBox.h"

#include <srchilite/sourcehighlightutils.h>

LanguageComboBox::LanguageComboBox(const QString &path) {
    setToolTip(tr("choose the language definition for highlighting."));

    reload(path);
}

LanguageComboBox::LanguageComboBox( Qt4SyntaxHighlighter *highlighter) {
    setToolTip(tr("choose the language definition for highlighting."));

    std::set<std::string> langfiles =
            highlighter->getLangMap()->getMappedFileNames();
    for (std::set<std::string>::const_iterator it = langfiles.begin(); it
            != langfiles.end(); ++it) {
        addItem(QString((*it).c_str()));
    }
}

void LanguageComboBox::setCurrentLanguage(const QString &lang) {
    int item = findText(lang);
    if (item != -1)
        setCurrentIndex(item);
}

const QString LanguageComboBox::getCurrentLanguage() const {
    return currentText();
}

void LanguageComboBox::reload(const QString &path) {
    const QString &currentItem = getCurrentLanguage();

    // make sure we block signals when we reload the contents
    // otherwise we signal an empty current item
    bool prevState = blockSignals(true);

    clear();

    srchilite::StringSet files =
            srchilite::SourceHighlightUtils::getLangFileNames(path.toStdString());
    for (srchilite::StringSet::const_iterator it = files.begin(); it
            != files.end(); ++it) {
        addItem(QString((*it).c_str()));
    }

    // renable signals
    blockSignals(prevState);

    // and reselect the previous item (if it's still there)
    setCurrentLanguage(currentItem);
}
