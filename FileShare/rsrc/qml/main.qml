import QtQuick 2.3
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
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
        color: "#D03A41"
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 30
        Row {
            anchors.left: parent.left; anchors.leftMargin: 5
            anchors.verticalCenter: parent.verticalCenter
            spacing: 10
            LabelEx {
                text: "I'm "
            }
            LabelEx {
                color: "#00ff00"
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
                text: NetMgr.status == PeerViewInfoMsg.Free ? "Available" : "Busy"
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
        Row {
            anchors.right: btnSettings.left; anchors.rightMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            spacing: 2
            LabelEx {
                text: "Broadcast?"
                anchors.verticalCenter: parent.verticalCenter
            }

            CheckBox {
                anchors.verticalCenter: parent.verticalCenter
                checked: NetMgr.broadcastingEnabled
                onClicked: {
                    if (checked != NetMgr.broadcastingEnabled) {
                        NetMgr.updateBCEnabledChanged(checked)
                    }
                }
            }
        }

        ImageButton {
            id: btnSettings
            anchors.right: parent.right; anchors.rightMargin: 3
            anchors.verticalCenter: parent.verticalCenter
            imgNormal: "qrc:/images/rsrc/images/settings.png"
            imgHover: "qrc:/images/rsrc/images/settings-pressed.png"
            imgPressed: "qrc:/images/rsrc/images/settings-pressed.png"
            onClicked2: rectSettingsView.visible = !rectSettingsView.visible
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
                id: peerListView
                spacing: 5
                anchors.margins: 5
                anchors.fill: parent
                model: peersModel
                function currentConnection() {
                    if (currentIndex >= 0) {
                        return peersModel.get(currentIndex).connObj
                    }
                }

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
                        text: connObj.peerViewInfo.name + ": " + (connObj.peerViewInfo.status == PeerViewInfoMsg.Free ? "Available" : "Busy")
                    }
                }
            }

            Rectangle {
                id: rectManual
                visible: false
                color: "gray"
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                height: gridManualConnect.height + 20
                GridLayout {
                    id: gridManualConnect
                    anchors.centerIn: parent
                    anchors.margins: 5
                    rows: 3
                    columns: 2
                    rowSpacing: 10
                    columnSpacing: 5
                    LabelEx {text: "host:"}
                    TextField {id: tfIp; horizontalAlignment: Qt.AlignHCenter; verticalAlignment: Qt.AlignVCenter}
                    LabelEx {text: "port:"}
                    TextField {id: tfPort; horizontalAlignment: Qt.AlignHCenter; verticalAlignment: Qt.AlignVCenter}
                    BorderedButton {
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                        text: "cancel"
                        impWidth: 80; impHeight: 30
                        onClicked2: {rectManual.visible = false; btnAddManual.visible = true }

                    }
                    BorderedButton {
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                        text: "connect"
                        impWidth: 80; impHeight: 30;
                        onClicked2: {
                            rectManual.visible = false
                            btnAddManual.visible = true
                            NetMgr.connectManual(tfIp.text.trim(), parseInt(tfPort.text.trim()))
                        }
                    }
                }
            }

            BorderedButton {
                id: btnAddManual
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom; anchors.bottomMargin: 2
                impWidth: 30
                impHeight: 30
                radius: 15; borderWidth: 2
                text: "+"
                onClicked2: {
                    btnAddManual.visible = false
                    rectManual.visible = true
                }
            }
        }

        Rectangle {
            anchors.left: rectPeers.right
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            visible: peerListView.currentIndex < 0
            color: "#000000"
            LabelEx {
                anchors.fill: parent
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                color: "#ff0000"
                text: "Seems no machines available around"
            }
        }


        ContainerView {
            id: container
            color: "transparent"
            currentIndex: peerListView.currentIndex
            visible: peerListView.currentIndex >=  0
            anchors.left: rectPeers.right
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.bottom: parent.bottom
        }

        SettingsView {
            id: rectSettingsView
            visible: false
            anchors.fill: parent
        }
    }

    Connections {
        target: NetMgr
        onNewParticipant: {
            var view = compUserView.createObject(container, {connObj: connection})
            view.connObj = connection
            container.addView(view)
            peersModel.append({connObj :  connection})                        
        }

        onParticipantLeft: {
            console.log('ui particapath left called')
            for (var i = 0; i < peersModel.count; i++) {
                var obj = peersModel.get(i);
                if (obj.connObj == connection) {
                    container.removeAt(i)
                    peersModel.remove(i, 1);
                    console.log('removed peer from ui');
                    break;
                }
            }
        }
    }


    Component {
        id: compUserView
        UserTransferView {
            anchors.fill: parent
        }
    }
}
