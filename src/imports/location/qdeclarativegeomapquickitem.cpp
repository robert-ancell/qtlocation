/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativegeomapquickitem_p.h"
#include "qdeclarativecoordinate_p.h"
#include <QtDeclarative/qdeclarativeinfo.h>

#include <QDebug>
#include <cmath>

QT_BEGIN_NAMESPACE

/*!
    \qmlclass MapQuickItem

    The MapQuickItem element is part of the \bold{QtLocation 5.0} module.
*/

QDeclarativeGeoMapQuickItem::QDeclarativeGeoMapQuickItem(QQuickItem *parent)
    : QDeclarativeGeoMapItemBase(parent),
      coordinate_(0),
      sourceItem_(0),
      zoomLevel_(0.0),
      inUpdate_(false),
      mapAndSourceItemSet_(false),
      dragActive_(true) {}

QDeclarativeGeoMapQuickItem::~QDeclarativeGeoMapQuickItem() {}

void QDeclarativeGeoMapQuickItem::setCoordinate(QDeclarativeCoordinate *coordinate)
{
    if (coordinate_ == coordinate)
        return;
    if (coordinate_)
        coordinate_->disconnect(this);
    coordinate_ = coordinate;
    update();
    if (coordinate_) {
        connect(coordinate_,
                SIGNAL(latitudeChanged(double)),
                this,
                SLOT(coordinateCoordinateChanged(double)));
        connect(coordinate_,
                SIGNAL(longitudeChanged(double)),
                this,
                SLOT(coordinateCoordinateChanged(double)));
        connect(coordinate_,
                SIGNAL(altitudeChanged(double)),
                this,
                SLOT(coordinateCoordinateChanged(double)));
    }
    emit coordinateChanged();
}

void QDeclarativeGeoMapQuickItem::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    if (!dragActive_ && quickMap() && sourceItem() && newGeometry.isValid() && newGeometry != oldGeometry) {
        QPointF point(newGeometry.x(), newGeometry.y());
        // screenPositionToCoordinate seems to return nan values when
        // it goes beyond viewport, hence sanity check (fixme todo):
        QGeoCoordinate newCoordinate = map_->screenPositionToCoordinate(point, false);
        if (newCoordinate.isValid()) {
            internalCoordinate_.setCoordinate(newCoordinate);
            setCoordinate(&internalCoordinate_);
        }
    }
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
}

void QDeclarativeGeoMapQuickItem::dragStarted()
{
    dragActive_ = true;
}

void QDeclarativeGeoMapQuickItem::dragEnded()
{
    if (!dragActive_)
        return;
    dragActive_ = false;
    if (quickMap() && sourceItem()) {
        QPointF point(x(), y());
        // screenPositionToCoordinate seems to return nan values when
        // it goes beyond viewport, hence sanity check (fixme todo):
        QGeoCoordinate newCoordinate = map_->screenPositionToCoordinate(point, false);
        if (newCoordinate.isValid()) {
            internalCoordinate_.setCoordinate(newCoordinate);
            setCoordinate(&internalCoordinate_);
        }
    }
}

void QDeclarativeGeoMapQuickItem::coordinateCoordinateChanged(double)
{
    updateMapItem();
    emit coordinateChanged();
}

QDeclarativeCoordinate* QDeclarativeGeoMapQuickItem::coordinate()
{
    return coordinate_;
}

void QDeclarativeGeoMapQuickItem::setSourceItem(QQuickItem* sourceItem)
{
    if (sourceItem == sourceItem_)
        return;
    sourceItem_ = sourceItem;
    mapChanged();
    emit sourceItemChanged();
}

void QDeclarativeGeoMapQuickItem::updateMapItem()
{
    if (!mapAndSourceItemSet_)
        return;
    QDeclarativeGeoMapItemBase::updateMapItem();
}

void QDeclarativeGeoMapQuickItem::mapChanged()
{
    // currently this function checks for source item changes too
    if (!quickMap() && sourceItem_) {
        mapAndSourceItemSet_ = false;
        sourceItem_->setParentItem(0);
        return;
    }
    if (!quickMap() || !map_ || !sourceItem_) {
        mapAndSourceItemSet_ = false;
        return;
    }
    if (!mapAndSourceItemSet_ && quickMap() && map_ && sourceItem_) {
        mapAndSourceItemSet_ = true;
        sourceItem_->setParentItem(this);
        sourceItem_->setTransformOrigin(QQuickItem::TopLeft);
        connect(quickMap(), SIGNAL(heightChanged()), this, SLOT(updateMapItem()));
        connect(quickMap(), SIGNAL(widthChanged()), this, SLOT(updateMapItem()));
        connect(map_, SIGNAL(cameraDataChanged(CameraData)), this, SLOT(updateMapItem()));
        connect(sourceItem_, SIGNAL(xChanged()), this, SLOT(updateMapItem()));
        connect(sourceItem_, SIGNAL(yChanged()), this, SLOT(updateMapItem()));
        connect(sourceItem_, SIGNAL(widthChanged()), this, SLOT(updateMapItem()));
        connect(sourceItem_, SIGNAL(heightChanged()), this, SLOT(updateMapItem()));
    }
    updateMapItem();
}

QQuickItem* QDeclarativeGeoMapQuickItem::sourceItem()
{
    return sourceItem_;
}

void QDeclarativeGeoMapQuickItem::setAnchorPoint(const QPointF &anchorPoint)
{
    if (anchorPoint == anchorPoint_)
        return;
    anchorPoint_ = anchorPoint;
    updateMapItem();
    emit anchorPointChanged();
}

QPointF QDeclarativeGeoMapQuickItem::anchorPoint() const
{
    return anchorPoint_;
}

void QDeclarativeGeoMapQuickItem::setZoomLevel(qreal zoomLevel)
{
    if (zoomLevel == zoomLevel_)
        return;
    zoomLevel_ = zoomLevel;
    updateMapItem();
    emit zoomLevelChanged();
}

qreal QDeclarativeGeoMapQuickItem::zoomLevel() const
{
    return zoomLevel_;
}

void QDeclarativeGeoMapQuickItem::updateContent()
{
    sourceItem_->setScale(scaleFactor());
    sourceItem_->setPos(QPointF(0,0));
    setWidth(sourceItem_->width());
    setHeight(sourceItem_->height());
}

QPointF QDeclarativeGeoMapQuickItem::contentTopLeftPoint()
{
    if (!map_)
        return QPointF(0,0);
    return map_->coordinateToScreenPosition(coordinate()->coordinate(), false) - scaleFactor() * anchorPoint_;
}

qreal QDeclarativeGeoMapQuickItem::scaleFactor()
{
    qreal scale = 1.0;
    if (zoomLevel_ != 0.0)
        scale = pow(0.5, zoomLevel_ - map_->cameraData().zoomFactor());
    return scale;
}

#include "moc_qdeclarativegeomapquickitem_p.cpp"

QT_END_NAMESPACE
