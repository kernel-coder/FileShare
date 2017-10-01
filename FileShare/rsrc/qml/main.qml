import QtQuick 2.3
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import "controls"
import com.kcl.fileshare 1.0

Window {
    visible: true
    x: AppSettings.appPosX
    onXChanged: AppSettings.updateAppGeometry(x, y, width, height)
    y: AppSettings.appPosY
    onYChanged:  AppSettings.updateAppGeometry(x, y, width, height)
    width: AppSettings.appWidth
    onWidthChanged:  AppSettings.updateAppGeometry(x, y, width, height)
    height: AppSettings.appHeight
    onHeightChanged:  AppSettings.updateAppGeometry(x, y, width, height)
    title: qsTr("LAN Sharing")

    Rectangle {
        color: "white"
        anchors.fill: parent

        SplashView {
            id: splash
            anchors.fill: parent
        }

        Loader {
            id: mainLoader;
            asynchronous: true;
            anchors.fill: parent
            onStatusChanged: {
                if( mainLoader.status == Loader.Ready ) {
                    //splash.visible = false
                }
            }
        }
    }

    Component.onCompleted: {
        mainLoader.source = "MainWindow.qml"
    }
}
