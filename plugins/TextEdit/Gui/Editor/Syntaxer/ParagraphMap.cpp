/*
 *  Copyright (C) 2008-2010  Lorenzo Bettini, http://www.lorenzobettini.it
 *  License: See COPYING file that comes with this distribution
 */

#include "ParagraphMap.h"

using namespace std;

ParagraphMap::ParagraphMap() : dataVector(vectorType(STARTING_SIZE)) {
    // let's create some (empty) elements, 100 should be enough to start
    const vectorType::size_type s = 100;
    dataVector.resize(s);
    for (vectorType::iterator it = dataVector.begin(); it
            != dataVector.end(); ++it) {
        *it = 0;
    }
}

ParagraphMap::~ParagraphMap() {
    for (vectorType::const_iterator it = dataVector.begin(); it
            != dataVector.end(); ++it) {
        if (*it) {
            delete *it;
        }
    }
}

void ParagraphMap::insert(int pos, HighlightStateData *data) {
    if (pos < (int)dataVector.size()) {
        if (dataVector[pos]) {
            // first remove possible previous data
            delete dataVector[pos];
        }
        dataVector[pos] = data;
    } else {
        //add it to the end (and automatically extend the size)
        dataVector.push_back(data);
    }
}

HighlightStateData *ParagraphMap::getData(int pos) {
    return dataVector[pos];
}
