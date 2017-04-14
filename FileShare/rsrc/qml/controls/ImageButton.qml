import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1

ButtonEx {
    id: btn
    property url imgNormal
    property url imgHover
    property url imgPressed
    property url imgDisabled
    property bool fontWieghtBold: false
    property real textPixelSize: 18
    property color textColorDisabled: "#5D5D5D"
    property color textColorNormal: "white"
    property color textColorHovered: "white"
    property color textColorPressed: "white"

    style: ButtonStyle {
        background: ImageEx {
            id:img;
            source : enabled ? (btn.down ? btn.imgPressed : (btn.hovered ? btn.imgHover : btn.imgNormal)) : btn.imgDisabled
            LabelEx {
                anchors.centerIn: parent
                text: control.text
                font.pixelSize: control.textPixelSize
                color : enabled ? (btn.down ? btn.textColorPressed : (btn.hovered ? btn.textColorHovered : btn.textColorNormal)) : btn.textColorDisabled
                font.bold: control.fontWieghtBold
            }
        }
        label : Item{}
    }

    Behavior on x {
        NumberAnimation { duration: 400 }
    }
}
