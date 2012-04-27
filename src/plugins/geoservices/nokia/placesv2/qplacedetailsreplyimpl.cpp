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
** This file is part of the Ovi services plugin for the Maps and
** Navigation API.  The use of these services, whether by use of the
** plugin or by other means, is governed by the terms and conditions
** described by the file OVI_SERVICES_TERMS_AND_CONDITIONS.txt in
** this package, located in the directory containing the Ovi services
** plugin source code.
**
****************************************************************************/

#include "qplacedetailsreplyimpl.h"
#include "jsonparserhelpers.h"
#include "../qplacemanagerengine_nokiav2.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtNetwork/QNetworkReply>
#include <QtLocation/QPlaceManager>
#include <QtLocation/QPlaceSupplier>
#include <QtLocation/QPlaceImage>
#include <QtLocation/QPlaceEditorial>
#include <QtLocation/QPlaceReview>
#include <QtLocation/QPlaceUser>

QT_BEGIN_NAMESPACE

// These countries format the street address as: {house number} {street name}
// All other countries format it as: {street name} {house number}
static const char COUNTRY_TABLE_string[] =
    "CAN\0"
    "NZL\0"
    "GBR\0"
    "AUS\0"
    "LKA\0"
    "USA\0"
    "SGP\0"
    "FRA\0"
    "BHS\0"
    "CHN\0"
    "IND\0"
    "IRL\0"
    "ARE\0"
    "\0";

static const int COUNTRY_TABLE_indices[] = {
       0,    4,    8,   12,   16,   20,   24,   28,
      32,   36,   40,   44,   48,   -1
};

static bool countryTableContains(const QString &countryCode)
{
    for (int i = 0; COUNTRY_TABLE_indices[i] != -1; ++i) {
        if (countryCode == QLatin1String(COUNTRY_TABLE_string + COUNTRY_TABLE_indices[i]))
            return true;
    }

    return false;
}

QPlaceDetailsReplyImpl::QPlaceDetailsReplyImpl(QNetworkReply *reply,
                                               QPlaceManagerEngineNokiaV2 *parent)
    :   QPlaceDetailsReply(parent), m_reply(reply), m_engine(parent)
{
    Q_ASSERT(parent);

    if (!m_reply)
        return;

    m_reply->setParent(this);
    connect(m_reply, SIGNAL(finished()), this, SLOT(replyFinished()));
    connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(replyError(QNetworkReply::NetworkError)));
}

QPlaceDetailsReplyImpl::~QPlaceDetailsReplyImpl()
{
}

void QPlaceDetailsReplyImpl::abort()
{
    if (m_reply)
        m_reply->abort();
}

void QPlaceDetailsReplyImpl::setError(QPlaceReply::Error error_, const QString &errorString)
{
    QPlaceReply::setError(error_, errorString);
    emit error(error_, errorString);
    setFinished(true);
    emit finished();
}

