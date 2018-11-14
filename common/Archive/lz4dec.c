// ///////////////////////////////////////
// smallz4cat.c
// Copyright (c) 2016-2018 Stephan Brumme. All rights reserved.
// see https://create.stephan-brumme.com/smallz4/
//
// "MIT License":
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the Software
// is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// This program is a shorter, more readable, albeit slower re-implementation of lz4cat ( https://github.com/Cyan4973/xxHash )

// compile: gcc smallz4cat.c -O3 -o smallz4cat -Wall -pedantic -std=c99 -s
// The static 8k binary was compiled using Clang and dietlibc (see https://www.fefe.de/dietlibc/ )

// Limitations:
// - skippable frames and legacy frames are not implemented (and most likely never will)
// - checksums are not verified (see https://create.stephan-brumme.com/xxhash/ for a simple implementation)

// Replace getByteFromIn() and sendToOut() by your own code if you need in-memory LZ4 decompression.
// Corrupted data causes a call to error().

#define READ_BUFFER_SIZE 4*1024

#include <stdint.h> // uint32_t
#include <stdio.h>  // stdin/stdout/stderr, fopen, ...
#include <stdlib.h> // exit()
#include <string.h> // memcpy

#include "lz4dec.h"

// error handler
void error(const char* msg) {
	// smaller static binary than fprintf(stderr, "ERROR: %s\n", msg);
	fputs("ERROR: ", stderr);
	fputs(msg,       stderr);
	fputs("\n",      stderr);
	exit(  1);
}

static unsigned char getByte( FILE *in ) {
	// modify buffer size as you like ... for most use cases, bigger buffer aren't faster anymore - and even reducing to 1 byte works !
	static unsigned char readBuffer[READ_BUFFER_SIZE];
	static size_t        pos       = 0;
	static size_t        available = 0;

	// refill buffer
	if (pos == available)
	{
		pos = 0;
		available = fread(readBuffer, 1, READ_BUFFER_SIZE, in);
		if (available == 0)
			error("out of data");
	}

	// return a byte
	return readBuffer[pos++];
}

static void sendBytes(const unsigned char* data, unsigned int numBytes, FILE *out) {
	if (data != NULL && numBytes > 0)
		fwrite(data, 1, numBytes, out);
}

