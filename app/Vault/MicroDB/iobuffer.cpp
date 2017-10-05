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

#include "iobuffer.h"

#include <QIODevice>


IOBuffer::IOBuffer()
		: m_nCurrentPos(0)
{

}

IOBuffer::IOBuffer(const QByteArray &baSeed)
        : m_ba(baSeed),
        m_nCurrentPos(0)
{
}

IOBuffer::IOBuffer( int size )
        : m_nCurrentPos(0)
{
    m_ba.resize( size );
}

void IOBuffer::clear()
{
	m_nCurrentPos = 0;
	m_ba.clear();
}

quint32 IOBuffer::size() const
{
	return m_ba.size();
}


quint32 IOBuffer::currentPosition()
{
	return m_nCurrentPos;
}

bool IOBuffer::hasData()
{
    return (int)m_nCurrentPos < m_ba.size();
}


void IOBuffer::seek(quint32 nPos)
{
    if ( nPos < size())
        m_nCurrentPos = nPos;
}

int IOBuffer::read( QIODevice* ioDevice, qint64 size )
{
    clear();
    m_ba = ioDevice->read( size );
    return m_ba.size();
}


const QByteArray& IOBuffer::getByteArray() const
{
	return m_ba;
}

quint8 IOBuffer::readByte()
{
	quint8 num =0;
	if (ensureSpace(sizeof(quint8)))
	{
		num = m_ba.at(m_nCurrentPos);
		m_nCurrentPos++;
	}
	return num;
}

void IOBuffer::writeByte( quint8 n)
{
	m_ba.insert(m_nCurrentPos++, n);
}

quint32 IOBuffer::readUInt32()
{
	quint32 num = 0;
	if (ensureSpace(sizeof(quint32)))
	{
		num = *((quint32*) m_ba.mid(m_nCurrentPos, sizeof(quint32)).data());
		m_nCurrentPos += sizeof(quint32);
	}

    return num;
}

quint64 IOBuffer::readUInt64()
{
    quint64 num = 0;
    if (ensureSpace(sizeof(quint64)))
    {
        num = *((quint64*) m_ba.mid(m_nCurrentPos, sizeof(quint64)).data());
        m_nCurrentPos += sizeof(quint64);
    }

    return num;
}


void IOBuffer::writeUInt32(quint32 n)
{
    m_ba.insert(m_nCurrentPos, QByteArray::fromRawData((const char*) &n, sizeof(quint32)));
	m_nCurrentPos += sizeof(quint32);
}



void IOBuffer::writeUInt64(quint64 n)
{
    m_ba.insert(m_nCurrentPos, QByteArray::fromRawData((const char*) &n, sizeof(quint64)));
    m_nCurrentPos += sizeof(quint64);
}


quint16 IOBuffer::readUInt16()
{
    quint16 n =0;
	if (ensureSpace(sizeof(quint16)))
	{
        n = *((quint16*) m_ba.mid(m_nCurrentPos, sizeof(quint16)).data());
		m_nCurrentPos += sizeof(quint16);
	}

    return n;
}

void IOBuffer::writeUInt16(quint16 n)
{
    m_ba.insert(m_nCurrentPos, QByteArray::fromRawData((const char*) &n, sizeof(quint16)));
	m_nCurrentPos += sizeof(quint16);
}

QByteArray IOBuffer::readByteArray(quint32 size)
{
	QByteArray ba;
    if (ensureSpace(size))
	{
        ba = m_ba.mid(m_nCurrentPos, size);
        m_nCurrentPos += size;
	}
	return ba;
}

void IOBuffer::writeByteArray(const QByteArray &ba)
{
    //writeUInt32(ba.size());

    m_ba.insert(m_nCurrentPos, ba);
    m_nCurrentPos += ba.size();
}




void IOBuffer::writeRawBytes(const QByteArray &ba)
{
	m_ba.insert(m_nCurrentPos, ba);
	m_nCurrentPos += ba.size();
}

void IOBuffer::writeRawData(const void* data, int nLen)
{
    m_ba.insert(m_nCurrentPos, (char*)data, nLen);
    m_nCurrentPos += nLen;
}


void IOBuffer::pad(quint8 nNumBytes)
{
	for (quint8 i = 0; i < nNumBytes; i++)
	{
		writeByte(0);
	}
}

void IOBuffer::skip(quint8 nNumBytes)
{
	for (quint8 i = 0; i < nNumBytes; i++)
	{
		readByte();
	}
}

bool IOBuffer::ensureSpace(quint32 nSize)
{
	/// return true if we can read nSize bytes without hitting the end of the buffer:
	return (m_nCurrentPos + nSize <= size());
}

