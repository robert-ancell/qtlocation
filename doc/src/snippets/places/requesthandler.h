/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QDebug>
#include <QGeoBoundingCircle>
#include <QGeoServiceProvider>
#include <QPlaceDetailsReply>
#include <QPlaceIdReply>
#include <QPlaceManager>
#include <QPlaceSearchReply>
#include <QPlaceImage>
#include <QtCore/QMetaObject>

class RequestHandler : public QObject
{
public:
    void initializeManager() {
        //! [Initialize Manager]
        //The "provider name" is used to select a particular provider
        QGeoServiceProvider *provider = new QGeoServiceProvider("provider name");
        QPlaceManager *manager = provider->placeManager();
        //! [Initialize Manager]
    }

    void simpleSearch()
    {
        //! [Simple search]
        //1) Make an appropriate request
        QPlaceSearchRequest searchRequest;
        searchRequest.setSearchTerm("ice cream");
        searchRequest.setSearchArea(new QGeoBoundingCircle(QGeoCoordinate(12.34, 56.78)));

        //2) Use the manager to initiate a request and retreive a reply object
        QPlaceSearchReply * searchReply = manager->search(searchRequest);

        //3) Connect the reply object to a slot which is invoked upon operation completion
        connect(searchReply, SIGNAL(finished()), this, SLOT(processSearchReply()));
        //! [Simple search]
    }

    void search()
    {
        //! [Search for places cpp]

        //instantiate request and set parameters
        QPlaceSearchRequest searchRequest;
        searchRequest.setSearchTerm("ice cream");
        searchRequest.setSearchArea(new QGeoBoundingCircle(QGeoCoordinate(12.34, 56.78)));

        //send off a search request
        /*QPlaceSearchReply * */ searchReply = manager->search(searchRequest);

        //connect a slot to handle the reply
        connect(searchReply, SIGNAL(finished()), this, SLOT(handleSearchReply()));

        //! [Search for places cpp]
    }

    void searchPaging()
    {
        //! [Search paging]
        QPlaceSearchRequest searchRequest;
        searchRequest.setOffset(10);  //specify the index of the first result
        searchRequest.setLimit(15); //specify how many results are to be retrieved.
        //! [Search paging]
    }

    void didYouMean()
    {
        QPlaceSearchRequest searchRequest;
        //! [Corrections]
        searchRequest.setMaximumCorrections(2);
        //! [Corrections]


        //! [Corrections handling pt1]
        QPlaceSearchResult result;
        //! [Corrections handling pt1]

        //! [Corrections handling pt2]
        if (result.type() == QPlaceSearchResult::CorrectionResult)
            qDebug() << result.correction();
        //! [Corrections handling pt2]
    }

    void details()
    {
        QPlace place;
        //! [Details check]
        if (!place.detailsFetched()) {
            /*QPlaceDetailsReply * */ detailsReply = manager->getPlaceDetails(place.placeId());
            connect(detailsReply, SIGNAL(finished()), this, SLOT(handleDetailsReply()));
        }
        //! [Details check]
    }

    void images()
    {
        QPlace place;

        //! [Image request]
        QPlaceContentRequest request;
        request.setContentType(QPlaceContent::ImageType);
        request.setOffset(0);
        request.setLimit(5);
        /*QPlaceContentReply * */ contentReply = manager->getContent(place, request);
        connect(contentReply, SIGNAL(finished()), this, SLOT(handleImagesReply()));
        //! [Image request]
    }


    void suggestion()
    {
        //! [Suggestion request]
        QPlaceSearchRequest request;
        request.setSearchTerm("piz");
        request.setSearchArea(new QGeoBoundingCircle(QGeoCoordinate(12.34, 56.78)));
        /* QPlaceSearchSuggestion * */suggestionReply = manager->searchSuggestions(request);
        connect(suggestionReply, SIGNAL(finished()), this, SLOT(handleSuggestionReply()));
        //! [Suggestion request]
    }

    void recommendation()
    {
        //! [Recommendation]
        QPlaceSearchRequest request;
        request.setSearchArea(new QGeoBoundingCircle(QGeoCoordinate(12.34, 56.78)));

        /* QPlaceSearchReply * */ recommendationReply = manager->recommendations(place.placeId(), request);
        connect(recommendationReply, SIGNAL(finished()), this, SLOT(handleRecommendationReply()));
        //! [Recommendation]
    }

