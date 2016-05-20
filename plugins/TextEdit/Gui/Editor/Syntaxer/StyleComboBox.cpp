/*
 *  Copyright (C) 2008-2010  Lorenzo Bettini, http://www.lorenzobettini.it
 *  License: See COPYING file that comes with this distribution
 */

#include "StyleComboBox.h"
#include <srchilite/sourcehighlightutils.h>

StyleComboBox::StyleComboBox(QWidget * parent, const QString &path)
: QComboBox(parent)
{
	setToolTip(tr("choose the style for highlighting."));
	reload(path);
}

StyleComboBox::StyleComboBox(const QString &path) {
    setToolTip(tr("choose the style for highlighting."));

    reload(path);
}

void StyleComboBox::setCurrentStyle(const QString &styleFile) {
    int item = findText(styleFile);
    if (item != -1)
        setCurrentIndex(item);
}

const QString StyleComboBox::getCurrentStyle() const {
    return currentText();
}

void StyleComboBox::reload(const QString &path) {
    const QString &currentItem = getCurrentStyle();

    // make sure we block signals when we reload the contents
    // otherwise we signal an empty current item
    bool prevState = blockSignals(true);

    clear();

    srchilite::StringSet styleFiles =
            srchilite::SourceHighlightUtils::getStyleFileNames(path.toStdString());
    for (srchilite::StringSet::const_iterator it = styleFiles.begin(); it
            != styleFiles.end(); ++it) {
        addItem(QString((*it).c_str()));
    }

    // a separator before .css files
    insertSeparator(count() + 1);

    styleFiles = srchilite::SourceHighlightUtils::getCssStyleFileNames(path.toStdString());
    for (srchilite::StringSet::const_iterator it = styleFiles.begin(); it
            != styleFiles.end(); ++it) {
        addItem(QString((*it).c_str()));
    }

    // renable signals
    blockSignals(prevState);

    // and reselect the previous item (if it's still there)
    setCurrentStyle(currentItem);
}
