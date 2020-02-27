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

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QObject>
#include <QTime>
#include <QBasicTimer>
#include <QDebug>
#include <QEasingCurve>
#include <QGeoCoordinate>
#include <QtPositioning/private/qwebmercator_p.h>
#include <QPointF>
//#include <QTcpSocket>
#include <QInputDialog>
#include <QtWidgets>

#include "chatclient.hpp"


#define ANIMATION_DURATION 500




//! [PlaneController1]
class PlaneController: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QGeoCoordinate position READ position WRITE setPosition NOTIFY positionChanged)
//! [PlaneController1]
    //! [C++Pilot1]
    Q_PROPERTY(QGeoCoordinate from READ from WRITE setFrom NOTIFY fromChanged)
    Q_PROPERTY(QGeoCoordinate to READ to WRITE setTo NOTIFY toChanged)
    //! [C++Pilot1]

public:
    PlaneController()
    {
        easingCurve.setType(QEasingCurve::InOutQuad);
        easingCurve.setPeriod(ANIMATION_DURATION);
    }
    ~PlaneController(){
        close(socketFD);
    }

    void setSocketFD(int socketFD){
        this->socketFD = socketFD;
    }

    void setFrom(const QGeoCoordinate& from)
    {
        fromCoordinate = from;
    }

    QGeoCoordinate from() const
    {
        return fromCoordinate;
    }

    void setTo(const QGeoCoordinate& to)
    {
        toCoordinate = to;
    }

    QGeoCoordinate to() const
    {
        return toCoordinate;
    }

    void setPosition(const QGeoCoordinate &c) {
        if (currentPosition == c)
            return;

        currentPosition = c;
        emit positionChanged();
    }

    QGeoCoordinate position() const
    {
        return currentPosition;
    }

    Q_INVOKABLE bool isFlying() const {
        return timer.isActive();
    }

//! [C++Pilot2]
public slots:
    void startFlight()
    {
        if (timer.isActive())
            return;

        startTime = QTime::currentTime();
        finishTime = startTime.addMSecs(ANIMATION_DURATION);

        timer.start(15, this);
        emit departed();
    }
//! [C++Pilot2]

    void swapDestinations() {
        if (currentPosition == toCoordinate) {
            // swap destinations
            toCoordinate = fromCoordinate;
            fromCoordinate = currentPosition;
        }
    }

    void updateToCoordinate(QGeoCoordinate newCoordinate){
        toCoordinate = newCoordinate;
    }
    void updateFromCoordinate(){
        fromCoordinate = toCoordinate;
    }
    void updateCurrentLocation() {
        char buffer[100];
        memset(buffer, '\0', sizeof(buffer));
        // request latitude from drone
        send(socketFD, "latitude", 8, 0);
        recv(socketFD, buffer, sizeof(buffer), 0);
        double latitude = atof(buffer);
        memset(buffer, '\0', sizeof(buffer));
        // request longitude from drone
        send(socketFD, "longitude", 9, 0);
        recv(socketFD, buffer, sizeof(buffer), 0);
        double longitude = atof(buffer);
        QGeoCoordinate current_location(latitude, longitude);
        updateToCoordinate(current_location);
//        updatePosition();
    }

signals:
    void positionChanged();
    void arrived();
    void departed();
    void toChanged();
    void fromChanged();

protected:
    void timerEvent(QTimerEvent *event) override
    {
        if (!event)
            return;

        if (event->timerId() == timer.timerId())
            updatePosition();
        else
            QObject::timerEvent(event);
    }

private:
    //! [C++Pilot3]
    void updatePosition()
    {
        // simple progress animation
        qreal progress;
        QTime current = QTime::currentTime();
        if (current >= finishTime) {
            progress = 1.0;
            timer.stop();
        } else {
            progress = ((qreal)startTime.msecsTo(current) / ANIMATION_DURATION);
        }

        setPosition(QWebMercator::coordinateInterpolation(
                          fromCoordinate, toCoordinate, easingCurve.valueForProgress(progress)));

        if (!timer.isActive())
            emit arrived();
    }


    //! [C++Pilot3]


private:
    QGeoCoordinate currentPosition;
    QGeoCoordinate fromCoordinate, toCoordinate;
    QBasicTimer timer;
    QTime startTime, finishTime;
    QEasingCurve easingCurve;
    int socketFD;

//! [PlaneController2]
    // ...
};
//! [PlaneController2]

//! [PlaneControllerMain]
int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);


//    PlaneController oslo2berlin;

    PlaneController HydraPlane;

    // Connecting to server
    int socketFD, portNumber, charsWritten, charsRead;
    struct sockaddr_in serverAddress;
    char buffer[BUFFER_SIZE];
    char buffer2[BUFFER_SIZE];
    memset(buffer, '\0', sizeof(buffer));
    memset(buffer2, '\0', sizeof(buffer2));
    char hostname[] = "localhost";
    char portNumberString[] = "5000";
    char s[INET6_ADDRSTRLEN];
    int status;
    struct addrinfo hints;
    struct addrinfo *servinfo, *p;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // fill in the IP for me
    if ((status = getaddrinfo(hostname, portNumberString, &hints, &servinfo)) != 0){
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }
    // loop through all the results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        // creates socket (taken from Beej's guide)
        if ((socketFD = socket(p->ai_family, p->ai_socktype,
                               p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }
        // establishes connection to server (taken form Beej's guide)
        if (connect(socketFD, p->ai_addr, p->ai_addrlen) == -1) {
            close(socketFD);
            perror("client: connect");
            continue;
        }
        break;
    }
    bool connection_status = true;
    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        connection_status = false;
//        return 2;
    }
    if (connection_status == true){
        // gets the IP address of the hostname and prints it
        inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
        printf("client: connecting to %s\n", s);
        freeaddrinfo(servinfo);
        send(socketFD, "latitude", 8, 0);
        recv(socketFD, buffer, sizeof(buffer), 0);
        printf("%s", buffer);
    }
    HydraPlane.setSocketFD(socketFD);



    QQmlApplicationEngine engine;
//    engine.rootContext()->setContextProperty("oslo2Berlin", &oslo2berlin);
    engine.rootContext()->setContextProperty("HydraPlane", &HydraPlane);
    engine.load(QUrl(QStringLiteral("qrc:/planespotter.qml")));

    return app.exec();
}
//! [PlaneControllerMain]

#include "main.moc"
