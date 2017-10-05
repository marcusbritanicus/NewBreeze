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

#ifndef ZIPGLOBAL_H
#define ZIPGLOBAL_H

//#ifdef ZIP_STATIC
//# define ZIP_EXPORT
//#else
# if !defined(ZIP_STATIC_LIBRARY)
#   define ZIP_EXPORT Q_DECL_EXPORT
# else
#   define ZIP_EXPORT Q_DECL_IMPORT
# endif
//#endif



#endif // ZIPGLOBAL_H
