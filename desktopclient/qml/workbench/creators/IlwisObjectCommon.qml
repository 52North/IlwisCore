import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0
import IlwisObjectCreatorModel 1.0
import "../../Global.js" as Global
import "../../controls" as Controls
import "../.." as Base

Column {
    width : parent.width
    height : 65
    spacing : 4
    property alias itemname: namevalue.content
    property alias description : descvalue.content

    Controls.TextEditLabelPair{
        id : namevalue
        labelText: qsTr("Name")
        labelWidth: 120
        width : parent.width
    }
    Controls.TextAreaLabelPair{
        id : descvalue
        labelText: qsTr("Description")
        width : parent.width
        height : 40
        labelWidth: 120
    }
}

