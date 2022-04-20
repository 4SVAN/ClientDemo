import QtQuick 2.15
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1

import Mqtt 1.1
import ClientManager 1.1

import "../"
import "../myFunction.js" as MyFunc

ToolBar {
    background: Rectangle {
        implicitHeight: 40
        color: "whitesmoke"

        Rectangle {
            width: parent.width
            height: 1
            anchors.bottom: parent.bottom
            color: "transparent"
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        spacing: 5

        ToolButton {
            id: listDrawer
            x: 10
            text: qsTr("≡")
            onClicked: {
                if(menuListRect.y==0){
                    listDrawerStartAnim.start()
                }
                else{
                    listDrawerStopAnim.start()
                }
            }
        }

        ToolSeparator {}

        Label {
            text: "HomePage"
            font.family: "Arial"
            font.pixelSize: 14
            font.weight: Font.Bold

            elide: Label.ElideRight
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            Layout.fillWidth: true
        }

        ToolButton {
            id: menuButton
            text: qsTr("⋮")
            onClicked: {
                if(menuButtonRect.y==0){
                    menuButtonStartAnim.start()
                }
                else{
                    menuButtonStopAnim.start()
                }
            }
        }
    }

    // 左侧菜单栏
    MenuListRect{
        id: menuListRect
        x: 3
        height: menuList1.count * 40 + 6

        ListView{
            id: menuList1
            clip: true
            model: menuModel1
            anchors.fill: parent
            anchors.topMargin: 3
            anchors.bottomMargin: 3
            anchors.leftMargin: 1
            anchors.rightMargin: 1
            delegate: MenuDelegate{
                property bool isHovered1: false
                height: 40
                width: menuList1.width
                state: isHovered1

                Text {
                    text: name
                    font.family: "Arial"
                    font.pixelSize: 14
                    font.weight: Font.Bold

                    elide: Label.ElideRight
                    anchors.centerIn: parent
            //                    anchors.horizontalCenterOffset: 20
                }

                MouseArea{
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        menuList1.currentIndex = index
                        listDrawerStopAnim.start()
//                        if(menuList1.currentIndex==0){
//                            homePage.visible = true
//                            ftpToolPage.visible = false
//                        }

                        if(menuList1.currentIndex==1){
                            console.log(menuList1.currentIndex)
                            ftpLoginPopup.open()
                        }
                    }
                    onEntered: isHovered1 = true
                    onExited: isHovered1 = false
                }
            }
        }

    }

    // 右侧菜单栏
    MenuListRect{
        id: menuButtonRect
        x: parent.width - menuButtonRect.width - 3
        height: menuList2.count * 40 + 6

        ListView{
            id: menuList2
            clip: true
            model: menuModel2
            anchors.fill: parent
            anchors.topMargin: 3
            anchors.bottomMargin: 3
            anchors.leftMargin: 1
            anchors.rightMargin: 1

            delegate: MenuDelegate{
                property bool isHovered2: false
                height: 40
                width: menuList2.width
                state: isHovered2
                Text {
                    text: name
                    font.family: "Arial"
                    font.pixelSize: 14
                    font.weight: Font.Bold

                    elide: Label.ElideRight
                    anchors.centerIn: parent
            //                    anchors.horizontalCenterOffset: 20
                }

                MouseArea{
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        menuList2.currentIndex = index
                        menuButtonStopAnim.start()
                        if(menuList2.currentIndex==0){
                            if(!ClientManager.isEmpty()){
                                console.log(ClientManager.deviceInfo.length)
                                ClientManager.writeXlsFile("Test.xls")
                            }
                            else
                                console.log("不存在数据，请检查连接")
                        }


                    }
                    onEntered: isHovered2 = true
                    onExited: isHovered2 = false
                }
            }
        }

    }

    ListModel{
        id: menuModel1
        ListElement{
            name: "HomePage"
        }
        ListElement{
            name: "FTP 传输"
        }
    }

    ListModel{
        id: menuModel2
        ListElement{
            name: "导出到excel"
        }
    }

    //组合动画
    ParallelAnimation{
        id: listDrawerStartAnim
        //属性动画
        NumberAnimation{
            target: menuListRect
            properties: "y"
            from: 0
            to: listDrawer.height
            //动画持续时间，毫秒
            duration: 200
            //动画渐变曲线
            easing.type: Easing.OutQuad
        }
        NumberAnimation{
            target: menuListRect
            properties: "opacity"
            from: 0.1
            to: 1
            duration: 200;
            easing.type: Easing.OutQuad
        }
    }

    ParallelAnimation{
        id: listDrawerStopAnim
        NumberAnimation{
            target: menuListRect
            properties: "y"
            from: listDrawer.height
            to: 0
            duration: 100;
            easing.type: Easing.Linear
        }
        NumberAnimation{
            target: menuListRect
            properties: "opacity"
            from: 1
            to: 0.1
            duration: 100;
            easing.type: Easing.Linear
        }
    }

    //组合动画
    ParallelAnimation{
        id: menuButtonStartAnim
        //属性动画
        NumberAnimation{
            target: menuButtonRect
            properties: "y"
            from: 0
            to: listDrawer.height
            //动画持续时间，毫秒
            duration: 200
            //动画渐变曲线
            easing.type: Easing.OutQuad
        }
        NumberAnimation{
            target: menuButtonRect
            properties: "opacity"
            from: 0.1
            to: 1
            duration: 200;
            easing.type: Easing.OutQuad
        }
    }

    ParallelAnimation{
        id: menuButtonStopAnim
        NumberAnimation{
            target: menuButtonRect
            properties: "y"
            from: listDrawer.height
            to: 0
            duration: 100;
            easing.type: Easing.Linear
        }
        NumberAnimation{
            target: menuButtonRect
            properties: "opacity"
            from: 1
            to: 0.1
            duration: 100;
            easing.type: Easing.Linear
        }
    }

}


