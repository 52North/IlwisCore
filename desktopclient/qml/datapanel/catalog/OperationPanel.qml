import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import CatalogModel 1.0
import MasterCatalogModel 1.0
import CatalogFilterModel 1.0
import OperationCatalogModel 1.0
import TabModel 1.0
import "../../Global.js" as Global
import "../../controls" as Controls

Item {
    property CatalogModel currentCatalog
    id : operationViews
    width : parent.width
    height : parent.height
    property TabModel tabmodel

    signal catalogChanged()

    function setResources(){
        if ( currentCatalog)
            return currentCatalog.resources
    }

    function addDataSource(filter, sourceName, sourceType){
        //empty implementation for the moment as it is called by datapanel
        //if in the future the code of onCompleted moves here(not needed atm) then
        // the setResources() method must fill the models of the Catalog representations here.
        // This call atm takes place before addDataSource() so the catalog will not be filled
        // if the stuff from onCompleted happens here
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
                    var filterString = "type='OperationMetaData'"
                    if (currentIndex != 0)
                        filterString += " and keyword='" + model[currentIndex] + "'"
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
    Component.onCompleted: {
        var url = mastercatalog.currentUrl
        currentCatalog = mastercatalog.newCatalog(url,"type='OperationMetaData'")
        if ( currentCatalog){
            currentCatalog.makeParent(operationViews)
            mastercatalog.currentCatalog = currentCatalog
        }
    }

}

