import QtQuick 2.0



Rectangle {
    id: tooltip
    color: "#A9353F"; radius: 4; visible: false
    width: 0; height: 50; z: 200

    function hideTooltip() {
        timerTooltipHider.stop();
        animTooltip.to = "0"
        animTooltip.start()
    }

    function showTooltip(msg, duration) {
        timerTooltipHider.stop();
        tooltipText.text = msg;
        if (duration && duration > 0) {
            timerTooltipHider.interval = duration;
            timerTooltipHider.start();
        }
        if (tooltip.visible == false) {
            tooltip.visible = true
            animTooltip.to = "400"
            animTooltip.start()
        }
    }

    LabelEx {
        id: tooltipText
        anchors.fill: parent
        elide: Text.ElideMiddle
        verticalAlignment: Qt.AlignVCenter
        horizontalAlignment: Qt.AlignHCenter
    }

    Timer {
        id: timerTooltipHider
        interval: 1000
        repeat: false;
        running: false
        onTriggered: {
            animTooltip.to = "0"
            animTooltip.start()
        }
    }
    PropertyAnimation {
        id: animTooltip; target: tooltip; properties: "width";  duration: 200;
        onRunningChanged: {
            if (!running && tooltip.width == 0) {
                tooltip.visible = false;
            }
        }
    }
}


