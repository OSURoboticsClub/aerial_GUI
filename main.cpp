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
#include <iostream>
#include <iomanip>
#include "chatclient.hpp"


#define ANIMATION_DURATION 500

class Worker : public QObject
{
    Q_OBJECT

public slots:
    void doWork(const int socketFD) {
        sleep(1);
        QGeoCoordinate newCoord;
        // ***************** Getting Current Position **********************
        qDebug() << "Thread: requesting current location from server";
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

        // ****************** Getting Battery Status ***********************
        // request altitude from drone
        send(socketFD, "altitude", 8, 0);
        recv(socketFD, buffer, sizeof(buffer), 0);
        // cuts string at 5th char
        buffer[5] = '\0';

        // ****************** Getting Altitude *****************************
//        std::cout << "Thread: altitude: %" << std::endl;
        double altitude = atof(buffer);
        send(socketFD, "battery", 7, 0);
        memset(buffer, '\0', sizeof(buffer));
        recv(socketFD, buffer, sizeof(buffer), 0);
        double battery = atof(buffer);

        // ****************** Getting Armed/Disarmed Status *****************
        send(socketFD, "is_armed", 8, 0);
        memset(buffer, '\0', sizeof(buffer));
        recv(socketFD, buffer, sizeof(buffer), 0);

        int is_armed;
        qDebug() << buffer;
        if (buffer[0] == 'F'){
            qDebug() << "disarmed";
            is_armed = 0;
        }
        else {
            is_armed = 1;
        }
        qDebug() << "is armed:";
        qDebug() << is_armed;


        qDebug() << "Thread: sending location to master";

        emit resultReady(current_location, altitude, battery, is_armed);

    }
//    void setSocketFD(int socketFD){
//        this->socketFD = socketFD;
//    }
private:
//    int socketFD;

signals:
    void resultReady(const QGeoCoordinate &result, const double &alt, const double &batt, const int &is_armed);
};


//! [PlaneController1]
class PlaneController: public QObject
{
    Q_OBJECT
    QThread workerThread;
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
        this->currentAltitude = 0;
        this->batteryVolt = 13.2;
        this->is_armed = false;

    }
    ~PlaneController(){
        workerThread.quit();
        workerThread.wait();
        close(socketFD);
    }

    void start_client_thread(){
        this->worker = new Worker;
        worker->moveToThread(&workerThread);
        connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
        connect(this, &PlaneController::operate, worker, &Worker::doWork);
        // remember update position has an animation gotta get rid of
        connect(worker, &Worker::resultReady, this, &PlaneController::updatedCoordinatesSlot);
        workerThread.start();
    }

    void setSocketFD(int socketFD){
        this->socketFD = socketFD;
//        this->worker->setSocketFD(socketFD);

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
    Q_INVOKABLE double altitude() const{
        return currentAltitude;
    }

    Q_INVOKABLE double batteryVoltage() const{
        return batteryVolt;
    }

    Q_INVOKABLE bool isFlying() const {
        return timer.isActive();
    }
    Q_INVOKABLE void arm(){
        send(socketFD, "arm", 3, 0);
    }
    Q_INVOKABLE void disarm(){
        send(socketFD, "disarm", 6, 0);
    }
    Q_INVOKABLE void kill(){
        send(socketFD, "kill", 4, 0);
    }
    Q_INVOKABLE bool isArmed() const{
        return is_armed;
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
    void updatedCoordinatesSlot(const QGeoCoordinate newCoord, double newAltitude, double newBattery, int is_armed){
        updateIsArmed(is_armed);
        updateToCoordinate(newCoord);
        updatePosition();
        updateAltitude(newAltitude);
        updateBattery(newBattery);
        operate(socketFD);
    }
//! [C++Pilot2]

    void swapDestinations() {
        if (currentPosition == toCoordinate) {
            // swap destinations
            toCoordinate = fromCoordinate;
            fromCoordinate = currentPosition;
        }
    }

    void updateIsArmed(int is_armed){
        if (is_armed == false){
            this->is_armed = false;
        }
        else {
            this->is_armed = true;
        }
    }

    void updateToCoordinate(QGeoCoordinate newCoordinate){
        toCoordinate = newCoordinate;
    }
    void updateFromCoordinate(){
        fromCoordinate = toCoordinate;
    }
    void updateAltitude(double altitude){
        currentAltitude = altitude;
    }
    void updateBattery(double battery){
        batteryVolt = battery;
    }
    void updateCurrentLocation() {

    }

signals:
    void positionChanged();
    void arrived();
    void departed();
    void toChanged();
    void fromChanged();

    // start thread signal
    void operate(int socketFD);

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
    Worker *worker;
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
    int is_armed;
    double currentAltitude;
    double batteryVolt;
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
    int socketFD;
//    struct sockaddr_in serverAddress;
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
        HydraPlane.start_client_thread();
        HydraPlane.setSocketFD(socketFD);
        HydraPlane.operate(socketFD);
    }




    QQmlApplicationEngine engine;
//    engine.rootContext()->setContextProperty("oslo2Berlin", &oslo2berlin);
    engine.rootContext()->setContextProperty("HydraPlane", &HydraPlane);
    engine.load(QUrl(QStringLiteral("qrc:/planespotter.qml")));

    return app.exec();
}
//! [PlaneControllerMain]

#include "main.moc"
