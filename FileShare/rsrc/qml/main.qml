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
                text: "I'm "
            }
            LabelEx {
                color: "#0072C5"
                font.bold: true
                text: NetMgr.username
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                verticalAlignment: Qt.AlignVCenter

                MouseArea {
                    anchors.fill: parent
                    onDoubleClicked: {
                        txtUsername.text = NetMgr.username
                        txtUsername.visible = true
                    }
                }
                TextField {
                    id: txtUsername
                    visible: false
                    anchors.fill: parent
                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter
                    onAccepted:  {
                        if (text.trim().length > 0) {
                            NetMgr.username = text.trim()
                            NetMgr.broadcastUserInfoChanged()
                        }
                    }
                    onEditingFinished: visible = false
                }
            }
            LabelEx {
                text: " on port " + NetMgr.port + ", "
            }

            LabelEx {
                text: NetMgr.status == PeerViewInfoMsg.Free ? "Availale" : "Busy"
            }
            CheckBox {
                anchors.verticalCenter: parent.verticalCenter
                checked: NetMgr.status == PeerViewInfoMsg.Free
                onClicked: {
                    NetMgr.status = checked ? PeerViewInfoMsg.Free : PeerViewInfoMsg.Busy
                    NetMgr.broadcastUserInfoChanged()
                }
            }
        }
    }

    Rectangle {
        anchors.left: parent.left; anchors.right: parent.right
        anchors.top: titlebar.bottom; anchors.bottom: parent.bottom
        color: "#333333"
        Rectangle {
            id: rectPeers
            color: "#222222"
            anchors.left: parent.left;
            anchors.top: parent.top;
            anchors.bottom: parent.bottom
            width: parent.width * .3
            ListView {
                spacing: 5
                anchors.margins: 5
                anchors.fill: parent
                model: peersModel
                delegate:  Component {
                    BorderedButton {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        checkable: true;
                        isRadioMode: true
                        borderWidth: 0
                        radius: 0
                        colorNormal: "#333333"
                        impHeight: 30
                        text: name + ": " + (status == PeerViewInfoMsg.Free ? "Availale" : "Busy")
                    }
                }
            }
        }
        Rectangle {
            id: dropRect
            color: "transparent"
            anchors.left: rectPeers.right
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            LabelEx {
                id: lblDropInfo
                font.pixelSize: 10
                visible: dragArea.containsDrag
                anchors.fill: parent
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
            }

            states: [
                  State {
                      when: dragArea.containsDrag
                      PropertyChanges {
                          target: dropRect
                          color: "grey"
                      }
                  }
              ]
            DropArea {
                id: dragArea
                anchors.fill: parent
                onEntered: {
                    drag.accept(Qt.CopyAction); console.log(drag.action);
                    var files = "Wants to share?\n";
                    for (var i = 0; i < drag.urls.length; i++) {
                        files += (Utils.urlToFile(drag.urls[i]) + "\n")
                    }
                    lblDropInfo.text = files;
                }
                onDropped: {
                    drop.accept(Qt.CopyAction); console.log(drop.urls)
                    FileMgr.shareFilesTo(null, drop.urls)
                }
            }
        }
    }

    Connections {
        target: NetMgr
        onNewParticipant: {
            peersModel.append({ connObj :  connection, name: connection.peerViewInfo.name,
                                status: connection.peerViewInfo.status
                              })
        }
        onParticipantLeft: {
            for (var i = 0; i < peersModel.count; i++) {
                var obj = peersModel.get(i);
                if (obj.connObj == connection) {
                    peersModel.remove(i, 1);
                    break;
                }
            }
        }
    }
}
