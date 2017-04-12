import QtQuick 2.0

Rectangle {
    id: container
    property var __views: []
    property int currentIndex: -1
    readonly property int count : __views.length

    onCurrentIndexChanged: {
        for (var i = 0; i < __views.length; i++) {
            __views[i].visible = i == currentIndex
        }
    }

    function addView(view) {
        view.parent = container
        view.anchors.fill = view.parent
        return __views.push(view)
    }
    function removeAt(index) {
        var view = null;
        if (index >= 0 && index < __views.length) {
            view = __views[index];
            view.parent = null
            _views.splice(index, 1)
        }
        return view;
    }
    function viewAt(index) {
        var view = null;
        if (index >= 0 && index < __views.length) {
            view = __views[index];
        }
        return view;
    }
}