void QPlaceDetailsReplyImpl::replyFinished()
{
    QJsonDocument document = QJsonDocument::fromJson(m_reply->readAll());
    if (!document.isObject()) {
        setError(ParseError, tr("Error parsing response."));
        return;
    }

    QJsonObject object = document.object();

    QPlace place;

    place.setPlaceId(object.value(QLatin1String("placeId")).toString());

    //const QString refId = object.value(QLatin1String("refId")).toString();
    //const QUrl view = object.value(QLatin1String("view")).toString();

    place.setName(object.value(QLatin1String("name")).toString());

    //if (object.contains(QLatin1String("distance")))
    //    double distance = object.value(QLatin1String("distance")).toDouble();

    //if (object.contains(QLatin1String("alternativeNames"))) {
    //    QJsonArray alternativeNames = object.value(QLatin1String("alternativeNames")).toArray();
    //}

    QGeoLocation location;

    QJsonObject locationObject = object.value(QLatin1String("location")).toObject();

    //if (locationObject.contains(QLatin1String("locationId")))
    //    const QString locationId = locationObject.value(QLatin1String("locationId")).toString();

    QJsonArray position = locationObject.value(QLatin1String("position")).toArray();
    location.setCoordinate(QGeoCoordinate(position.at(0).toDouble(), position.at(1).toDouble()));

    QGeoAddress address;

    QJsonObject addressObject = locationObject.value(QLatin1String("address")).toObject();

    address.setText(addressObject.value(QLatin1String("text")).toString());

    address.setCountry(addressObject.value(QLatin1String("country")).toString());
    address.setCountryCode(addressObject.value(QLatin1String("countryCode")).toString());

    QString house;
    QString street;

    if (addressObject.contains(QLatin1String("house")))
        house = addressObject.value(QLatin1String("house")).toString();
    if (addressObject.contains(QLatin1String("street")))
        street = addressObject.value(QLatin1String("street")).toString();

    if (countryTableContains(address.countryCode())) {
        if (!house.isEmpty() && !street.startsWith(house))
            street = house + QLatin1Char(' ') + street;
    } else {
        if (!house.isEmpty() && !street.endsWith(house))
            street += QLatin1Char(' ') + house;
    }

    address.setStreet(street);

    if (addressObject.contains(QLatin1String("city")))
        address.setCity(addressObject.value(QLatin1String("city")).toString());
    if (addressObject.contains(QLatin1String("district")))
        address.setDistrict(addressObject.value(QLatin1String("district")).toString());
    if (addressObject.contains(QLatin1String("state")))
        address.setState(addressObject.value(QLatin1String("state")).toString());
    if (addressObject.contains(QLatin1String("county")))
        address.setCounty(addressObject.value(QLatin1String("county")).toString());
    if (addressObject.contains(QLatin1String("postalCode")))
        address.setPostalCode(addressObject.value(QLatin1String("postalCode")).toString());

    location.setAddress(address);

    if (locationObject.contains(QLatin1String("bbox"))) {
        QJsonArray bbox = locationObject.value(QLatin1String("bbox")).toArray();
        QGeoBoundingBox box(QGeoCoordinate(bbox.at(3).toDouble(), bbox.at(0).toDouble()),
                            QGeoCoordinate(bbox.at(1).toDouble(), bbox.at(2).toDouble()));
        location.setBoundingBox(box);
    }

    place.setLocation(location);

    place.setCategories(parseCategories(object.value(QLatin1String("categories")).toArray(),
                                        m_engine));

    QString iconPath = m_engine->iconPath(
                            object.value(QLatin1String("icon")).toString());
    QVariantMap parameters;
    parameters.insert(QPlaceIcon::SingleUrl, QUrl(iconPath));
    QPlaceIcon icon;
    icon.setParameters(parameters);
    place.setIcon(icon);

    if (object.contains(QLatin1String("contacts"))) {
        QJsonObject contactsObject = object.value(QLatin1String("contacts")).toObject();

        if (contactsObject.contains(QLatin1String("phone"))) {
            place.setContactDetails(QPlaceContactDetail::Phone,
                                    parseContactDetails(contactsObject.value(QLatin1String("phone")).toArray()));
        }
        if (contactsObject.contains(QLatin1String("fax"))) {
            place.setContactDetails(QPlaceContactDetail::Fax,
                                    parseContactDetails(contactsObject.value(QLatin1String("fax")).toArray()));
        }
        if (contactsObject.contains(QLatin1String("website"))) {
            place.setContactDetails(QPlaceContactDetail::Website,
                                    parseContactDetails(contactsObject.value(QLatin1String("website")).toArray()));
        }
        if (contactsObject.contains(QLatin1String("email"))) {
            place.setContactDetails(QPlaceContactDetail::Email,
                                    parseContactDetails(contactsObject.value(QLatin1String("email")).toArray()));
        }
    }

    //if (object.contains(QLatin1String("verifiedByOwner")))
    //    bool verifiedByOwner = object.value(QLatin1String("verifiedByOwner")).toBool();

    if (object.contains(QLatin1String("attribution")))
        place.setAttribution(object.value(QLatin1String("attribution")).toString());

    if (object.contains(QLatin1String("supplier")))
        place.setSupplier(parseSupplier(object.value(QLatin1String("supplier")).toObject()));

    if (object.contains(QLatin1String("ratings"))) {
        QJsonObject ratingsObject = object.value(QLatin1String("ratings")).toObject();

        QPlaceRatings ratings;
        ratings.setAverage(ratingsObject.value(QLatin1String("average")).toDouble());
        ratings.setCount(ratingsObject.value(QLatin1String("count")).toString().toInt());
        ratings.setMaximum(5.0);

        place.setRatings(ratings);
    }

    if (object.contains(QLatin1String("extended"))) {
        QJsonObject extendedObject = object.value(QLatin1String("extended")).toObject();

        foreach (const QString &key, extendedObject.keys()) {
            QJsonObject attributeObject = extendedObject.value(key).toObject();

            QPlaceAttribute attribute;

            attribute.setLabel(attributeObject.value(QLatin1String("label")).toString());
            attribute.setText(attributeObject.value(QLatin1String("text")).toString());

            if (key == QLatin1String("payment"))
                place.setExtendedAttribute(QPlaceAttribute::Payment, attribute);
            else if (key == QLatin1String("openingHours"))
                place.setExtendedAttribute(QPlaceAttribute::OpeningHours, attribute);
            else
                place.setExtendedAttribute(key, attribute);
        }
    }

    if (object.contains(QLatin1String("media"))) {
        QJsonObject mediaObject = object.value(QLatin1String("media")).toObject();

        if (mediaObject.contains(QLatin1String("images"))) {
            QPlaceContent::Collection collection;
            int totalCount = 0;

            parseCollection(QPlaceContent::ImageType,
                            mediaObject.value(QLatin1String("images")).toObject(),
                            &collection, &totalCount);

            place.setTotalContentCount(QPlaceContent::ImageType, totalCount);
            place.setContent(QPlaceContent::ImageType, collection);
        }
        if (mediaObject.contains(QLatin1String("editorials"))) {
            QPlaceContent::Collection collection;
            int totalCount = 0;

            parseCollection(QPlaceContent::EditorialType,
                            mediaObject.value(QLatin1String("editorials")).toObject(),
                            &collection, &totalCount);

            place.setTotalContentCount(QPlaceContent::EditorialType, totalCount);
            place.setContent(QPlaceContent::EditorialType, collection);
        }
        if (mediaObject.contains(QLatin1String("reviews"))) {
            QPlaceContent::Collection collection;
            int totalCount = 0;

            parseCollection(QPlaceContent::ReviewType,
                            mediaObject.value(QLatin1String("reviews")).toObject(),
                            &collection, &totalCount);

            place.setTotalContentCount(QPlaceContent::ReviewType, totalCount);
            place.setContent(QPlaceContent::ReviewType, collection);
        }
    }

    //if (object.contains(QLatin1String("related"))) {
    //    QJsonObject relatedObject = object.value(QLatin1String("related")).toObject();
    //}

    QPlaceAttribute provider;
    provider.setText(QLatin1String("nokia"));
    place.setExtendedAttribute(QPlaceAttribute::Provider, provider);

    place.setDetailsFetched(true);
    setPlace(place);

    m_reply->deleteLater();
    m_reply = 0;

    setFinished(true);
    emit finished();
}

void QPlaceDetailsReplyImpl::replyError(QNetworkReply::NetworkError error)
{
    switch (error) {
    case QNetworkReply::OperationCanceledError:
        setError(CancelError, "Request canceled.");
        break;
    default:
        setError(CommunicationError, "Network error.");
    }
}

QT_END_NAMESPACE
