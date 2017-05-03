import QtQuick 2.0
import LayersView 1.0
import "../../../qml/Global.js" as Global

Item {
    width: parent.width
    height: parent.height
    property string form: "SMCEDefForm.qml"
    property var selectedNode: null
    property var selectedItem: null

    Rectangle {
        width: parent.width
        height: parent.height
        color : "#F0FFF0"

        Loader {
            id : formEditor
            width : parent.width
            height : parent.height
        }
    }

    function setForm(mode) {
        var newForm
        if (mode === "defMode")
            newForm = "SMCEDefForm.qml"
        else if (mode === "evalMode")
            newForm = "SMCEEvalForm.qml"
        else { // the future analysis mode.....

        }
        if (form != newForm) {
            form = newForm
            if (selectedNode != null) {
                formEditor.setSource(form)
            }
        }
    }

    function setSelectedNode(node) {
        var change = ((node != null) && (selectedNode == null)) || ((node == null) && (selectedNode != null))
        if (change) {
            if (node == null) {
                if (formEditor.item)
                    formEditor.item.selNodeAboutToChange()
                formEditor.setSource("")
                selectedNode = node
            } else {
                selectedNode = node
                formEditor.setSource(form)
                if (formEditor.item)
                    formEditor.item.selNodeChanged()
            }
        } else {
            if (formEditor.item)
                formEditor.item.selNodeAboutToChange()
            selectedNode = node
            if (formEditor.item)
                formEditor.item.selNodeChanged()
        }
    }

    function setSelectedItem(item) {
        selectedItem = item
    }
}
