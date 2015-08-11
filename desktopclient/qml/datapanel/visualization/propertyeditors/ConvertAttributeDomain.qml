import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1

Rectangle {
    width: 300
    height: parent ? parent.height - 10 : 0
    property var editor

    Column {
        id : col
        y : 8
        width : 200
        height : childrenRect.height + 10
        spacing : 4
        Text {
            text : qsTr("Domain type")
            width : 18
        }
        ComboBox{
            id : targets
            width : 200
            height : 20
            model :  ["identifier","thematic","time","float","integer","color"]
        }
        Text {
            text : qsTr("Color scheme")
            width : 18
        }

        ComboBox{
            id : colorscheme
            width : 200
            height : 20
            model :  editor.colorSchemes
        }
        Row {
            height : 20
            width : 200
            spacing : 40
            CheckBox{
                id : check
                width : 60
                height : 20
                checked : false
                text : qsTr("Domain name")
            }
            TextField{
                id : domainName
                width : 100
                height : 20
                enabled : check.checked
                opacity : check.checked ? 1 : 0.25
            }
        }

    }
    Button{
        width : 80
        height : 22
        text : qsTr("Apply")
        anchors.right : col.right
        //anchors.rightMargin:
        anchors.top : col.bottom
        onClicked: {
            var attrIndex = attributeListColumn.currentIndex
            if (editor.execute(targets.currentText, domainName.text, colorscheme.currentText,renderer.viewerId)){
                attributeListColumn.currentIndex = attrIndex
            }
        }
    }


}