    void savePlace()
    {
        //! [Save place pt1]
        QPlace  place;
        place.setName( "Fred's Ice Cream Parlor" );

        QGeoLocation location;
        location.setCoordinate(QGeoCoordinate(12.34, 56.78));

        QGeoAddress address;
        address.setStreet("111 Nother Street");
        //! [Save place pt1]

        //! [Save place pt2]
        location.setAddress(address);
        place.setLocation(location);

        /* QPlaceIdReply * */savePlaceReply = manager->savePlace(place);
        connect(savePlaceReply, SIGNAL(finished()), this, SLOT(handleSavePlaceReply()));
        //! [Save place pt2]
    }

    void removePlace()
    {
        QPlace place;
        //! [Remove place]
        /* QPlaceIdReply * */removePlaceReply = manager->removePlace(place.placeId());
        connect(removePlaceReply, SIGNAL(finished()), this, SLOT(handleRemovePlaceReply()));
        //! [Remove place]
    }

    void initializeCategories()
    {
        //! [Initialize categories]
        /* QPlaceReply * */initCatReply = manager->initializeCategories();
        connect(initCatReply, SIGNAL(finished()), this, SLOT(handleInitCatReply()));
        //! [Initialize categories]
    }

    void saveCategory()
    {
        //! [Save category]
        QPlaceCategory fastFood;

        QPlaceCategory category;
        category.setName("pizza");
        /*QPlaceIdReply */ saveCategoryReply = manager->saveCategory(category);
        connect(saveCategoryReply, SIGNAL(finished()), this, SLOT(handleSaveCategoryReply()));

        //we could have saved a category as a child by supplying a parent id.
        saveCategoryReply = manager->saveCategory(category, fastFood.categoryId());
        //! [Save category]
    }

    void removeCategory()
    {
        QPlaceCategory category;
        //! [Remove category]
        /* QPlaceIdReply * */removeCategoryReply = manager->removeCategory(place.placeId());
        connect(removeCategoryReply, SIGNAL(finished()), this, SLOT(handleRemoveCategoryReply()));
        //! [Remove category]
    }

    void searchRequest() {
        QPlaceCategory diner;
        QPlaceCategory restaurant;

        //! [Search request]
        QPlaceSearchRequest searchRequest;
        searchRequest.setSearchTerm("Fast food"); //search term for what we are interested in

        //set a search center
        searchRequest.setSearchArea(new QGeoBoundingCircle(QGeoCoordinate(2.3, 48.87)));

        //set a distance hint as a relevancy hint.
        //closer places have greater weighting in the ranking of results.
        searchRequest.setRelevanceHint(QPlaceSearchRequest::DistanceHint);

        //use offset and limit to provide pagination.
        //this retrieves the next 5 items from the 10th index
        searchRequest.setOffset(9);
        searchRequest.setLimit(5);

        //provide some categories to narrow down search
        QList<QPlaceCategory> categories;
        categories << diner << restaurant;
        searchRequest.setCategories(categories);
        //! [Search request]
    }

    void content() {
        QPlace place;
        //! [Content request]
        QPlaceContentRequest request;
        request.setContentType(QPlaceContent::ImageType);
        request.setOffset(9);
        request.setLimit(5);

        QPlaceContentReply *contentReply = manager->getContent(place, request);
        //..connect signals..//

        //! [Content request]
    }

    void contentConversion()
    {
        //! [Content conversion]
        QPlaceImage image;
        image.setUrl(QUrl("www.example.com"));

        QPlaceContent content = image;

        QPlaceImage image2;
        image2 = content;
        qDebug() << image2.url(); //image2.url() == "www.example.com"
        //! [Content conversion]
    }

    void icon() {
        QPlace place;
        //! [icon]
        QUrl iconSourceUrl = place.icon().url(QSize(32,32), QPlaceIcon::Selected | QPlaceIcon::List);

        //A default icon may also be requested like so
        iconSourceUrl = place.icon().url();
        //! [icon]
    }

