import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import ObjectCreator 1.0
import IlwisObjectCreatorModel 1.0
import UIContextModel 1.0
import "../Global.js" as Global
import "../controls" as Controls
import ".." as Base

Controls.ComboxLabelPair{

    ListModel {
        id : projectionList
    }

    id : projections
    labelText: qsTr("Projection")
    labelWidth: 120
    property var selectedObjectid
    property var items : []
    role : "text"

    onCurrentIndexChanged: {
        selectedObjectid =  projections.items[currentIndex].split("|")[0]
    }

    Component.onCompleted: {
        projections.items = mastercatalog.select("type=" + uicontext.typeName2typeId("projection"),"name")
        for(var i = 0; i < projections.items.length; ++i){
            var parts = items[i].split("|")
            projectionList.append({"text": parts[1]})
        }
        itemModel = projectionList
    }
}

