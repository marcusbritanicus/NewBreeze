/*
 *  Copyright (C) 2008-2010  Lorenzo Bettini, http://www.lorenzobettini.it
 *  License: See COPYING file that comes with this distribution
 */

#pragma once

#include <string>

#include <boost/shared_ptr.hpp>

#include <srchilite/formatter.h>

class FormatterParams;
class QSyntaxHighlighter;

/**
 * A specialization of srchilite::Formatter in order to format parts of
 * a document, instead of outputting the formatted text.
 *
 * Note that the abstract method setQSyntaxHighlighter(QSyntaxHighlighter *).
 * Since this header file does not include the header file for QSyntaxHighlighter,
 * then this same class can be re-used both for Qt3 and for Qt4 (the subclasses
 * of this class will include the correct header file for QSyntaxHighlighter).
 *
 */
class TextFormatter: public srchilite::Formatter {
protected:
    /// the language element represented by this formatter
    std::string elem;

public:
    TextFormatter(const std::string &elem_ = "normal");
    virtual ~TextFormatter();

    /**
     * @return the language element represented by this formatter
     */
    const std::string &getElem() const { return elem; }
    void setElem(const std::string &e) { elem = e; }

    virtual void setQSyntaxHighlighter(QSyntaxHighlighter *qSyntaxHighlighter_) = 0;
};

/// shared pointer for TextFormatter
typedef boost::shared_ptr<TextFormatter> TextFormatterPtr;
