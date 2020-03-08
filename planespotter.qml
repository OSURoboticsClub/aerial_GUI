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
import QtQuick.Controls 1.6
import QtQuick.Extras 1.4
import QtWebEngine.Controls1Delegates 1.0
import QtQuick.Layouts 1.3
import Qt.labs.calendar 1.0
import QtGraphicalEffects 1.0
import QtQuick.Controls.Styles.Desktop 1.0
import QtQuick.Dialogs.qml 1.0
import QtTest 1.2
import QtQuick.Layouts 1.0

ApplicationWindow {
    id: window
    width: 1000
    height: 500
    visible: true

    property variant topLeftEurope: QtPositioning.coordinate(60.5, 0.0)
    property variant bottomRightEurope: QtPositioning.coordinate(51.0, 14.0)
    property variant topLeftCorvallis: QtPositioning.coordinate(44.599480, -123.326150)
    property variant bottomRightCorvallis: QtPositioning.coordinate(44.552963, -123.221276)
    property variant viewOfCorvallis:
        QtPositioning.rectangle(topLeftCorvallis, bottomRightCorvallis)
    property variant viewOfEurope:
        QtPositioning.rectangle(topLeftEurope, bottomRightEurope)

    property variant berlin: QtPositioning.coordinate(52.5175, 13.384)
    property variant corvallis: QtPositioning.coordinate(44.5646, -123.2620)
    property variant albany: QtPositioning.coordinate(44.6365, -123.1059)
    property variant oslo: QtPositioning.coordinate(59.9154, 10.7425)
    property variant london: QtPositioning.coordinate(51.5, 0.1275)
    property variant destinationCoordinate: QtPositioning.coordinate(44.34, -123.17)

    Rectangle {
        id: rightRectangle
        color: "#2e3436"
        anchors.top: parent.top
        anchors.topMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 916
        anchors.right: parent.right
        anchors.rightMargin: 0

        Text {
            id: element4
            x: 4
            y: 259
            width: 80
            height: 16
            color: "#ffffff"
            text: qsTr("Set Altitude")
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 225
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 14
        }

        Button {
            id: resetAltitudeButton
            x: 8
            y: 468
            width: 68
            height: 27
            text: qsTr("Reset")
        }

        Text {
            id: altitudeDisplay
            x: 29
            width: 81
            height: 22
            color: "#ffffff"
            fontSizeMode: Text.FixedSize
            anchors.right: parent.right
            anchors.rightMargin: -1
            anchors.top: parent.top
            anchors.topMargin: 18
            verticalAlignment: Text.AlignTop
            textFormat: Text.AutoText
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            //            property string altitude: "Altitude: " + HydraPlane.altitude
            font.pixelSize: 14
        }


        Text {
            id: element1
            x: 35
            width: 67
            height: 19
            color: "#ffffff"
            text: qsTr("Altitude")
            anchors.right: parent.right
            anchors.rightMargin: 8
            anchors.top: parent.top
            anchors.topMargin: 0
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 14
        }


        Text {
            id: element2
            x: 41
            width: 54
            height: 23
            color: "#ffffff"
            text: qsTr("Voltage")
            anchors.top: parent.top
            anchors.topMargin: 46
            anchors.right: parent.right
            anchors.rightMargin: 14
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 14
        }

        Gauge {
            id: voltageGauge
            x: 47
            width: 54
            height: 185
            value: 13.2
            anchors.top: parent.top
            anchors.topMargin: 67
            anchors.right: parent.right
            anchors.rightMargin: 8
            minimumValue: 9.5
            maximumValue: 14
            tickmarkStepSize: 0.5
        }

        Button {
            id: setAltitudeButton
            x: 8
            y: 435
            width: 68
            height: 28
            text: qsTr("Set")
        }

        Slider {
            id: setAltitudeBar
            x: 33
            y: 303
            width: 22
            height: 89
            orientation: Qt.Vertical
        }

        TextField {
            id: enterAltitudeField
            x: 8
            y: 403
            width: 68
            height: 26
            placeholderText: qsTr("Text Field")
        }

        Text {
            id: setAltitudeValue
            x: 8
            y: 281
            width: 68
            height: 16
            color: "#ffffff"
            text: qsTr("0m")
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 14
        }



    }
    Rectangle {
        id: mapRectangle
        color: "#ffffff"
        anchors.top: parent.top
        anchors.topMargin: 42
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 58
        anchors.right: parent.right
        anchors.rightMargin: 84
        anchors.left: parent.left
        anchors.leftMargin: 0

        Map {
            id: mapOfEurope
            anchors.rightMargin: 0
            anchors.bottomMargin: 0
            anchors.centerIn: parent;
            anchors.fill: parent
            plugin: Plugin {
                name: "esri" // "mapboxgl", "esri", osm...
            }
            MapQuickItem {
                id: marker
                anchorPoint.x: imageMarker.width/2
                anchorPoint.y: imageMarker.height
                coordinate: corvallis

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

            //! [CppPlane1]
            Plane {
                id: cppPlane
                pilotName: "Hydra"
                coordinate: HydraPlane.position

                MouseArea {
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

            visibleRegion: viewOfCorvallis
        }
    }

    Timer {
        interval: 1000; running: true; repeat: true;
        onTriggered: {
            console.log(HydraPlane.altitude())
            altitudeDisplay.text = HydraPlane.altitude() + " meters"
            console.log("battery:")
            console.log(HydraPlane.batteryVoltage())
            voltageGauge.value = HydraPlane.batteryVoltage()
            if (HydraPlane.isArmed() == 0){
                statusIndicator.color = "red"
            }
            else if(HydraPlane.isArmed() == 1){
                statusIndicator.color = "green"
            }

        }
    }

    Rectangle {
        id: bottomRectangle
        y: 443
        height: 57
        color: "#2e3436"
        anchors.right: parent.right
        anchors.rightMargin: 84
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 0

        Button {
            id: takeoffButton
            x: 17
            y: 15
            text: qsTr("Takeoff")
            anchors.verticalCenter: parent.verticalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 14
        }

        Button {
            id: landButton
            x: 115
            y: 15
            text: qsTr("Land")
            anchors.verticalCenter: parent.verticalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 14
        }

        Button {
            id: armButton
            x: 212
            y: 15
            text: qsTr("Arm")
            anchors.verticalCenter: parent.verticalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 14
            onClicked: HydraPlane.arm()
        }

        Button {
            id: disarmButton
            x: 310
            y: 15
            text: qsTr("Disarm")
            anchors.verticalCenter: parent.verticalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 14
            onClicked: HydraPlane.disarm()
        }

        Button {
            id: killButton
            x: 404
            y: 15
            text: qsTr("Kill")
            onClicked: HydraPlane.kill()

        }

        Button {
            id: returnHomeButton
            x: 500
            y: 15
            text: qsTr("Return Home")
        }
    }

    Rectangle {
        id: topRectangle
        height: 41
        color: "#2e3436"
        anchors.right: parent.right
        anchors.rightMargin: 84
        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.top: parent.top
        anchors.topMargin: 0

        StatusIndicator {
            id: statusIndicator
            x: 70
            y: 0
            color: "#008000"
            active: true
        }

        Text {
            id: element
            x: 0
            y: 12
            width: 75
            height: 21
            color: "#ffffff"
            text: qsTr("ARMED")
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 14
        }
    }



}



/*##^##
Designer {
    D{i:32;anchors_x:105}D{i:31;anchors_height:54;anchors_width:916;anchors_x:0;anchors_y:0}
}
##^##*/
