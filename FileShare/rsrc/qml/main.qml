import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import "controls"
import com.kcl.fileshare 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("QFileShare")
    property var peersModel : ListModel{}

    Rectangle {
        id: titlebar
        color: "#555555"
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 30
        Row {
            anchors.centerIn: parent
            spacing: 10
            LabelEx {
                text: NetMgr.username + ":"
            }
            LabelEx {
                text: NetMgr.status == PeerViewInfoMsg.Free ? "Availale" : "Busy"
            }
            CheckBox {
                anchors.verticalCenter: parent.verticalCenter
                checked: NetMgr.status == PeerViewInfoMsg.Free
                onClicked: {
                    NetMgr.status = checked ? PeerViewInfoMsg.Free : PeerViewInfoMsg.Busy
                }
            }
        }
    }

    Rectangle {
        anchors.left: parent.left; anchors.right: parent.right
        anchors.top: titlebar.bottom; anchors.bottom: parent.bottom
        color: "#333333"
        Rectangle {
            color: "#222222"
            anchors.leftMargin: 5
            anchors.left: parent.left;
            anchors.top: parent.top;
            anchors.bottom: parent.bottom
            width: parent.width * .3
            ListView {
                model: peersModel
                delegate:  Component {
                    Item {
                        height: 30
                        Row {
                            spacing: 10
                            LabelEx {
                                text: name
                            }
                            LabelEx {
                                text: status == PeerViewInfoMsg.Free ? "Availale" : "Busy"
                            }
                        }
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
