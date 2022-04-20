import QtQuick 2.0
import QtGraphicalEffects 1.15

Rectangle{
    y: 0
    width: 100
    border.color: "black"
    border.width: 1
    visible: y > 10 ? true : false

    layer.enabled: true
    layer.effect: DropShadow{
        horizontalOffset:  3
        verticalOffset:  3
        radius:  8.0
        samples:  17
        color:  "#80000000"
    }
}
