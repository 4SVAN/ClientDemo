import QtQuick 2.15
import QtQuick.Window 2.2
import QtQuick.Controls 2.15
import Qt.labs.settings 1.0

import "./myComponents"
import FtpController 1.1

ApplicationWindow {
    id:root
    width: 430
    height: 300
    visible: true
    title: qsTr("登录")

    property double scaleWidth: homePage.width - 20

    Component.onCompleted: {
        FtpController.setUserInfo(settings.ftpUsername, settings.ftpPassword, settings.ftpHostPort)
    }

    Settings{
        id: settings
        property string mqttHostname: ""            // 平台端ip地址
        property var mqttHostPort: 9527               // port类型为quint16，默认为80
        property var ftpHostPort: 2121              // port类型为quint16，默认为21
        property string ftpUsername: "admin"        // ftp服务器用户名，默认为admin
        property string ftpPassword: ""             // ftp服务器用户密码，默认为空
    }

    LoginPage{
        id: loginPage
        visible: true
    }

    HomePage{
        id: homePage
        visible: false
    }

}
