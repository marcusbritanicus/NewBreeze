/*
 *  Copyright (C) 2008-2010  Lorenzo Bettini, http://www.lorenzobettini.it
 *  License: See COPYING file that comes with this distribution
 */

#include "SourceHighlightExceptionBox.h"

#include <srchilite/parserexception.h>

#include <QMessageBox>
#include <QObject>

#include <sstream>

void SourceHighlightExceptionBox::showMessageBox(
        const srchilite::ParserException &e, QWidget *parent) {
    std::ostringstream details;
    details << e;
    QMessageBox::warning(
            parent,
            QObject::tr("QSource-Highlight"),
            QObject::tr(
                    "Exception from Source-Highlight library:\n%1\n\ndetails: %2.") .arg(
                    e.what()).arg(details.str().c_str()));
}

void SourceHighlightExceptionBox::showMessageBox(const std::exception &e,
        QWidget *parent) {
    QMessageBox::warning(parent, QObject::tr("QSource-Highlight"), QObject::tr(
            "Exception from Source-Highlight library:\n%1.") .arg(e.what()));
}
