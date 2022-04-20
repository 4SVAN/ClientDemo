import QtQuick 2.0

Rectangle{
    id: root
    color: "white"
    clip: true

    states: [
        State { name: "true"; PropertyChanges { target: root; color : "lightgrey" } },
        State { name: "false"; PropertyChanges { target: root; color : "white" }}
    ]

    transitions: [
        Transition { ColorAnimation { to: "lightgrey"; duration: 60 } },
        Transition { ColorAnimation { to: "white"; duration: 30  } }
    ]
}


