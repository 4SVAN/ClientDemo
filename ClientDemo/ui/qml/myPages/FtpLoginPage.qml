import QtQuick 2.9
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.2

import "../myComponents"
import Mqtt 1.1
import FtpController 1.1

Popup{
    id: root
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside  // "NoAutoClose"——不点就不退

    function printError(ftpState){
        if(ftpState === FtpController.Connected){
            errorText.color = "black"
            errorText.text = "已实现与主机的连接"
        }
        else if(ftpState === FtpController.LoggedIn){
            FtpModel.clear()
            root.close()
            ftpDownloadPage.open()
        }
        else if(ftpState === FtpController.Unconnected)
        {
            errorText.color = "red"
            errorText.text = "没有连接到主机或已经断开连接"
        }
        else{
            console.log(ftpState)
        }
    }

    onVisibleChanged: {
        errorText.text = ""
        errorText.color = "white"
        if(Mqtt.getSeatHostname(messageView.currentIndex) !== " ")
            hostnameField.text = Mqtt.getSeatHostname(messageView.currentIndex)
        else
            hostnameField.clear()
    }

    MouseArea {
        anchors.fill: login
        onClicked: {
            forceActiveFocus()
        }
    }

    ColumnLayout{
        id: login
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10
        Keys.enabled: true

        Keys.onEnterPressed: {
            console.log("onEnterPressed")
            connectFtpButton.clicked()
        }
        Keys.onReturnPressed: {
            console.log("onRetrunPressed")
            connectFtpButton.clicked()
        }
        Keys.onEscapePressed: {
            console.log("onEscapePressed")
            forceActiveFocus()
        }

        RowLayout{
            id: ftpLoginTitle
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 180

            Label {
                text: "FTP Server Login"
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
            id: ftpUserInputField
            Layout.alignment: Qt.AlignHCenter
            width: 200
            spacing: 10

            Label {
                text: "Username:"
                width: 50
                horizontalAlignment: Text.AlignRight
                font.family: "Arial"
                font.pixelSize: 14
                font.weight: Font.DemiBold
        //        enabled: Mqtt.state == Mqtt.Disconnected
            }

            TextField {
                id: usernameField
                Layout.fillWidth: true
                text: settings.ftpUsername
                placeholderText: "Enter User name"
                selectByMouse: true
                selectedTextColor: "white"
        //        enabled: Mqtt.state == Mqtt.Disconnected
            }

            Label {
                text: "password:"
                width: 50
                horizontalAlignment: Text.AlignRight
                font.family: "Arial"
                font.pixelSize: 14
                font.weight: Font.DemiBold
        //        enabled: Mqtt.state == Mqtt.Disconnected
            }

            TextField {
                id: passwordField
                Layout.fillWidth: true
                text: settings.ftpPassword
                echoMode: TextInput.Password
                selectByMouse: true
                selectedTextColor: "white"
                placeholderText: "Enter password"
        //        enabled: Mqtt.state == Mqtt.Disconnected
            }
        }

        RowLayout{
            id: ftpHostInputField
            Layout.alignment: Qt.AlignHCenter
            width: 200
            spacing: 10

            Label {
                width: 50
                horizontalAlignment: Text.AlignRight
                text: "Hostname:"
                font.family: "Arial"
                font.pixelSize: 14
                font.weight: Font.DemiBold
        //        enabled: Mqtt.state == Mqtt.Disconnected
            }

            TextField{
                id:hostnameField
                Layout.fillWidth: true
        //        enabled: Mqtt.state == Mqtt.Disconnected
                text: ""
                placeholderText: "Enter indicated Port"
                selectByMouse: true
                selectedTextColor: "white"
            }

            Label {
                width: 50
                horizontalAlignment: Text.AlignRight
                text: "Port:"
                font.family: "Arial"
                font.pixelSize: 14
                font.weight: Font.DemiBold
        //        enabled: Mqtt.state == Mqtt.Disconnected
            }

            TextField{
                id:portField
                Layout.fillWidth: true
        //        enabled: Mqtt.state == Mqtt.Disconnected
                text: settings.ftpHostPort
                placeholderText: "Enter indicated Port"
                selectByMouse: true
                selectedTextColor: "white"
            }
        }

        RowLayout{
            id: errorMessage
            width: 100

            Text{
                id: errorText
                width: 50
                horizontalAlignment: Text.AlignRight
                font.family: "Arial"
                font.pixelSize: 14
                font.weight: Font.Light
            }
        }

        RowLayout{
            id: ftpLoginButtonList
            spacing: 30
            focus: true

            ClickButton {
                id: connectFtpButton
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredHeight: 40
                Layout.fillWidth: true
                text: "连接"

                onClicked: {
                    errorText.text = ""
                    errorText.color = "white"

                    // 保存配置(ip由心跳包提供)
                    settings.ftpHostPort = portField.text
                    settings.ftpUsername = usernameField.text
                    settings.ftpPassword = passwordField.text

                    FtpController.listFileInfo(Mqtt.seatNumber)

//                        var server_path = "MSETP-StudentSuite_2022_02_27_00.txt"
//                        FtpController.get(server_path,server_path)

                    FtpController.stateChanged.connect(printError)


                }
            }

            ClickButton {
                id: quitFtpButton
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredHeight: 40
                Layout.fillWidth: true
                text: "取消"

                onClicked: {
                    root.close()
                }
            }
        }
    }

}
