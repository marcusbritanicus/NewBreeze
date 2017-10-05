/****************************************************************************
**
** Copyright (C) 2016 Andy Bray
**
** This file is part of AbZip.
**
** AbZip is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** AbZip is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with AbZip.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef IOBUFFER_H
#define IOBUFFER_H

#include <QObject>
#include <QIODevice>

///\brief Provides a message buffer that can be used to read & write data across the network.
class IOBuffer
{
public:
	/// construct a new, empty buffer:
    IOBuffer();

    /// Construct a new buffer with a given size
    IOBuffer( int size );

    /// Construct a new buffer, but seed with the contents of a ByteArray.
    IOBuffer(const QByteArray &baSeed);

	/// Clear the contents of the message:
	void clear();

    int read( QIODevice* ioDevice, qint64 size );

    /// returns true if there is still data in the buffer to read
    bool hasData();

	/// get message size:
	quint32 size() const;

	quint32 currentPosition();

	void seek(quint32 nPos);

	/// get a copy of the message as a byte array:
    const QByteArray& getByteArray() const;

	/// read a single byte. Modifies internal position:
	quint8 readByte();
	/// Write a single byte.
	void writeByte( quint8 n);

	/// reads a four byte unsigned integer from the message, and moves the current position one by 4 bytes.
    quint32 readUInt32();

	/// writes four bytes at the current position and moves the position forwards by 4 bytes.
    void writeUInt32(quint32 n);

    /// reads a 8 byte unsigned long from the message, and moves the current position one by 4 bytes.
    quint64 readUInt64();


    /// writes 8 bytes at the current position and moves the position forwards by 8 bytes.
    void writeUInt64(quint64 n);

    /// reads a string of size length. Modifies position.
    QByteArray readByteArray(quint32 size);
	/// writes data size, then data (without nulls) to message. Modifies position.
	void writeByteArray(const QByteArray &ba);


	/// Write some raw bytes to the buffer array. This can be used to concatenate two messages together.
	/// No syntax checking is done, so it's important that the source byte array comes from another net message buffer.
	void writeRawBytes(const QByteArray &ba);

    /// Write raw byte data to buffer
    void writeRawData(const void* data, int nLen);

	/// Read an unsigned short:
    quint16 readUInt16();
	/// Write an unsigned short:
    void writeUInt16(quint16 n);

	/// Pad a certain number of empty (0) bytes:
	void pad(quint8 nNumBytes);
	/// Skip a number of bytes - to be used with the pad() method above:
	void skip(quint8 nNumBytes);

private:
	QByteArray m_ba;
	quint32 m_nCurrentPos;

	bool ensureSpace(quint32 nSize);

};

#endif // IOBUFFER_H
