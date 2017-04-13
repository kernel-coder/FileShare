import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import "controls"
import com.kcl.fileshare 1.0


Rectangle {
    color: "#666666"
    Grid {
        columns: 2
        rows: 2
        anchors.centerIn: parent
        CheckBox {
            checked: NetMgr.broadcastingEnabled
            text : "Broadcast User Info "
            onClicked: {
                console.log("checked unchecked")

                if (checked != NetMgr.broadcastingEnabled) {
                    console.log("checked unchecked")
                    NetMgr.updateBCEnabledChanged(checked)
                }
            }
        }
    }
}
