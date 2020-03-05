/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
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
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
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

import QtQuick 2.4
import QtQuick.Window 2.2
import QtPositioning 5.5
import QtLocation 5.6

Window {
    width: 700
    height: 500
    visible: true

    property variant topLeftEurope: QtPositioning.coordinate(60.5, 0.0)
    property variant bottomRightEurope: QtPositioning.coordinate(51.0, 14.0)
    property variant viewOfEurope:
            QtPositioning.rectangle(topLeftEurope, bottomRightEurope)

    property variant berlin: QtPositioning.coordinate(52.5175, 13.384)
    property variant corvallis: QtPositioning.coordinate(44.5646, -123.2620)
    property variant albany: QtPositioning.coordinate(44.6365, -123.1059)
    property variant oslo: QtPositioning.coordinate(59.9154, 10.7425)
    property variant london: QtPositioning.coordinate(51.5, 0.1275)
    property variant destinationCoordinate: QtPositioning.coordinate(44.34, -123.17)


    Map {
        id: mapOfEurope
        anchors.centerIn: parent;
        anchors.fill: parent
        plugin: Plugin {
            name: "esri" // "mapboxgl", "esri", osm...
        }
        MapQuickItem {
            id: marker
            anchorPoint.x: imageMarker.width/2
            anchorPoint.y: imageMarker.height
            coordinate: destinationCoordinate

            sourceItem: Image {
                id: imageMarker
                source: "marker.png"
            }
        }
        MouseArea {
            anchors.fill: parent
            onPressed: {
                //marker.coordinate = mapOfEurope.toCoordinate(Qt.point(mouse.x, mouse.y))
                destinationCoordinate = mapOfEurope.toCoordinate(Qt.point(mouse.x, mouse.y))
                HydraPlane.updateToCoordinate(mapOfEurope.toCoordinate((Qt.point(mouse.x, mouse.y))))
                cppPlane.showMessage(qsTr("Setting destination to " + destinationCoordinate))
                console.log(destinationCoordinate)
            }
        }

//        Plane {
//            id: qmlPlane
//            pilotName: "QML"
//            coordinate: oslo2Berlin.position

//            SequentialAnimation {
//                id: qmlPlaneAnimation
//                property real rotationDirection : 0;
//                NumberAnimation {
//                    target: qmlPlane; property: "bearing"; duration: 1000
//                    easing.type: Easing.InOutQuad
//                    to: qmlPlaneAnimation.rotationDirection
//                }
//                //! [QmlPlane1]
//                CoordinateAnimation {
//                    id: coordinateAnimation; duration: 5000
//                    target: oslo2Berlin; property: "position"
//                    easing.type: Easing.InOutQuad
//                }
//                //! [QmlPlane1]

//                onStopped: {
////                    if (coordinateAnimation.to === berlin)
//                        qmlPlane.showMessage(qsTr("Arriving!"))
////                    else if (coordinateAnimation.to === oslo)
////                        qmlPlane.showMessage(qsTr("!"))
//                }
//                onStarted: {
////                    if (coordinateAnimation.from === oslo)
//                        qmlPlane.showMessage(qsTr("Departing!"))
////                    else if (coordinateAnimation.from === berlin)
////                        qmlPlane.showMessage(qsTr("See you Berlin!"))
//                }
//            }



            //! [QmlPlane2]
//            MouseArea {
//                anchors.fill: parent
//                onClicked: {
//                    if (qmlPlaneAnimation.running) {
//                        console.log("Plane still in the air.");
//                        return;
//                    }

//                    if (oslo2Berlin.position === berlin) {
//                        coordinateAnimation.from = berlin;
//                        coordinateAnimation.to = oslo;
//                    } else if (oslo2Berlin.position === oslo) {
//                        coordinateAnimation.from = oslo;
//                        coordinateAnimation.to = berlin;
//                    }

//                    qmlPlaneAnimation.rotationDirection = oslo2Berlin.position.azimuthTo(coordinateAnimation.to)
//                    qmlPlaneAnimation.start()
//                }
//            }
//            //! [QmlPlane2]
//            Component.onCompleted: {
//                oslo2Berlin.position = oslo;
//            }
//        }

        //! [CppPlane1]
        Plane {
            id: cppPlane
            pilotName: "Hydra"
            coordinate: HydraPlane.position

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (cppPlaneAnimation.running || HydraPlane.isFlying()) {
                        console.log("Hydra still in the air");
                        return;
                    }

//                    HydraPlane.swapDestinations();
                    cppPlaneAnimation.rotationDirection = HydraPlane.position.azimuthTo(HydraPlane.to)
                    cppPlaneAnimation.start();
                    cppPlane.departed();
                }
            }
        //! [CppPlane1]
            //! [CppPlane3]
            SequentialAnimation {
                id: cppPlaneAnimation
                property real rotationDirection : 0;
                NumberAnimation {
                    target: cppPlane; property: "bearing"; duration: 1000
                    easing.type: Easing.InOutQuad
                    to: cppPlaneAnimation.rotationDirection
                }
                ScriptAction { script: HydraPlane.startFlight() }
            }
            //! [CppPlane3]

            Component.onCompleted: {
                // happens only on the first animation
//                console.log("component.onCompleted")
                HydraPlane.position = corvallis;
//                HydraPlane.position = berlin;
                HydraPlane.to = albany;
                HydraPlane.from = corvallis;
                HydraPlane.arrived.connect(arrived)
            }

            function arrived(){
//                if (HydraPlane.to === berlin)
//                cppPlane.showMessage(qsTr("Arriving!"))
                HydraPlane.updateFromCoordinate()

//                else if (HydraPlane.to ===destinationCoordinate)
//                    cppPlane.showMessage(qsTr("Hello destinationCoordinate!"))
            }

            function departed(){
//                if (HydraPlane.from === berlin)
//                    cppPlane.showMessage(qsTr("See you Berlin!"))
//                else if (HydraPlane.from ===destinationCoordinate)
                    cppPlane.showMessage(qsTr("Departing!"))
            }
        //! [CppPlane2]
        }


        //! [CppPlane2]

        visibleRegion: viewOfEurope
    }

    Rectangle {
        id: infoBox
        anchors.centerIn: parent
        color: "white"
        border.width: 1
        width: text.width * 1.3
        height: text.height * 1.3
        radius: 5
        Text {
            id: text
            anchors.centerIn: parent
            text: qsTr("Hit the plane to start the flight!")
        }

        Timer {
            interval: 5000; running: true; repeat: false;
            onTriggered: fadeOut.start()
        }
//        Timer{
//            interval: 1500; running: true; repeat: true;
//            onTriggered: HydraPlane.updateCurrentLocation()
//        }


        NumberAnimation {
            id: fadeOut; target: infoBox;
            property: "opacity";
            to: 0.0;
            duration: 200
            easing.type: Easing.InOutQuad
        }
    }
}
