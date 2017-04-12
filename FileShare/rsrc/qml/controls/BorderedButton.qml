import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1

ImageButton {
    id: btn

    property color borderColor: "white"
    property real borderWidth: 1
    property real radius: 5
    property real impWidth: 64
    property real impHeight: 36
    property color colorDisabled: "transparent"
    property color colorNormal: "transparent"
    property color colorHovered: "#0072C5"
    property color colorPressed: "#D03A41"
    property int verticalOffset: 0
    property real margins: 5


    style: ButtonStyle {
        background: Rectangle {
            implicitWidth: impWidth
            implicitHeight: impHeight;
            border.width: borderWidth
            border.color: control.borderColor
            radius: control.radius
            color : enabled ? (control.down ? control.colorPressed : (control.hovered ? control.colorHovered : control.colorNormal)) : control.colorDisabled
            Row {
                spacing: 7; anchors.centerIn: parent
                anchors.margins: control.margins
                ImageEx {
                    id:img;
                    source : enabled ? (control.down ? control.imgPressed : (control.hovered ? control.imgHover : control.imgNormal)) : control.imgDisabled
                }
                Text {
                    color : enabled ? (control.down ? control.textColorPressed : (control.hovered ? control.textColorHovered : control.textColorNormal)) : control.textColorDisabled
                    anchors.verticalCenter: parent.verticalCenter; anchors.verticalCenterOffset: control.verticalOffset
                    horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
                    text: control.text
                    font.pixelSize: control.textPixelSize
                }
            }
        }

        label: Item {}
    }    
}
