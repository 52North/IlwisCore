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
        id : ellipsoidList
    }

    id : ellipsoids
    labelText: qsTr("Ellipsoid")
    labelWidth: 140
    property var selectedObjectid
    property var items : []
    role : "text"

    onCurrentIndexChanged: {
        selectedObjectid =  ellipsoids.items[currentIndex].split("|")[0]
    }

    Component.onCompleted: {
        ellipsoids.items = mastercatalog.select("type=" + uicontext.typeName2typeId("ellipsoid"),"name")
        var defaultChoice = 0
        for(var i = 0; i < ellipsoids.items.length; ++i){
            var parts = ellipsoids.items[i].split("|")
            ellipsoidList.append({"text": parts[1]})
            if ( parts[1] === "Wgs 84"){
                defaultChoice = i
            }
        }
        itemModel = ellipsoidList
        currentIndex = defaultChoice

    }
}



