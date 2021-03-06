/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia. For licensing terms and
** conditions see http://qt.digia.com/licensing. For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights. These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QPLACESEARCHREQUEST_H
#define QPLACESEARCHREQUEST_H

#include <QtCore/QSharedDataPointer>
#include <QtLocation/placemacro.h>
#include <QtLocation/QPlaceCategory>
#include <QtLocation/QLocation>

QT_BEGIN_NAMESPACE

class QGeoShape;
class QPlaceSearchRequestPrivate;

class Q_LOCATION_EXPORT QPlaceSearchRequest
{
public:
    enum RelevanceHint {
        UnspecifiedHint,
        DistanceHint,
        LexicalPlaceNameHint
    };

    QPlaceSearchRequest();
    QPlaceSearchRequest(const QPlaceSearchRequest &other);


    QPlaceSearchRequest &operator=(const QPlaceSearchRequest &other);

    bool operator==(const QPlaceSearchRequest &other) const;
    bool operator!=(const QPlaceSearchRequest &other) const;

    ~QPlaceSearchRequest();

    QString searchTerm() const;
    void setSearchTerm(const QString &term);

    QList<QPlaceCategory> categories() const;
    void setCategory(const QPlaceCategory &category);
    void setCategories(const QList<QPlaceCategory> &categories);

    QGeoShape searchArea() const;
    void setSearchArea(const QGeoShape &area);

    QString recommendationId() const;
    void setRecommendationId(const QString &recommendationId);

    QVariant searchContext() const;
    void setSearchContext(const QVariant &context);

    QLocation::VisibilityScope visibilityScope() const;
    void setVisibilityScope(QLocation::VisibilityScope visibilityScopes);

    RelevanceHint relevanceHint() const;
    void setRelevanceHint(RelevanceHint hint);

    int limit() const;
    void setLimit(int limit);

    void clear();

private:
    QSharedDataPointer<QPlaceSearchRequestPrivate> d_ptr;
    inline QPlaceSearchRequestPrivate *d_func();
    inline const QPlaceSearchRequestPrivate *d_func() const;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QPlaceSearchRequest::RelevanceHint)

#endif // QPLACESEARCHQUERY_H
