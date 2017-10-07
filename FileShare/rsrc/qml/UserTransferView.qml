import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import "controls"
import com.kcl.fileshare 1.0

Rectangle {
    id: view
    property var connObj: null
    color: "transparent"
    anchors.fill: parent

    property var transferHistoryModel : ListModel{}

    Rectangle {
        id: rectTitle
        color: "#D03A41"
        anchors.left: parent.left; anchors.right: parent.right
        anchors.top: parent.top; height: 30

        Row {
            anchors.centerIn: parent
            spacing: 5
            LabelEx {
                color: "#00ff00"
                text: view.connObj.peerViewInfo.name
            }
            LabelEx {
                text: ", on port " + view.connObj.peerViewInfo.port + ", "
            }
            LabelEx {
                text: view.connObj.peerViewInfo.status == PeerViewInfoMsg.Free ? "Available" : "Busy"
            }
        }
    }

    Item {
        anchors.left: parent.left; anchors.right: parent.right
        anchors.top: rectTitle.bottom; anchors.bottom: teChat.top

        ListView {
            spacing: 20
            anchors.margins: 20
            anchors.fill: parent
            model: transferHistoryModel
            delegate:  Rectangle {
                color: "#444444"
                anchors.left: parent.left; anchors.right: parent.right
                height: 40
                Item {
                    anchors.fill: parent
                    anchors.margins: 5
                    anchors.bottomMargin: 0
                    visible: info != undefined
                    Image {
                        id: imgDirection
                        anchors.left: parent.left; anchors.verticalCenter: parent.verticalCenter
                        source: info != undefined ? ( info.sizeTotalFile != info.sizeFileProgress ?
                                    (info.isSending ? "qrc:/images/rsrc/images/btn-upload.png" :
                                                      "qrc:/images/rsrc/images/btn-download.png")
                                  :
                                    (info.isSending ? "qrc:/images/rsrc/images/btn-upload-hovered.png" :
                                                      "qrc:/images/rsrc/images/btn-download-hovered.png")
                                                     )
                                                  : ""
                    }
                    Column {
                        anchors.left: imgDirection.right; anchors.leftMargin: 5
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: 3
                        LabelEx {
                            anchors.horizontalCenter: parent.horizontalCenter
                            font.pixelSize: 12
                            linkColor: "white"
                            text: info != undefined ? "<a href=\"file:///%1\">%2</a> [%3/%4]".arg(info.filePathRoot).arg(info.filePath).arg(info.countFileProgress).arg(info.countTotalFile) : ""
                            onLinkActivated: Utils.openUrl(link)
                        }
                        ProgressBarEx {
                            anchors.left: parent.left; anchors.right: parent.right
                            maximumValue: info != undefined ? info.sizeTotalFile : 0
                            value: info != undefined ? info.sizeFileProgress : 0
                        }
                    }
                    LabelEx {
                        anchors.fill: parent
                        font.pixelSize: 12
                        horizontalAlignment: Qt.AlignRight
                        verticalAlignment: Qt.AlignVCenter
                        wrapMode: Text.WordWrap
                        color: "blue"
                        visible: chat != undefined && chat.sending
                        text : chat != undefined ? + chat.msg + " :ME"  : ""
                        onLinkActivated: Utils.openUrl(link)
                    }
                    LabelEx {
                        anchors.fill: parent
                        font.pixelSize: 12
                        horizontalAlignment: Qt.AlignLeft
                        verticalAlignment: Qt.AlignVCenter
                        wrapMode: Text.WordWrap
                        color: "red"
                        visible: chat != undefined && !chat.sending
                        text : chat != undefined ? "BUDDY: " + chat.msg : ""
                        onLinkActivated: Utils.openUrl(link)
                    }
                }
            }
        }

        DropArea {
            id: dragArea
            anchors.fill: parent
            enabled: view.connObj.peerViewInfo.status == PeerViewInfoMsg.Free
            onEntered: {
                var conn = view.connObj
                if (conn) {
                    drag.accept(Qt.CopyAction);
                    var files = "Want to share with " + conn.peerViewInfo.name + "?\n";
                    for (var i = 0; i < drag.urls.length; i++) {
                        files += (Utils.urlToFile(drag.urls[i]) + "\n")
                    }
                    lblDropInfo.text = files;
                }
                else {
                    drag.accepted = false
                }
            }
            onDropped: {
                drop.accept(Qt.CopyAction);
                var conn = view.connObj
                if (conn) {
                    FileMgr.shareFilesTo(conn, drop.urls)
                }
                else {
                    drop.accepted = false
                }
            }
        }

        Rectangle {
            anchors.fill: parent
            opacity: .5
            color: "gray"
            visible: dragArea.containsDrag
            LabelEx {
                id: lblDropInfo
                font.pixelSize: 12
                anchors.fill: parent
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
            }
        }
    }

    TextField {
        id: teChat
        enabled: view.connObj.peerViewInfo.status == PeerViewInfoMsg.Free
        anchors.left: parent.left; anchors.right: parent.right
        anchors.bottom: parent.bottom;
        placeholderText: "Chat Box"
        onAccepted: {
            if (teChat.text) {
                FileMgr.sendChatTo(view.connObj, teChat.text)
                transferHistoryModel.append({chat : {msg: teChat.text, sending: true}})
            }
        }
    }


    states: [
          State {
              when: dragArea.containsDrag
              PropertyChanges {
                  target: view
                  color: "grey"
              }
          }
      ]


    Connections {
        target: FileMgrUIHandler
        onFileTransfer: {
            if (conn == view.connObj) {
                transferHistoryModel.append({info :  uiInfo})
            }
        }
    }

    Connections {
        target: FileMgr
        onChatReceived: {
            if (conn == view.connObj) {
                transferHistoryModel.append({chat : {msg: msg, sending: false}})
            }
        }
    }
}
