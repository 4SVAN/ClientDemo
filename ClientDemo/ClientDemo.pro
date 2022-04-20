QT += core widgets gui network quick mqtt multimedia axcontainer ftp

CONFIG += c++11

include(3rdparty/xlsx/qtxlsx.pri)

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        src/clientManager.cpp \
        src/ftpClient.cpp \
        src/ftpcontroller.cpp \
        src/listElements.cpp \
        src/log.cpp \
        src/main.cpp \
        src/modelmanager.cpp \
        src/mqtt.cpp \
        src/mymodels.cpp \
        src/tcpClient.cpp

RESOURCES += \
        ui/qml.qrc \
        ui/res.qrc


# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    src/clientManager.h \
    src/ftpClient.h \
    src/ftpcontroller.h \
    src/listElements.h \
    src/log.h \
    src/modelmanager.h \
    src/mqtt.h \
    src/mymodels.h \
    src/singleton.h \
    src/tcpClient.h \
