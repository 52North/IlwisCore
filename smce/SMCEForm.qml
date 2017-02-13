import QtQuick 2.0
import LayersView 1.0
import "../../../qml/Global.js" as Global

Item {
    width: parent.width
    height: parent.height
    property string form: "SMCEDefForm.qml"
    property var selectedNode: null

    Rectangle {
        width: parent.width
        height: parent.height
        color : "#F0FFF0"

        Loader {
            id : formEditor
            width : parent.width
            height : parent.height
            source: form
        }
    }

    function updateForm() {
        formEditor.setSource(form)
    }

    function setForm(mode){
        if (mode === "defMode")
            form = "SMCEDefForm.qml"
        else if (mode === "evalMode")
            form = "SMCEEvalForm.qml"
        else { // the future analysis mode.....

        }
        updateForm()
    }

    function setSelectedNode(node) {
        selectedNode = node
        updateForm()
    }
}
