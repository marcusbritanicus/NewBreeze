/*
 *  Copyright (C) 2008-2010  Lorenzo Bettini, http://www.lorenzobettini.it
 *  License: See COPYING file that comes with this distribution
 */

#pragma once

#include <QComboBox>

/**
  * A subclass of QComboBox which provides functionalities for showing
  * and selecting the style files of Source-Highlight.
  */
class StyleComboBox : public QComboBox
{
    Q_OBJECT
public:
    /**
      * Initializes the combo box with the .style and .css files retrieved in the
      * specified path.
      * @param path where to look for files (if empty, uses the data dir
      * path of source-highlight)
      */
    StyleComboBox(const QString &path = "");

    /**
      * Initializes the combo box with the .style and .css files retrieved in the
      * specified path.
      * @param parent
      * @param path where to look for files (if empty, uses the data dir
      * path of source-highlight)
      */
    StyleComboBox(QWidget * parent, const QString &path = "");

public slots:
    /**
     * Sets the style file in the combo
     * @param styleFile the style file name; if the file is not part of the
     * combo list, nothing happens
     */
    void setCurrentStyle(const QString &styleFile);

    /**
      * @return the currently selected style file
      */
    const QString getCurrentStyle() const;

    /**
     * Reloads the contents of the combo box by using the specified
     * path for searching for .style and .css files.
     * @param path
     */
    void reload(const QString &path);;

};
