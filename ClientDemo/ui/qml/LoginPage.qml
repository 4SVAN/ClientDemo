import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.2
import QtQuick.Layouts 1.2

import Mqtt 1.1
import TcpClient 1.1
import "./myComponents"
import "./myFunction.js" as MyFunction

Rectangle{
    width: 430
    height: 300
    anchors.centerIn: parent

    function handlePage(){
        root.close()
        homePage.showNormal()
    }

    function printError(mqttState){
        errorText.text = " 发生错误 : " + mqttState
        errorText.color = "red"
    }

    MouseArea {
        anchors.fill: login
        onClicked: {
            forceActiveFocus()
//            console.log(root.width + " " + root.height)
//            console.log(login.width + " " + login.height)
        }
    }

    ColumnLayout{
        id: login
        anchors.fill: parent
        anchors.leftMargin: 50
        anchors.rightMargin: 50
        anchors.topMargin: 30
        anchors.bottomMargin: 20
        spacing: 20
        focus: true
        Keys.enabled: true

        Keys.onEnterPressed: {
            console.log("onEnterPressed")
            connectMQTTButton.clicked()
        }
        Keys.onReturnPressed: {
            console.log("onRetrunPressed")
            connectMQTTButton.clicked()
        }
        Keys.onEscapePressed: {
            console.log("onEscapePressed")
            forceActiveFocus()
        }

        Component.onCompleted: {
            // 关联信号槽
            Mqtt.connected.connect(handlePage)
            Mqtt.errorOccured.connect(printError)
        }

        RowLayout{
            id: loginTitle
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 180

            Label {
                text: "Login"
                    font.family: "Arial"
                font.pixelSize: 18
                font.weight: Font.Bold

                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }
        }

        RowLayout{
            id: mqttHostname
            Layout.preferredWidth: 200
//            width: 200
            spacing: 30

            Label {
                text: "平台地址 :"
                Layout.fillWidth: true
                font.family: "Arial"
                font.pixelSize: 14
                font.weight: Font.DemiBold
                horizontalAlignment: Text.AlignRight

                enabled: Mqtt.state == Mqtt.Disconnected
            }

            TextField {
                id: hostnameField
                width: parent.width * 0.6
                text: settings.mqttHostname // 准备换成settings
                placeholderText: "Enter school Platform IP"
                selectByMouse: true
                selectedTextColor: "white"
                enabled: Mqtt.state == Mqtt.Disconnected
            }
        }

        RowLayout{
            id: mqttHostPort
            Layout.preferredWidth: 200
            spacing: 30

            Label {
                text: "端口 :"
                Layout.fillWidth: true
                font.family: "Arial"
                font.pixelSize: 14
                font.weight: Font.DemiBold
                horizontalAlignment: Text.AlignRight

                enabled: Mqtt.state == Mqtt.Disconnected
            }

            TextField{
                id:hostPortField
                width: parent.width * 0.6
                enabled: Mqtt.state == Mqtt.Disconnected
                text: settings.mqttHostPort
                placeholderText: "Enter school Platform Port"
                selectByMouse: true
                selectedTextColor: "white"
            }
        }

        ColumnLayout{
            spacing: 5

            RowLayout{
                id: errorMessage
                width: 100

                Text{
                    id: errorText
                    width: 50
                    horizontalAlignment: Text.AlignRight
                    font.family: "Arial"
                    font.pixelSize: 12
                    font.weight: Font.Light
                }
            }

            ClickButton {
                id: connectMQTTButton
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredHeight: 40
                Layout.preferredWidth: mqttHostPort.width
                text: "连接"

                onClicked: {
                    errorText.text = ""
                    errorText.color = "white"

                    // 保存信息
                    Mqtt.schoolPlatformIP = hostnameField.text
                    Mqtt.schoolPlatformPort = hostPortField.text
                    settings.mqttHostname = hostnameField.text
                    settings.mqttHostPort = hostPortField.text

                    console.log("正在尝试建立连接...")
                    Mqtt.connectToHost()


                }
            }
        }

    }

}

