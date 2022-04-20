import QtQuick 2.9
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.2

Window{
    id: root
    width: 400
    height: 250
//    modal: true

    property string finished: "0"
    property string total: "10"

    ColumnLayout{
        spacing: 20

        Label{
            id: title
            text: "正在下载 ( " + finished + " / " + total + " ) :"
        }

        Label{
            id: filename
            text: "hi.txt"
        }

        ProgressBar {
            id: control
            value: 0
            padding: 2

            Component.onCompleted: {
                if (value == 0){
                    indeterminate = true
                }
                else
                    indeterminate = false

            }

            onVisibleChanged: {
                if (control.visible == true){
                    mytimer.start()
                }
            }

            background: Rectangle {
                implicitWidth: 200
                implicitHeight: 6
                color: "#e6e6e6"
                radius: 3
            }

            contentItem: Item {
                implicitWidth: 200
                implicitHeight: 10

                Rectangle {
                    width: control.visualPosition * parent.width
                    height: parent.height
                    radius: 2
                    color: "#17a81a"
                }

            }

            Timer{
                id: mytimer
                interval: 1000
                repeat: true
                onTriggered: {
                    if (control.value < 1.0){
                        control.value += 0.1
                    }
                    else
                        stop()
                }
            }
        }

    }
}
