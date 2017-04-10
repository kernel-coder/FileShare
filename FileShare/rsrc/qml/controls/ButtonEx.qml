import QtQuick.Controls 1.1
import QtQuick 2.3

Button {
    id: btn
    property bool down: pressed || (checkable && checked)
    property string sTag: ""
    property int    iTag: -1
    property bool isRadioMode: false

    signal clicked2(Button sender)
    signal toggled(Button sender, bool isOn)

    onClicked: {
        btn.clicked2(btn);
        if (checkable) {
            if (!isRadioMode) {
                checked = !checked
                btn.toggled(btn, checked)
            }
        }
    }
}
