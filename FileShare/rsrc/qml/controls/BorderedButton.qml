import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1

ImageButton {
    id: btn

    property color borderColor: "#656565"
    property int borderWidth: 1
    property real impWidth: 64
    property real impHeight: 36
    property color colorDisabled: "transparent"
    property color colorNormal: "transparent"
    property color colorHovered: "#0072C5"
    property color colorPressed: "#D03A41"
    property int verticalOffset: 0


    style: ButtonStyle {
        background: Rectangle {
            implicitWidth: impWidth
            implicitHeight: impHeight;
            border.width: 1
            border.color: control.borderColor
            radius: 5
            color : enabled ? (control.down ? control.colorPressed : (control.hovered ? control.colorHovered : control.colorNormal)) : control.colorDisabled
            Row {
                spacing: 7; anchors.centerIn: parent
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
