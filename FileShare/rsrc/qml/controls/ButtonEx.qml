import QtQuick.Controls 1.1
import QtQuick 2.3

Button {
    id: btn
    property bool down: pressed || (checkable && checked)
    property string sTag: ""
    property int    iTag: -1

    signal clicked2(Button sender);

    onClicked: {
        btn.clicked2(btn);
    }
}