void unlz4( const char *ifn, const char *ofn, const char* dictionary) {

	FILE *inpFp = fopen( ifn, "rb" );
	FILE *outFp = fopen( ofn, "wb" );

	// signature
	unsigned char signature1 = getByte( inpFp );
	unsigned char signature2 = getByte( inpFp );
	unsigned char signature3 = getByte( inpFp );
	unsigned char signature4 = getByte( inpFp );
	uint32_t signature = (signature4 << 24) | (signature3 << 16) | (signature2 << 8) | signature1;
	int isModern = (signature == 0x184D2204);
	int isLegacy = (signature == 0x184C2102);
	if (!isModern && !isLegacy)
		error("invalid signature");

	unsigned char hasBlockChecksum   = 0;
	unsigned char hasContentSize     = 0;
	unsigned char hasContentChecksum = 0;
	unsigned char hasDictionaryID    = 0;
	if (isModern) {
		// flags
		unsigned char flags = getByte( inpFp );
		hasBlockChecksum   = flags & 16;
		hasContentSize     = flags &  8;
		hasContentChecksum = flags &  4;
		hasDictionaryID    = flags &  1;

		// only version 1 file format
		unsigned char version = flags >> 6;
		if (version != 1)
			error("only LZ4 file format version 1 supported");

		// ignore blocksize
		getByte( inpFp );

		if (hasContentSize)
		{
			// ignore, skip 8 bytes
			getByte( inpFp ); getByte( inpFp ); getByte( inpFp ); getByte( inpFp );
			getByte( inpFp ); getByte( inpFp ); getByte( inpFp ); getByte( inpFp );
		}
		if (hasDictionaryID)
		{
			// ignore, skip 4 bytes
			getByte( inpFp ); getByte( inpFp ); getByte( inpFp ); getByte( inpFp );
		}

		// ignore header checksum (xxhash32 of everything up this point & 0xFF)
		getByte( inpFp );
	}

	// don't lower this value, backreferences can be 64kb far away
#define HISTORY_SIZE 64*1024
	// contains the latest decoded data
	unsigned char history[HISTORY_SIZE];
	// next free position in history[]
	unsigned int  pos = 0;

	// dictionary compression is a recently introduced feature, just move its contents to the buffer
	if (dictionary != NULL)
	{
		// open dictionary
		FILE* dict = fopen(dictionary, "rb");
		if (!dict)
			error("cannot open dictionary");

		// get dictionary's filesize
		fseek(dict, 0, SEEK_END);
		size_t dictSize = ftell(dict);
		// only the last 64k are relevant
		size_t relevant = dictSize < 65536 ? 0 : dictSize - 65536;
		fseek(dict, (long)relevant, SEEK_SET);
		if (dictSize > 65536)
			dictSize = 65536;
		// read it and store it at the end of the buffer
		fread(history + HISTORY_SIZE - dictSize, 1, dictSize, dict);
		fclose(dict);
	}

	// parse all blocks until blockSize == 0
	while (1)
	{
		// block size
		uint32_t blockSize = getByte( inpFp );
		blockSize |= (uint32_t)getByte( inpFp ) <<  8;
		blockSize |= (uint32_t)getByte( inpFp ) << 16;
		blockSize |= (uint32_t)getByte( inpFp ) << 24;

		// highest bit set ?
		unsigned char isCompressed = isLegacy || (blockSize & 0x80000000) == 0;
		if (isModern)
			blockSize &= 0x7FFFFFFF;

		// stop after last block
		if (blockSize == 0)
			break;

		if (isCompressed)
		{
			// decompress block
			uint32_t blockOffset = 0;
			while (blockOffset < blockSize)
			{
				// get a token
				unsigned char token = getByte( inpFp );

				blockOffset++;

				// determine number of literals
				uint32_t numLiterals = (token >> 4) & 0x0F;
				if (numLiterals == 15)
				{
					// number of literals length encoded in more than 1 byte
					unsigned char current;
					do
					{
						current = getByte( inpFp );
						numLiterals += current;
						blockOffset++;
					} while (current == 255);
				}

				blockOffset += numLiterals;
				// copy all those literals
				while (numLiterals-- > 0)
				{
					history[pos++] = getByte( inpFp );

					// flush output buffer
					if (pos == HISTORY_SIZE)
					{
						sendBytes(history, HISTORY_SIZE, outFp);
						pos = 0;
					}
				}

				// last token has only literals
				if (blockOffset == blockSize)
					break;

				// match distance is encoded by two bytes (little endian)
				blockOffset += 2;
				uint32_t delta = getByte( inpFp );
				delta |= (uint32_t)getByte( inpFp ) << 8;
				// zero isn't allowed
				if (delta == 0)
					error("invalid offset");

				// match length (must be >= 4, therefore length is stored minus 4)
				uint32_t matchLength = 4 + (token & 0x0F);
				if (matchLength == 4 + 0x0F)
				{
					unsigned char current;
					do // match length encoded in more than 1 byte
					{
						current = getByte( inpFp );
						matchLength += current;
						blockOffset++;
					} while (current == 255);
				}

				// copy match
				uint32_t reference = (pos >= delta) ? pos - delta : HISTORY_SIZE + pos - delta;
				if (pos + matchLength < HISTORY_SIZE && reference + matchLength < HISTORY_SIZE)
				{
					// fast copy
					if (pos >= reference + matchLength || reference >= pos + matchLength)
					{
						// non-overlapping
						memcpy(history + pos, history + reference, matchLength);
						pos += matchLength;
					}
					else
					{
						// overlapping
						while (matchLength-- > 0)
							history[pos++] = history[reference++];
					}
				}
				else
				{
					// slower copy, have to take care of buffer limits
					while (matchLength-- > 0)
					{
						// copy single byte
						history[pos++] = history[reference++];

						// cannot write anymore ? => wrap around
						if (pos == HISTORY_SIZE)
						{
							// flush output buffer
							sendBytes(history, HISTORY_SIZE, outFp);
							pos = 0;
						}
						// cannot read anymore ? => wrap around
						if (reference == HISTORY_SIZE)
							reference = 0;
					}
				}
			}

			// all legacy blocks must be completely filled - except for the last one
			if (isLegacy && blockSize < 8*1024*1024)
				break;
		}
		else
		{
			// copy uncompressed data and add to history, too (if next block is compressed and some matches refer to this block)
			while (blockSize-- > 0)
			{
				// copy a byte ...
				history[pos++] = getByte( inpFp );
				// ... until buffer is full => send to output
				if (pos == HISTORY_SIZE)
				{
					sendBytes(history, HISTORY_SIZE, outFp);
					pos = 0;
				}
			}
		}

		if (hasBlockChecksum)
		{
			// ignore checksum, skip 4 bytes
			getByte( inpFp ); getByte( inpFp ); getByte( inpFp ); getByte( inpFp );
		}
	}

	if (hasContentChecksum)
	{
		// ignore checksum, skip 4 bytes
		getByte( inpFp ); getByte( inpFp ); getByte( inpFp ); getByte( inpFp );
	}

	// flush output buffer
	sendBytes(history, pos, outFp);
}
