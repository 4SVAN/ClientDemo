import QtQuick 2.15
import QtQuick.Controls 2.4

Button {
    id: root
    hoverEnabled: true

    background: Rectangle{
        id: buttonBackground
        color: "lightgrey"
        state: root.hovered
        clip: true

        states: [
            State { name: "true"; PropertyChanges { target: buttonBackground; color : "lightgrey" } },
            State { name: "false"; PropertyChanges { target: buttonBackground; color : "whitesmoke" }}
        ]

        transitions: [
            Transition { ColorAnimation { to: "lightgrey"; duration: 30 } },
            Transition { ColorAnimation { to: "whitesmoke"; duration: 10  } }
        ]
    }
}
