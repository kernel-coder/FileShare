import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import com.kcl.fileshare 1.0

ProgressBar {
    id: progressbar

    property color  backColor : "#5D5D5D"
    property color progressColor: "#D03A41" // D03A41
    property color textColor: "white"
    property int fontPixelSize: 12

    anchors.left: parent.left; anchors.right: parent.right; height: 20;
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
        text: "%1/%2".arg(Utils.formatSize(progressbar.value)).arg(Utils.formatSize(progressbar.maximumValue));
    }

    LabelEx {
        anchors.right: parent.right
        anchors.rightMargin: 2
        anchors.verticalCenter: parent.verticalCenter
        font.pixelSize: progressbar.fontPixelSize
        text: "[%1%]".arg((progressbar.value * 100 / progressbar.maximumValue).toFixed(0))
    }
}

