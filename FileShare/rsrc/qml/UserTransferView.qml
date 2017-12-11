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

    property ListModel transferHistoryModel : ListModel{}
    function indexOfItem(item) {
        for(var i = 0; i < transferHistoryModel.count; i++) {
            if (transferHistoryModel.get(i) == item) {
                return i;
            }
        }
        return -1;
    }

    function initialize() {
        var historyItems = HistoryMgr.getHistoryForDevice(connObj)
        for (var i = 0; i < historyItems.length; i++) {
            transferHistoryModel.append(historyItems[i]);
        }
    }

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
                    visible: isFileTransfer
                    Image {
                        id: imgDirection
                        anchors.left: parent.left; anchors.verticalCenter: parent.verticalCenter
                        source: fileInfo.sizeTotalFile != fileInfo.sizeFileProgress ?
                                    (fileInfo.isSending ? "qrc:/images/rsrc/images/btn-upload.png" :
                                                      "qrc:/images/rsrc/images/btn-download.png")
                                  :
                                    (fileInfo.isSending ? "qrc:/images/rsrc/images/btn-upload-hovered.png" :
                                                      "qrc:/images/rsrc/images/btn-download-hovered.png")
                    }

                    Row {
                        anchors.right: parent.right
                        anchors.rightMargin: 0
                        anchors.top: parent.top
                        anchors.topMargin: -5
                        spacing: 10

                        ImageButton {
                            width: 16; height: 16
                            visible: isFileTransfer && fileInfo.sizeFileProgress < fileInfo.sizeTotalFile &&
                                     (fileInfo.transferStatus == TransferStatusFlag.Pause || fileInfo.transferStatus == TransferStatusFlag.Running)
                            imgNormal: fileInfo.transferStatus == TransferStatusFlag.Running ?
                                           "qrc:/images/rsrc/images/pause.png" : "qrc:/images/rsrc/images/play.png"
                            imgHover: fileInfo.transferStatus == TransferStatusFlag.Running ?
                                          "qrc:/images/rsrc/images/pause.png" : "qrc:/images/rsrc/images/play.png"
                            imgPressed: fileInfo.transferStatus == TransferStatusFlag.Running ?
                                            "qrc:/images/rsrc/images/pause-pressed.png" : "qrc:/images/rsrc/images/play-pressed.png"
                            onClicked2: {
                                console.log("clicked pause -- ", fileInfo.transferStatus)
                                if (fileInfo.transferStatus == TransferStatusFlag.Running) {
                                    FileMgrUIHandler.applyControlStatus(view.connObj, fileInfo, TransferStatusFlag.Pause);
                                }
                                else {
                                    FileMgrUIHandler.applyControlStatus(view.connObj, fileInfo, TransferStatusFlag.Running)
                                }
                            }
                        }

                        ImageButton {
                            width: 16; height: 16
                            imgNormal: "qrc:/images/rsrc/images/btn-cancel.png"
                            imgHover: "qrc:/images/rsrc/images/btn-cancel.png"
                            imgPressed: "qrc:/images/rsrc/images/btn-cancel-pressed.png"
                            onClicked2: FileMgrUIHandler.deleteItem(view.connObj, fileInfo);
                        }
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
                            text: "<a href=\"file:///%1\">%2</a> [%3/%4]"
                                           .arg(fileInfo.filePathRoot).arg(fileInfo.filePath)
                                           .arg(fileInfo.countFileProgress).arg(fileInfo.countTotalFile)
                            onLinkActivated: Utils.openUrl(link)
                        }
                        ProgressBarEx {
                            anchors.left: parent.left; anchors.right: parent.right
                            maximumValue: fileInfo.sizeTotalFile
                            value: fileInfo.sizeFileProgress
                        }
                    }
                }
                LabelEx {
                    anchors.fill: parent
                    anchors.margins: 2
                    anchors.leftMargin: 20
                    font.pixelSize: 12
                    horizontalAlignment: Qt.AlignRight
                    verticalAlignment: Qt.AlignVCenter
                    wrapMode: Text.WordWrap
                    color: "#0072C5"
                    visible: !isFileTransfer && isChatSending
                    text : chatMsg + " #ME"
                    onLinkActivated: Utils.openUrl(link)
                }
                LabelEx {
                    anchors.fill: parent
                    anchors.margins: 2
                    anchors.rightMargin: 20
                    font.pixelSize: 12
                    horizontalAlignment: Qt.AlignLeft
                    verticalAlignment: Qt.AlignVCenter
                    wrapMode: Text.WordWrap
                    color: "#D03A41"
                    visible: !isFileTransfer && !isChatSending
                    text : view.connObj.peerViewInfo.name + "# " + chatMsg
                    onLinkActivated: Utils.openUrl(link)
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
            anchors.fill: parent;
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
        placeholderText: "Chat with " + view.connObj.peerViewInfo.name
        onAccepted: {
            if (teChat.text.trim()) {
                FileMgr.sendChatTo(view.connObj, teChat.text.trim())
                teChat.text = ""
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
                transferHistoryModel.append(uiInfo)
                if (!appWindow.visible /*|| appWindow.visibility == Window.Minimized*/) {
                    TrayMgr.showMessage(view.connObj.peerViewInfo.name, "Sending: %1".arg(uiInfofileInfo.filePath))
                }
            }
        }
    }

    Connections {
        target: FileMgr
        onChatTransfer: {
            if (conn == view.connObj) {
                transferHistoryModel.append(uiInfo)
                if (!appWindow.visible || appWindow.visibility == Window.Minimized) {
                    TrayMgr.showMessage(view.connObj.peerViewInfo.name, uiInfo.chatMsg)
                }
            }
        }
    }

    Connections {
        target: HistoryMgr
        onHistoryItemRemoved : {
            if (conn == view.connObj) {
                var ioi = indexOfItem(item)
                if (ioi >= 0) {
                    transferHistoryModel.remove(ioi, 1)
                }
            }
        }
    }
}
