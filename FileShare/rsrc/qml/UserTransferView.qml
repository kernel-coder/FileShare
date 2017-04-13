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
        color: "#0072C5"
        anchors.left: parent.left; anchors.right: parent.right
        anchors.top: parent.top; height: 30

        Row {
            anchors.centerIn: parent
            spacing: 5
            LabelEx {
                color: "#D03A41"
                text: view.connObj.peerViewInfo.name
            }
            LabelEx {
                text: ", on port " + view.connObj.peerViewInfo.port + ", "
            }
            LabelEx {
                text: view.connObj.peerViewInfo.status == PeerViewInfoMsg.Free ? "Availale" : "Busy"
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
                    Image {
                        id: imgDirection
                        anchors.left: parent.left; anchors.verticalCenter: parent.verticalCenter
                        source: info.sizeTotalFile != info.sizeFileProgress ?
                                    (info.isSending ? "qrc:/images/rsrc/images/btn-upload.png" :
                                                      "qrc:/images/rsrc/images/btn-download.png")
                                  :
                                    (info.isSending ? "qrc:/images/rsrc/images/btn-upload-hovered.png" :
                                                      "qrc:/images/rsrc/images/btn-download-hovered.png")
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
                            text: "<a href=\"file:///%1\">%2</a> [%3/%4]".arg(info.filePathRoot).arg(info.filePath).arg(info.countFileProgress).arg(info.countTotalFile)
                            onLinkActivated: Utils.openUrl(link)
                        }
                        ProgressBarEx {
                            anchors.left: parent.left; anchors.right: parent.right
                            maximumValue: info.sizeTotalFile
                            value: info.sizeFileProgress
                        }
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
        //void fileTransfer(Connection* conn, RootFileUIInfo* uiInfo);
        onFileTransfer: {
            if (conn == view.connObj) {
                transferHistoryModel.append({info :  uiInfo})
            }
        }
    }
}
