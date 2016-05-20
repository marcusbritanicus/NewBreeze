/*
 *  Copyright (C) 2008-2010  Lorenzo Bettini, http://www.lorenzobettini.it
 *  License: See COPYING file that comes with this distribution
 */

#pragma once

#include <exception>

class QWidget;

namespace srchilite {
    class ParserException;
}

/**
 * Utility functions to show message boxes related to Source-Highlight
 * library thrown exceptions
 *
 * @since 0.2
 */
class SourceHighlightExceptionBox {
public:
    /**
     * Shows a message box with the details of the ParserException exception
     * (e.g., due to a syntax error in a language definition file)
     * @param e the exception
     * @param parent the parent of the message box
     */
    static void showMessageBox(const srchilite::ParserException &e, QWidget *parent = 0);

    /**
     * Shows a message box with the details of the std::exception
     * @param e the exception
     * @param parent the parent of the message box
     */
    static void showMessageBox(const std::exception &e, QWidget *parent = 0);
};
