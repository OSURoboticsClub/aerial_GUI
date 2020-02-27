TEMPLATE = app
TARGET = Hydra
QT += qml quick positioning positioning-private location network widgets

SOURCES += main.cpp

RESOURCES += qml.qrc

target.path = ~/Documents/School/Robotics_Club/QtProjects/HydraGUI/
INSTALLS += target

HEADERS += \
    chatclient.hpp

