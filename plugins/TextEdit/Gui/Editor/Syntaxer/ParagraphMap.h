/*
 *  Copyright (C) 2008-2010  Lorenzo Bettini, http://www.lorenzobettini.it
 *  License: See COPYING file that comes with this distribution
 */

#pragma once

#include <vector>
#include "HighlightStateData.h"

#define STARTING_SIZE 100

/**
 * Internal class for mapping paragraph numbers to HighlightStateData.
 * Don't use this class directly!
 */
class ParagraphMap {
    typedef std::vector<HighlightStateData *> vectorType;
    vectorType dataVector;
public:
    ParagraphMap();
    ~ParagraphMap();

    /**
     * Inserts data in the given position; if it is the first time we
     * use that position, space is allocated for it.
     * @param pos the position for inserting data; it must be non-negative
     * and if it exceeds the size of the vector it must be of only one position
     * @param data the data to insert; the data is then owned by this class
     */
    void insert(int pos, HighlightStateData *data);

    /**
     * Returns a reference to the data stored at the given position
     * @param pos must be non-negative
     * @return the reference to the pointer of the data
     */
    HighlightStateData *getData(int pos);
};
