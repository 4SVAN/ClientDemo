import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.2
import QtQuick.Layouts 1.2
import Qt.labs.qmlmodels 1.0

import Mqtt 1.1
import TcpClient 1.1
import ClientManager 1.1

import "./myComponents"
import "./myPages"
import "./myFunction.js" as MyFunction

ApplicationWindow{
    id: root
    width: 1024
    height: 768
    title: qsTr("MSETP-Managerkits")

    function updateModel(row, column, value){
        console.log(tableModel.data(tableModel.index(row, column), "display"))
        if(tableModel.setData(tableModel.index(row, column), "display", value)){
            console.log("hi")
        }
        else console.log("no")
    }

    Component.onCompleted: {
        TcpClient.init()
        for(let i=0;i<ClientManager.count();i++){
//            console.log(i + " init state: " + ClientManager.deviceInfo[i].checkState)
            // 5.15 new feature - Nullish Coalescing
            ClientManager.deviceInfo[i].hostname = ClientManager.readDeviceInfoSettings(i, "hostname") ?? ""
            ClientManager.deviceInfo[i].macAddress = ClientManager.readDeviceInfoSettings(i, "macAddr") ?? ""
            console.log(i + " : " + ClientManager.deviceInfo[i].hostname + " - " + ClientManager.deviceInfo[i].macAddress)
            tableModel.appendRow({"checked" : ClientManager.deviceInfo[i].checkState,
                                  "seatID" : i,
                                  "hostname" : ClientManager.deviceInfo[i].hostname,
                                  "macAddr" : ClientManager.deviceInfo[i].macAddress})
        }
    }

    header: MenuBar{
        id: headerMenuBar
    }

    ButtonGroup {
        id: childGroup
        exclusive: false
        checkState: seatCheckBox.checkState
    }

    Rectangle{
        id:header
        width: parent.width
        height: 30

        Row{
            spacing: 0

            Repeater{
                model: ["全选","seat","hostname","macAddr"]

                Rectangle{
                    width: header.width/4
                    height: header.height
                    color: "#666666"
                    border.width: 1
                    border.color: "#848484"
                    Text {
                        text: modelData
                        anchors.centerIn: parent
                        font.pixelSize: 14
                        font.bold: Font.DemiBold
                        color: "white"
                    }
                }
            }
        }
    }

    TableView{
        id:tableView
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        clip: true
        boundsBehavior: Flickable.OvershootBounds
        columnWidthProvider: function(column) {return parent.width/4}

        ScrollBar.vertical: ScrollBar {
            anchors.right:parent.right
            anchors.rightMargin: 0
            visible: tableModel.rowCount > 5
            background: Rectangle{
                color:"#666666"
            }
            onActiveChanged: {
                active = true;
            }
            contentItem: Rectangle
            {
                implicitWidth  : 8
                implicitHeight : 30
                radius : 3
                color  : "#848484"
            }
        }

        model: TableModel {
            id:tableModel

            TableModelColumn{display: "checked"}
            TableModelColumn{display: "seatID"}
            TableModelColumn{display: "hostname"}
            TableModelColumn{display: "macAddr"}


        }

        delegate: Repeater{
            model: [100,200,300,400]
        }

    }

    footer: ToolBar {

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

            CheckBox{
                id: seatCheckBox
                text: qsTr("全选")
                tristate: true
                checkState: childGroup.checkState
                onClicked: {
                    var array = new Array(ClientManager.count()).fill(checkState === Qt.Checked ? true : false)

                    ClientManager.setCheckStateGroup(array)
                }
                nextCheckState: function() {
                    if (checkState === Qt.Checked)
                        return Qt.Unchecked
                    else{
                        return Qt.Checked
                    }
                }
            }

            Label{
                Layout.fillWidth: true
                text: "version 1.3"
                font.pixelSize: 10
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
            }

            ClickButton{
                id: ftpBatchDownloadButton
                height: parent.height - 20
                text: "批量下载"
                onClicked: {
//                    MqttModel.batchDownload()
                }
            }

        }
    }
}
