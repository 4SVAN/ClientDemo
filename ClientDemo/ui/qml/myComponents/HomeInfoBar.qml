import QtQuick 2.15
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2

RowLayout{
    id: root
    spacing: 10

    Rectangle{
        id: blackTitle1
        Layout.minimumWidth: scaleWidth * 0.1
        Layout.maximumWidth: scaleWidth * 0.1
        Layout.fillHeight: true
    }

    Label{
        text: qsTr("座位")
        font.family: "Arial"
        font.pixelSize: 14
        font.weight: Font.DemiBold
        Layout.minimumWidth: scaleWidth * 0.08  + 10
        Layout.maximumWidth: scaleWidth * 0.08  + 10
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter
    }

    Label{
        text: qsTr("连接状态")
        font.family: "Arial"
        font.pixelSize: 14
        font.weight: Font.DemiBold
        Layout.minimumWidth: scaleWidth * 0.08
        Layout.maximumWidth: scaleWidth * 0.08
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter
    }

    Rectangle{
        id: blackTitle2
        Layout.minimumWidth: scaleWidth * 0.35
        Layout.maximumWidth: scaleWidth * 0.35
        Layout.fillHeight: true
    }

    Label{
        text: qsTr("实时日志")
        font.family: "Arial"
        font.pixelSize: 14
        font.weight: Font.DemiBold
        Layout.minimumWidth: 101
        Layout.maximumWidth: 101
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter
    }

    Label{
        text: qsTr("文件下载")
        font.family: "Arial"
        font.pixelSize: 14
        font.weight: Font.DemiBold
        Layout.minimumWidth: 101
        Layout.maximumWidth: 101
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter
    }
}