    void saveBetweenManagers()  {
        QPlaceSearchResult result;
        QPlaceIdReply *saveReply;
        //! [ Save to different manager]
        //result retrieved from a different manager
        QPlace place = result.place();

        //clear manager specific fields and
        //save to new manager
        place.setPlaceId(QString());
        place.setCategories(QList<QPlaceCategory>());
        place.setIcon(QPlaceIcon());
        place.setVisibility(QtLocation::UnspecifiedVisibility); //let the manager choose an appropriate default visibility
        saveReply = manager->savePlace(place);
        //! [ Save to different manager]
    }


public slots:
    // ![Simple search handler]
    //4) Have the slot appropriately process the results of the operation
    void processSearchReply() {
        if (searchReply->error() == QPlaceReply::NoError) {
            foreach (const QPlaceSearchResult &result, searchReply->results()) {
                if (result.type() == QPlaceSearchResult::PlaceResult)
                    qDebug() << "Name:" << result.place().name();
            }
        }

    //5) Discard the rely object when done.
        searchReply->deleteLater();
        searchReply = 0;
    }
    // ![Simple search handler]

    //! [Search for places handler cpp]
    void handleSearchReply() {
        if (searchReply->error() == QPlaceReply::NoError) {
            foreach (const QPlaceSearchResult &result, searchReply->results()) {
                if (result.type() == QPlaceSearchResult::PlaceResult) {
                    qDebug() << "Name: " << result.place().name();
                    qDebug() << "Coordinate " << result.place().location().coordinate().toString();
                    qDebug() << "Street: " << result.place().location().address().street();
                    qDebug() << "Distance: " << result.distance();
                }
            }
        }
        searchReply->deleteLater();  //discard reply
        searchReply = 0;
    }
    //! [Search for places handler cpp]

    //! [Details handler cpp]
    void handleDetailsReply() {
        QPlace place;
        if (detailsReply->error() == QPlaceReply::NoError)
            place = detailsReply->place();

        detailsReply->deleteLater(); //discard reply
        detailsReply = 0;
    }
    //! [Details handler cpp]

    //! [Image handler]
    void handleImagesReply() {
        if (contentReply->error() == QPlaceReply::NoError) {
            QMapIterator<int, QPlaceContent> iter(contentReply->content());
            while (iter.hasNext()) {
                qDebug() << "Index: " << iter.key();
                QPlaceImage image = iter.value();
                qDebug() << image.url();
                qDebug() << image.mimeType();
            }

            //alternatively if indexes are irrelevant
            foreach (const QPlaceImage &image, contentReply->content()) {
                qDebug() << image.url();
                qDebug() << image.mimeType();
            }

            //we can assign content to the place that it belongs to.
            //the place object serves as a container where we can retrieve
            //content that has already been fetched
            place.insertContent(contentReply->request().contentType(), contentReply->content());
            place.setTotalContentCount(contentReply->request().contentType(), contentReply->totalCount());
        }

        contentReply->deleteLater();
        contentReply = 0;
    }
    //! [Image handler]

    //! [Suggestion handler]
    void handleSuggestionReply() {
        if (suggestionReply->error() == QPlaceReply::NoError) {
            foreach (const QString &suggestion, suggestionReply->suggestions())
                qDebug() << suggestion;
        }

        suggestionReply->deleteLater(); //discard reply
        suggestionReply = 0;
    }

    //! [Suggestion handler]

    //! [Recommendation handler]
    void handleRecommendationReply() {
        if (recommendationReply->error() ==  QPlaceReply::NoError) {
            foreach (const QPlaceSearchResult &result, searchReply->results()) {
                if (result.type() == QPlaceSearchResult::PlaceResult)
                    qDebug() << result.place().name();
            }
        }
        recommendationReply->deleteLater(); //discard reply
        recommendationReply = 0;
    }


    //! [Recommendation handler]

    //! [Save place handler]
    void handleSavePlaceReply() {
        if (savePlaceReply->error() == QPlaceReply::NoError)
            qDebug() << savePlaceReply->id();

        savePlaceReply->deleteLater(); //discard reply
        savePlaceReply = 0;
    }
    //! [Save place handler]

    //! [Remove place handler]
    void handleRemovePlaceReply() {
        if (removePlaceReply->error() == QPlaceReply::NoError)
            qDebug() << "Removal of place identified by"
                     << removePlaceReply->id() << "was successful";

        removePlaceReply->deleteLater(); //discard reply
        removePlaceReply = 0;
    }
    //! [Remove place handler]

    //! [Initialize categories reply]
    void handleInitCatReply() {
        if (initCatReply->error() == QPlaceReply::NoError)
            qDebug() << "Categories initialized";
        else
            qDebug() << "Failed to initialize categories";

        initCatReply->deleteLater();
        initCatReply = 0;
    }
    //! [Initialize categories reply]

