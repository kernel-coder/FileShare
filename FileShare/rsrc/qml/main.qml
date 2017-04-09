import QtQuick 2.5
import QtQuick.Window 2.2
import "controls"
import "com.kcl.fileshare"

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("QFileShare")
    property var peersModel : ListModel{}

    Rectangle {
        anchors.fill: parent
        color: "#333333"
        Rectangle {
            color: "#222222"
            anchors.margins: 5
            anchors.left: parent.left;
            anchors.top: parent.top;
            anchors.bottom: parent.bottom
            width: parent.width * .3
            ListView {
                model: peersModel
                delegate:  Component {
                    Item {
                        height: 30
                        anchors.left: parent.left; anchors.right: parent.right
                        LabelEx {anchors.centerIn: parent; text: name}
                    }
                }
            }

        }
    }

    Connections {
        target: NetMgr
        onNewParticipant: peersModel.append({name: connection.peerViewInfo.name})
    }
}
