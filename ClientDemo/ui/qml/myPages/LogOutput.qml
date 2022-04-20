import QtQuick 2.9
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.2

import "../myComponents"
import TcpClient 1.1

Popup{
    id: root
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    onVisibleChanged: {
        textArea.clear()
    }

    onClosed: {
        TcpClient.handleDisconnect()
    }

    property bool isFocus: false

    function setMessage(message){
        textArea.text += message
        if (!isFocus){
            textArea.cursorPosition = textArea.length - 1
        }
        else{

        }
    }

    function changeErrorMessage(message){
        console.log("TCP连接异常")
        textArea.clear()
        textArea.text += "Occur QAbstractSocket::SocketError: "
        setMessage(message)
    }

    MouseArea{
        anchors.fill: parent
        onClicked: {
            forceActiveFocus()
        }
    }

    RowLayout{
        id: userInterface
        anchors.fill: parent
        Layout.margins: 10
//            spacing: 50

        Rectangle {
            id: textRect
            Layout.fillHeight: true
            Layout.fillWidth: true

            color: "#FFFFFF"
            border.color: "lightgrey"
            border.width: 1



            ScrollView {
                id: view
                anchors.fill: parent
                anchors.margins: 20
//                        clip: true

                onActiveFocusChanged:  {
                    console.log(textArea.focusReason)
                    isFocus = !isFocus
                }

                ScrollBar.vertical.policy: ScrollBar.AlwaysOn
                contentWidth: 15

                TextArea {
                    id: textArea
                    cursorVisible: true
                    cursorPosition: selectionEnd
                    focusReason: Qt.MouseFocusReason
                    wrapMode: TextArea.Wrap//换行
                    font.pixelSize: 20
                    font.weight: Font.DemiBold
                    focus: true
                    textFormat: TextArea.AutoText
                    selectByMouse: true
                    selectByKeyboard: true
//                            color: "red"
//                            hoverEnabled: false

//                        text: TcpClient.message
                }
            }
        }

        ClickButton {
            id: quitPopupButton
            height: 50
            Layout.alignment: Qt.AlignHCenter
            Layout.maximumHeight: 40
            Layout.maximumWidth: 60
            text: "取消"

            onClicked: {
                root.close()
                if(TcpClient.clientState() === "ConnectedState"){
                    TcpClient.handleDisconnect()
                }
            }
        }
    }

    Component.onCompleted: {
        // 窗口加载完毕后才开始连接信号接收数据
        TcpClient.messageChanged.connect(setMessage)
        TcpClient.errorOccurred.connect(changeErrorMessage)
        TcpClient.tcpDisconnect.connect(setMessage)
    }
}
