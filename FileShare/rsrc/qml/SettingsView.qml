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
