import QtQuick 2.3
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0

import Mqtt 1.1
import FtpController 1.1

import "../myComponents"

Dialog{
    id: root
    width: 800
    height: 600
    x: homePage.width / 2 - root.width
    y: homePage.height / 2 - root.height
    title: qsTr("选择文件")

    onVisibleChanged: {
        console.log("visible:" + visible)
    }

    ButtonGroup {
        id: childGroup
        exclusive: false
        checkState: filesBox.checkState
    }

    ColumnLayout{
        id: ftpPage
        anchors.fill: parent
        spacing: 10

        RowLayout{
            id: titleBar
            Layout.maximumHeight: 20
            Layout.fillWidth: true
            Layout.margins: 10
            spacing: 10

            CheckBox{
                id: filesBox
                text: qsTr("全选")
                tristate: true
                checkState: childGroup.checkState
                onClicked: {
                    FtpModel.selectAll(checkState === Qt.Checked ? Qt.Checked : Qt.Unchecked)
                }

                nextCheckState: function() {
                    if (checkState === Qt.Checked)
                        return Qt.Unchecked
                    else
                        return Qt.Checked
                }
            }


        }

        ListView{
            id: ftpListView
            Layout.fillHeight: true
            Layout.fillWidth: true
            clip: true
            spacing: 10
            focus: true
            model: FtpModel
            delegate: ftpDelegate
        }

        RowLayout{
            id: downloadBar
            Layout.maximumHeight: 100
            Layout.fillWidth: true
            Layout.margins: 10
            spacing: 10

            Label{
                text: qsTr("文件名:")
                Layout.fillWidth: true
                font.family: "Arial"
                font.pixelSize: 14
                font.weight: Font.DemiBold
                horizontalAlignment: Label.AlignRight
            }

            TextField{
                id: filenameInput
                Layout.topMargin: 30
                Layout.bottomMargin: 30
                selectByMouse: true
            }

            Button{
                id: ftpDownloadButton
                text: qsTr("下载")
                onClicked: {
                    FtpModel.downloadFile(Mqtt.seatNumber)
//                    progress.show()
                }
            }
        }

    }

    Component{
        id: ftpDelegate

        MenuDelegate{
            property bool isHovered: false
            height: 40
            width: ftpListView.width
//                        anchors.margins: 40
            state: isHovered

            MouseArea{
                id: mouseArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    ftpListView.currentIndex = index
//                                console.log(index + " before : " + model.modelData.state + " : " + deviceCheckBox.checked)
                    model.modelData.state = model.modelData.state ? Qt.Unchecked : Qt.Checked
//                                console.log(index + " after : " + model.modelData.state + " : " + deviceCheckBox.checked)
                }
                onEntered: isHovered = true
                onExited: isHovered = false

            }

            CheckBox{   // 复选框
                id: deviceCheckBox
                anchors.margins: 10
                leftPadding: 20
                width: 15
                anchors.verticalCenter: parent.verticalCenter
                ButtonGroup.group: childGroup
                checked: model.modelData.state
                onClicked: {
//                                console.log(index + " before : " + model.modelData.state + " : " + deviceCheckBox.checked)
                    model.modelData.state = deviceCheckBox.checked
//                                console.log(index + " after : " + model.modelData.state + " : " + deviceCheckBox.checked)
//                                console.log(seatCheckBox.checkState)
                }
            }

            Rectangle { // 文件图标
                id: fileIcon
                width: 34
                height: 34
                anchors.left: deviceCheckBox.right
                anchors.top: parent.top
                anchors.leftMargin: 30
                anchors.topMargin: 3

                Image {
                    anchors.fill: parent
                    fillMode: Image.PreserveAspectFit
                    source: "qrc:/img/txtFileIcon.png"
                }
            }

            Label { // 文件名
                id: filename
                text: model.modelData.filename.split("  ")[0]
                anchors.margins: 30
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: fileIcon.right
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                font.pixelSize: 12
                font.weight: Font.DemiBold
            }

            Label { // 最后修改日期
                id: fileLastModified
                text: model.modelData.filename.split("  ")[1]
                anchors.margins: 30
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: filename.right
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                font.pixelSize: 12
                font.weight: Font.DemiBold
            }

            Label { // 文件类型
                id: fileType
                text: model.modelData.filename.split("  ")[2]
                anchors.margins: 30
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: fileLastModified.right
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                font.pixelSize: 12
                font.weight: Font.DemiBold
            }

            Label { // 文件大小
                id: fileSize
                text: model.modelData.filename.split("  ")[3]
                anchors.margins: 30
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: fileType.right
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                font.pixelSize: 12
                font.weight: Font.DemiBold
            }

        }

    }

    MyProgressBar{
        id: progress
        x: root.width / 2
        y: root.height / 2
    }
}
