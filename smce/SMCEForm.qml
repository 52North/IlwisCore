import QtQuick 2.0
import LayersView 1.0
import "../../../qml/Global.js" as Global

Item {
    width: parent.width
    height: parent.height

    Rectangle {
        width: parent.width
        height: parent.height
        color : "#F0FFF0"

        Loader {
            id : formEditor
            width : parent.width
            height : parent.height
            source: "SMCEDefForm.qml"
        }
    }

    function setForm(mode){
        if (mode === "defMode")
            formEditor.setSource("SMCEDefForm.qml")
        else if (mode === "evalMode")
            formEditor.setSource("SMCEEvalForm.qml")
        else { // the future analysis mode.....

        }
    }

    function smcemodeChanged(msg) {
        console.log("ModeChanged was called with: " + msg)
        setForm(msg)
    }


    function updateNodeInfo(msg) {
        console.log("SMCEForm.qml: updateNodeInfo was called with: " + msg)
        formEditor.item.updateNodeInfo(msg)
    }



    Component.onCompleted: {

    }
}
