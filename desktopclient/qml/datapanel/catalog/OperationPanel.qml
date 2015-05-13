import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import CatalogModel 1.0
import MasterCatalogModel 1.0
import CatalogFilterModel 1.0
import OperationCatalogModel 1.0
import "../../Global.js" as Global
import "../../controls" as Controls

Item {
    property string tabLocation : "left"
    property CatalogModel currentCatalog
    property bool canSeparate : false
    property string panelType : "spatial-operations"
    id : operationViews
    width : parent.width
    height : parent.height

    signal catalogChanged()

    function setResources(){
        if ( currentCatalog)
            return currentCatalog.resources
    }

    Rectangle{
        id : toolbar
        anchors.top : parent.top
        width : parent.width
        height : 26
        border.width: 1
        border.color: "grey"
        color : Global.alternatecolor5

        ComboBox {
            height : 22
            width : 200
            anchors.verticalCenter: parent.verticalCenter
            model : operations.keywords
            onCurrentIndexChanged: {
                if ( currentCatalog){
                    var filterString="keyword='" + model[currentIndex] + "'"
                    currentCatalog.filter(filterString)
                }
            }
        }
    }

    Item {
        id : catalogView
        width : parent.width
        anchors.top: toolbar.bottom
        anchors.bottom: parent.bottom
        CatalogGrid{
            id : iconGridView
            height : parent.height
            opacity : 1
        }
        CatalogTable{
            id : iconListView
            opacity : 0
            height : 0
            enabled : false
        }
    }

}

