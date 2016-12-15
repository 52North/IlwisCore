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
        }


        // 1. Node detail


        // 2. Standardization form


        // 3. Standardization Preview
    }

    function setForm(){
        if (mode === "definition")
            formEditor.setSource("SMCEDefForm.qml")
        else if (mode === "definition")
            formEditor.setSource("SMCEEvalForm.qml")
        else { // the future analysis mode.....

        }
    }

    function smcemodeChanged() {
        console.log("ModeChanged was called")
        setForm()
    }

    Component.onCompleted: {
        //formEditor.setSource("SMCEDefForm.qml")
        formEditor.setSource("SMCEEvalForm.qml")
        //SMCEPanel.modeChanged.connect(smcemodeChanged)
        //setForm()
    }
}
