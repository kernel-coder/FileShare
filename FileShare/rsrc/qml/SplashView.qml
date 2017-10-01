import QtQuick 2.0
import QtQuick.Controls 1.2

Item {
    anchors.fill: parent

    Column {
        spacing: 50
        anchors.centerIn: parent
        anchors.verticalCenterOffset: - parent.height * .1
        Label {
            id: loadingLabel
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("PLEASE WAIT! LOADING...")
            color: "#A9353F"
            font.bold: true
            font.pixelSize: 40
        }
        BusyIndicator {
            running: true;
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.parent.width * .25
            height: parent.parent.width * .25
        }
    }
}
