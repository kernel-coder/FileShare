import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.2

ProgressBar {
    id: progressbar

    property color  backColor : "#5D5D5D"
    property color progressColor: "#7BC5FE" // D03A41
    property color textColor: "white"
    property int fontPixelSize: 20

    anchors.left: parent.left; anchors.right: parent.right; height: 30;
    minimumValue: 0; maximumValue: 100; value: 40;
    style: ProgressBarStyle {
        background: Rectangle {
            color: progressbar.backColor
        }
        progress: Rectangle {
            color: progressbar.progressColor

        }
    }
    LabelEx {
        //anchors.right: control.value > 5 ? parent.right : undefined; anchors.rightMargin: 2
        //anchors.left: control.value <= 5 ? parent.right : undefined; anchors.leftMargin: 2
        anchors.centerIn: parent; color: progressbar.textColor
        font.pixelSize: progressbar.fontPixelSize
        text: "%1/%2".arg(utils.formatSize(progressbar.value)).arg(utils.formatSize(progressbar.maximumValue));
    }
}

