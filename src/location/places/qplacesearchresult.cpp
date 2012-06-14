/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qplacesearchresult.h"
#include "qplacesearchresult_p.h"
#include "qplaceresult.h"
#include <QtCore/qnumeric.h>

#if !defined(Q_CC_MWERKS)
template<> QT_PREPEND_NAMESPACE(QPlaceSearchResultPrivate) *
            QSharedDataPointer<QT_PREPEND_NAMESPACE(QPlaceSearchResultPrivate)>::clone()
{
    return d->clone();
}
#endif

QT_USE_NAMESPACE

inline QPlaceSearchResultPrivate *QPlaceSearchResult::d_func()
{
    return static_cast<QPlaceSearchResultPrivate *>(d_ptr.data());
}

inline const QPlaceSearchResultPrivate *QPlaceSearchResult::d_func() const
{
    return static_cast<const QPlaceSearchResultPrivate *>(d_ptr.constData());
}

bool QPlaceSearchResultPrivate::compare(const QPlaceSearchResultPrivate *other) const
{
    return title == other->title
            && icon == other->icon;
}

/*!
    \class QPlaceSearchResult
    \inmodule QtLocation
    \ingroup QtLocation-places
    \ingroup QtLocation-places-data
    \since Qt Location 5.0

    \brief The QPlaceSearchResult class represents a search result.

    Currently the only useful search result type is the
    \l {QPlaceSearchResult::PlaceResult} {place result}.
    For search results of this type, the \l {QPlaceSearchResult::distance()} {distance},
    \l {QPlaceSearchResult::place()}{place} and {QPlaceSearchResult::isSponsored} isSponsored
    fields of the search result is filled in.  Thus the result contains the distance
    from the place from the search center of the search request, the place data containing
    coordinates, address and so on, and finally an indication of whether the
    result is sponsored or \l {http://en.wikipedia.org/wiki/Organic_search}{organic}.
*/

/*!
    \enum QPlaceSearchResult::SearchResultType
    Defines the type of search result
    \value PlaceResult The search result contains a place.
    \value UnknownSearchResult The contents of the search result are unknown.
*/

/*!
    Constructs a new search result.
*/
QPlaceSearchResult::QPlaceSearchResult()
    : d_ptr(new QPlaceSearchResultPrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QPlaceSearchResult::QPlaceSearchResult(const QPlaceSearchResult &other)
    :d_ptr(other.d_ptr)
{
}

/*!
    Destroys the search result.
*/
QPlaceSearchResult::~QPlaceSearchResult()
{
}

/*!
    Assigns \a other to this search result and returns a reference to this
    search result.
*/
QPlaceSearchResult &QPlaceSearchResult::operator =(const QPlaceSearchResult &other)
{
    if (this == &other)
        return *this;

    d_ptr = other.d_ptr;
    return *this;
}

/*!
    Returns true if \a other is equal to this search result, otherwise
    returns false.
*/
bool QPlaceSearchResult::operator==(const QPlaceSearchResult &other) const
{
    // An unknown object is only equal to another unknown search result
    if (!d_ptr)
        return !other.d_ptr;

    if (type() != other.type())
        return false;

    return d_ptr->compare(other.d_ptr);
}

/*!
    \fn bool QPlaceSearchResult::operator!=(const QPlaceSearchResult &other) const
    Returns true if \a other not equal to this search result, otherwise
    returns false.
*/

/*!
    Returns the result type.
*/
QPlaceSearchResult::SearchResultType QPlaceSearchResult::type() const
{
    if (!d_ptr)
        return UnknownSearchResult;
    return d_ptr->type();
}

/*!
    Returns the title of the search result.  This string can be used to display the search result
    to the user.
*/
QString QPlaceSearchResult::title() const
{
    Q_D(const QPlaceSearchResult);
    return d->title;
}

/*!
    Sets the title of the search result to \a title.
*/
void QPlaceSearchResult::setTitle(const QString &title)
{
    Q_D(QPlaceSearchResult);
    d->title = title;
}

/*!
    Returns an icon that can be used to represent the search result.
*/
QPlaceIcon QPlaceSearchResult::icon() const
{
    Q_D(const QPlaceSearchResult);
    return d->icon;
}

/*!
    Sets the icon of the search result to \a icon.
*/
void QPlaceSearchResult::setIcon(const QPlaceIcon &icon)
{
    Q_D(QPlaceSearchResult);
    d->icon = icon;
}

/*!
    \internal
    Constructs a new search result from the given pointer \a d.
*/
QPlaceSearchResult::QPlaceSearchResult(QPlaceSearchResultPrivate *d)
    :d_ptr(d)
{
}