    void categories() {
        QPlaceCategory pizza;
        //! [Top level categories]
        QList<QPlaceCategory> topLevelCategories = manager->childCategories();
        foreach (const QPlaceCategory &category, topLevelCategories)
            qDebug() << category.name();
        //! [Top level categories]

        //! [Child categories]
        QList<QPlaceCategory> childCategories = manager->childCategories(pizza.categoryId());
        //! [Child categories]
    }

    //! [Save category handler]
    void handleSaveCategoryReply() {
        if (saveCategoryReply->error() == QPlaceReply::NoError) {
            qDebug() << "Saved category id =" << saveCategoryReply->id();
        }

        saveCategoryReply->deleteLater();
        saveCategoryReply = 0;
    }
    //! [Save category handler]

    //! [Remove category handler]
    void handleRemoveCategoryReply() {
        if (removeCategoryReply->error() == QPlaceReply::NoError)
            qDebug() << "Removal of category identified by"
                     << removeCategoryReply->id() << "was successful";

        removeCategoryReply->deleteLater(); //discard reply
        removeCategoryReply = 0;
    }
    //! [Remove category handler]


    //! [Content handler]
    void contentHandler() {
        if (contentReply->error() == QPlaceReply::NoError)  {
            place.insertContent(contentReply->request().contentType(),
                                contentReply->content());
        }
    }
    //! [Content handler]

    void phoneNumbers() {
        //! [Phone numbers]
        if (place.contactTypes().contains(QPlaceContactDetail::Phone)) {
            foreach (const QPlaceContactDetail &number, place.contactDetails(QPlaceContactDetail::Phone))
                qDebug() << number.label() << ":" << number.value();
        }
        //! [Phone numbers]
    }


    void openingHours() {
        //! [Opening hours]
        if (place.extendedAttributeTypes().contains(QPlaceAttribute::OpeningHours))
            qDebug() << place.extendedAttribute(QPlaceAttribute::OpeningHours).text();
        //! [Opening hours]
    }

QPlaceSearchReply *searchReply;
QPlaceSearchReply *recommendationReply;
QPlaceManager *manager;
QPlaceDetailsReply *detailsReply;
QPlaceContentReply *contentReply;
QPlaceSearchSuggestionReply *suggestionReply;
QPlaceIdReply *savePlaceReply;
QPlaceIdReply *removePlaceReply;
QPlaceIdReply *saveCategoryReply;
QPlaceIdReply *removeCategoryReply;
QPlaceReply *initCatReply;
QPlace place;
};

class ManagerEngine : public QObject
{
};

//! [Implement reply pt1]
class SearchReply : public QPlaceSearchReply
{
public:
    explicit SearchReply(ManagerEngine *engine)
        : QPlaceSearchReply(engine), m_engine(engine){}

    ~SearchReply();
    void setResults(const QList<QPlaceSearchResult> &results);
    void setRequest(const QPlaceSearchRequest &request);
//! [Implement reply pt1]

//! [Implement reply pt2]
    void triggerDone(QPlaceReply::Error error = QPlaceReply::NoError,
                     const QString &errorString = QString());

    ManagerEngine *m_engine;
};
//! [Implement reply pt2]

class SearchSuggestionReply : public QPlaceSearchSuggestionReply
{
public:
    void triggerDone(QPlaceReply::Error error = QPlaceReply::NoError,
                     const QString &errorString = QString());

    ManagerEngine *m_engine;

};

//! [Trigger done]
void SearchSuggestionReply::triggerDone(QPlaceReply::Error error,
                         const QString &errorString)
{
    if (error != QPlaceReply::NoError) {
        this->setError(error,errorString);
        QMetaObject::invokeMethod(m_engine, "error", Qt::QueuedConnection,
                                  Q_ARG(QPlaceReply *,this),
                                  Q_ARG(QPlaceReply::Error, error),
                                  Q_ARG(QString, errorString));
        QMetaObject::invokeMethod(this, "error", Qt::QueuedConnection,
                                  Q_ARG(QPlaceReply::Error, error),
                                  Q_ARG(QString, errorString));
    }

    this->setFinished(true);
    QMetaObject::invokeMethod(m_engine, "finished", Qt::QueuedConnection,
                              Q_ARG(QPlaceReply *,this));
    QMetaObject::invokeMethod(this, "finished", Qt::QueuedConnection);
}
//! [Trigger done]