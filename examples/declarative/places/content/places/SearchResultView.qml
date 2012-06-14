/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the examples of the Qt Toolkit.
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
import QtLocation 5.0
import QtLocation.examples 5.0

Item {
    id: root

    clip: true

    function showSearchResults() {
        view.currentIndex = 0;
        placeContentList.source = "";
        placeContentList.place = null;
    }

    function showPlaceDetails(data) {
        placeDetails.place = data.place;
        placeDetails.distance = data.distance;
        view.currentIndex = 1;
    }

    ListView {
        id: view

        orientation: ListView.Horizontal
        interactive: false
        anchors.fill: parent
        snapMode: ListView.SnapOneItem

        model: VisualItemModel {
            Item {
                // search results (page 0)

                width: root.width
                height: root.height

                Connections {
                    target: placeSearchModel
                    onStatusChanged: searchView.visible = true
                }
                Connections {
                    target: recommendationModel
                    onStatusChanged: searchView.visible = false
                }

                //! [PlaceSearchModel place list]
                ListView {
                    id: searchView

                    anchors.fill: parent

                    model: placeSearchModel
                    delegate: SearchResultDelegate {
                        onDisplayPlaceDetails: showPlaceDetails(data)
                        onSearchFor: placeSearchModel.searchForText(query);
                    }

                    footer: Item {
                        width: searchView.width
                        height: childrenRect.height

                        Button {
                            text: qsTr("Previous")
                            onClicked: placeSearchModel.previousPage()

                            anchors.left: parent.left
                        }

                        Button {
                            text: qsTr("Clear")
                            onClicked: placeSearchModel.reset()

                            anchors.horizontalCenter: parent.horizontalCenter
                        }

                        Button {
                            text: qsTr("Next")
                            onClicked: placeSearchModel.nextPage()

                            anchors.right: parent.right
                        }
                    }
                }
                //! [PlaceSearchModel place list]

                //! [PlaceRecommendationModel place list]
                ListView {
                    id: similarView

                    anchors.fill: parent

                    spacing: 5

                    visible: !searchView.visible
                    model: recommendationModel
                    delegate: SearchResultDelegate {
                        onDisplayPlaceDetails: showPlaceDetails(data)
                    }
                }
                //! [PlaceRecommendationModel place list]
            }
            Item {
                // place details (page 1)
                width: root.width
                height: root.height

                IconButton {
                    id: placeBackButton
                    anchors.left:  parent.left

                    source: "../../resources/left.png"
                    pressedSource: "../../resources/left_pressed.png"

                    onClicked: showSearchResults()
                }

                PlaceDelegate {
                    id: placeDetails

                    anchors.top: placeBackButton.bottom
                    anchors.bottom: parent.bottom
                    width: parent.width
                    anchors.margins: 10

                    onShowEditorials: {
                        placeContentList.source = "";
                        placeContentList.place = place;
                        placeContentList.source = "PlaceEditorials.qml";
                        view.currentIndex = 2;
                    }

                    onShowReviews: {
                        placeContentList.source = "";
                        placeContentList.place = place;
                        placeContentList.source = "PlaceReviews.qml";
                        view.currentIndex = 2;
                    }

                    onShowImages: {
                        placeContentList.source = "";
                        placeContentList.place = place;
                        placeContentList.source = "PlaceImages.qml";
                        view.currentIndex = 2;
                    }

                    //! [PlaceRecommendationModel search]
                    onSearchForSimilar: {
                        placeContentList.source = "";
                        recommendationModel.placeId = place.placeId;
                        recommendationModel.update();
                        view.currentIndex = 0;
                    }
                    //! [PlaceRecommendationModel search]

                    onEditPlace: {
                        editPlaceDialog.prepareDialog(place);
                        page.state = "EditPlace"
                    }

                    onDeletePlace:  {
                        place.remove();
                        showSearchResults();
                    }
                }
            }
            Item {
                // content list (page 2)
                width: root.width
                height: root.height

                IconButton {
                    id: contentListBackButton

                    source: "../../resources/left.png"
                    pressedSource: "../../resources/left_pressed.png"

                    onClicked: view.currentIndex = 1
                }

                Loader {
                    id: placeContentList

                    property Place place

                    anchors.top: contentListBackButton.bottom
                    anchors.bottom: parent.bottom
                    width: parent.width
                    anchors.margins: 10

                    onStatusChanged: {
                        if (status === Loader.Null)
                            place = null;
                    }
                }
            }
            Item {
                // content (page 3)
                width: root.width
                height: root.height

                IconButton {
                    id: contentBackButton

                    source: "../../resources/left.png"
                    pressedSource: "../../resources/left_pressed.png"

                    onClicked: {
                        view.currentIndex = 2;
                        placeContent.source = "";
                        placeContent.data = null;
                    }
                }

                Loader {
                    id: placeContent

                    anchors.top: contentBackButton.bottom
                    anchors.bottom: parent.bottom
                    width: parent.width
                    anchors.margins: 10

                    onLoaded: view.currentIndex = 3

                    property variant data
                }
            }
        }
    }
}
