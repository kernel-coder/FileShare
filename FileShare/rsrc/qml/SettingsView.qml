import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import "controls"
import com.kcl.fileshare 1.0
import QtQuick.Dialogs 1.0



Rectangle {
    color: "#666666"
    Column {
        anchors.centerIn: parent
        spacing: 10

        Row {
            spacing: 2
            anchors.horizontalCenter: parent.horizontalCenter
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
            spacing: 2
            anchors.horizontalCenter: parent.horizontalCenter
            LabelEx {
                text: "Broadcast"
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

            LabelEx {
                text: ",  Interval"
                anchors.verticalCenter: parent.verticalCenter
            }

            SpinBox {
                enabled: NetMgr.broadcastingEnabled
                minimumValue: 1000
                maximumValue: 100000
                value: NetMgr.broadcastInterval
                onValueChanged: NetMgr.updateBCIntervalChanged(value)
            }
        }

        Row {
            spacing: 2
            anchors.horizontalCenter: parent.horizontalCenter
            LabelEx {
                text: "Skip Existing File"
                anchors.verticalCenter: parent.verticalCenter
            }

            CheckBox {
                anchors.verticalCenter: parent.verticalCenter
                checked: AppSettings.skippExistingFile
                onClicked: {
                    if (checked != AppSettings.skippExistingFile) {
                        AppSettings.skippExistingFile = checked
                    }
                }
            }
        }

        Row {
            spacing: 5
            LabelEx {text: "Save To:"}
            TextField{id: tfPath; width: 200; text: NetMgr.saveFolderName}
            BorderedButton {
                impHeight: tfPath.height
                textPixelSize: 12
                text: "change"
                onClicked2: {
                    fileDialog.folder = Utils.fileRelativeAppPath(tfPath.text.trim())
                    fileDialog.open()
                }
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: "Please Select Path Where to Save File"
        selectFolder : true
        selectMultiple : false
        onAccepted: {
            NetMgr.updateSavedFolderPath(Utils.urlToFile(fileDialog.folder))
        }
    }
}
