/*
 *  Copyright (C) 2008-2010  Lorenzo Bettini, http://www.lorenzobettini.it
 *  License: See COPYING file that comes with this distribution
 */

#include "Qt4SyntaxHighlighter.h"
#include "Qt4TextFormatter.h"
#include "Qt4TextFormatterFactory.h"
#include "HighlightStateData.h"

#include <QTextCursor>

//#define SHOW_DEBUG_INFO 1

#ifdef SHOW_DEBUG_INFO
#include <iostream>
#define DEBUG_INFO(x,y) std::cout << x << y << std::endl;
#else
#define DEBUG_INFO(x,y)
#endif

/**
 * Utility class to deal with current highlighting state (and stack of states)
 */
struct Qt4HighlightStateData: public QTextBlockUserData {
    HighlightStateData *highlightData;

    /**
     * Constructs a Qt4HighlightStateData using the passed HighlightStateData
     * (this object becomes the owner of the passed HighlightStateData)
     * @param data
     */
    Qt4HighlightStateData(HighlightStateData *data) :
        highlightData(data) {
    }

    virtual ~Qt4HighlightStateData() {
        if (highlightData)
            delete highlightData;
    }

    /**
     * Updates the HighlightStateData of this object, and becomes its owner.
     * If the data of this object was already set, it will be deleted.
     * @param data
     */
    void updateData(HighlightStateData *data) {
        if (highlightData)
            delete highlightData;
        highlightData = data;
    }
};

Qt4SyntaxHighlighter::Qt4SyntaxHighlighter(QTextDocument *doc) :
    QSyntaxHighlighter(doc) {
}

Qt4SyntaxHighlighter::~Qt4SyntaxHighlighter() {
}

void Qt4SyntaxHighlighter::init(const QString &langFile_,
        const QString &styleFile) {
    if (!getFormatterManager()) {
        Qt4TextFormatter *defaultFormatter = new Qt4TextFormatter("normal");
        formatterManager = new srchilite::FormatterManager(TextFormatterPtr(
                defaultFormatter));
        defaultFormatter->setQSyntaxHighlighter(this);
        defaultFormatter->setMonospace(isDefaultToMonospace());

        Qt4TextFormatterFactory f;
        f.setDefaultToMonospace(isDefaultToMonospace());
        setFormatters(getTextFormatterMap(f, styleFile));

        currentStyleFile = styleFile;
    }

    initHighlighter(langFile_);
}

void Qt4SyntaxHighlighter::setFormattingStyle(const QString &styleFile) {
    if (!formatterManager)
        return;

    // reset the formatter manager to remove previous formatters
    formatterManager->reset();

    // make sure we set the foreground and background to a standard color
    setForegroundColor("black");
    setBackgroundColor("white");

    Qt4TextFormatterFactory f;
    f.setDefaultToMonospace(isDefaultToMonospace());
    setFormatters(getTextFormatterMap(f, styleFile));

    currentStyleFile = styleFile;

    // rehighlight the contents with the new formatting style
    rehighlight();
}

void Qt4SyntaxHighlighter::setFormatters(const TextFormatterMap &formatterMap) {
    /*
     * For each element set this QSyntaxHighlighter
     * pointer (the formatters will call setFormat on such pointer).
     */
    for (TextFormatterMap::const_iterator it = formatterMap.begin(); it
            != formatterMap.end(); ++it) {
        Qt4TextFormatter *formatter =
                dynamic_cast<Qt4TextFormatter *> (it->second.get());
        formatter->setQSyntaxHighlighter(this);
        formatterManager->addFormatter(it->first, it->second);
    }

    // now store the color for the normal font, and set the highlighter for the formatter
    Qt4TextFormatter *formatter =
            dynamic_cast<Qt4TextFormatter *> (formatterManager->getFormatter(
                    "normal").get());
    if (formatter) {
        setForegroundColor(
                formatter->getQTextCharFormat().foreground().color().name());
    }
}

Qt4TextFormatterMap Qt4SyntaxHighlighter::getQt4TextFormatterMap() {
    Qt4TextFormatterMap formatterMap;

    // now query the highlighter for all the text formatters
    const srchilite::FormatterMap &map = getFormatterMap();

    for (srchilite::FormatterMap::const_iterator it = map.begin(); it
            != map.end(); ++it) {
        Qt4TextFormatter
                *formatter =
                        dynamic_cast<Qt4TextFormatter *> (it->second.get());
        if (formatter) {
            const std::string &elem = it->first;

            if (formatter == formatterManager->getDefaultFormatter().get()) {
                // create a new one so that for each element we have a different
                // and editable formatter (otherwise, for elements for which no style is
                // defined, i.e., default formatter, we couldn't let the user set a style)
                formatter = new Qt4TextFormatter(*formatter);
                formatter->setElem(elem);
                formatterManager->addFormatter(it->first,
                        srchilite::FormatterPtr(formatter));
            }

            formatterMap[elem.c_str()] = formatter;
        }
        /*
         * we avoid to check
         *  && formatter
         *      != formatterManager->getDefaultFormatter().get()
         * so that we have also language elements for which no color,style was
         * set and the user can then set a new one.
         */
    }

    return formatterMap;
}

bool Qt4SyntaxHighlighter::initFromFileName(const QString &fileName) {
    QString langDefFile = getLangDefFileFromFileName(fileName);
    if (langDefFile == "")
        return false;
    init(langDefFile);
    return true;
}

void Qt4SyntaxHighlighter::highlightBlock(const QString &text) {
    if (isReadOnly()) {
        // this does all the highlighting
        getHighlighter()->highlightParagraph(text.toStdString());

        // and we're done
        return;
    }

    DEBUG_INFO("", "");DEBUG_INFO("highlightBlock: \"", text.toStdString() + "\"");DEBUG_INFO("prevBlockState: ", previousBlockState());

    Qt4HighlightStateData
            *prevBlockData =
                    dynamic_cast<Qt4HighlightStateData *> (currentBlock().previous().userData());

    DEBUG_INFO("current block: \"", currentBlock().text().toStdString()+ "\"");DEBUG_INFO("current block position: ", currentBlock().position());

    Qt4HighlightStateData *currentBlockData =
            dynamic_cast<Qt4HighlightStateData *> (currentBlockUserData());

    HighlightStateData *blockData = 0;

    /*
     * The current block must use the highlighting state of the previous
     * QTextBlock (actually a copy of it)
     */
    if (prevBlockData) {
        // make a copy
        blockData = new HighlightStateData(*(prevBlockData->highlightData));
    }

    // does the actual highlighting
    highlightLine(text, blockData);

    int blockState = 0;
    if (blockData) {
        // we changed the highlighting state
        blockState = blockData->currentState->getId();

        if (!currentBlockData) {
            currentBlockData = new Qt4HighlightStateData(blockData);
        } else {
            currentBlockData->updateData(blockData);
        }
    } else {
        // we're in the main initial state so we don't need any data
        currentBlockData = 0;
    }

    /*
     * If previously this text block had some data set, then set it to
     * 0 will also make the previous data to be deleted
     */

    // we record this data in the current block
    setCurrentBlockUserData(currentBlockData);

    // this is crucial for QSyntaxHighlighter to know whether other parts
    // of the document must be re-highlighted
    setCurrentBlockState(blockState);
    DEBUG_INFO("currentBlockState: ", currentBlockState());
}
