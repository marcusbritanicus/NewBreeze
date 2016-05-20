/*
 *  Copyright (C) 2008-2010  Lorenzo Bettini, http://www.lorenzobettini.it
 *  License: See COPYING file that comes with this distribution
 */

#pragma once

#include <srchilite/sourcehighlighter.h>

/**
 * Utility class to deal with current highlighting state (and stack of states)
 */
struct HighlightStateData {
    /// the current state for the SourceHighlighter object
    srchilite::HighlightStatePtr currentState;

    /// the current stack for the SourceHighlighter object
    srchilite::HighlightStateStackPtr stateStack;

    HighlightStateData() {
    }

    /**
     * Performs a deep copy of the passed object (by duplicating the stack)
     * @param data
     */
    HighlightStateData(const HighlightStateData& data) :
                currentState(data.currentState),
                stateStack(srchilite::HighlightStateStackPtr(
                        new srchilite::HighlightStateStack(*(data.stateStack)))) {
    }

    HighlightStateData(srchilite::HighlightStatePtr currentState_,
            srchilite::HighlightStateStackPtr stateStack_) :
        currentState(currentState_), stateStack(stateStack_) {
    }

    /**
     * Performs a deep copy of the passed object (by duplicating the stack)
     */
    void copyFrom(const HighlightStateData& data) {
        currentState = data.currentState;
        stateStack = srchilite::HighlightStateStackPtr(
                new srchilite::HighlightStateStack(*(data.stateStack)));
    }
};
