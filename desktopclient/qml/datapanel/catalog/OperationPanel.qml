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
    property OperationCatalogModel currentCatalog
    id :    catalogViews
    width : parent.width
    height : parent.height
    property TabModel tabmodel

    signal catalogChanged()

    function setResources(){
        if ( currentCatalog){
            return currentCatalog.operations
        }
    }

    function addDataSource(filter, sourceName, sourceType){
        //empty implementation for the moment as it is called by datapanel
        //if in the future the code of onCompleted moves here(not needed atm) then
        // the setResources() method must fill the models of the Catalog representations here.
        // This call atm takes place before addDataSource() so the catalog will not be filled
        // if the stuff from onCompleted happens here
    }

    function iconsource(name) {
        if ( name.indexOf("/") !== -1)
            return name
        if ( name === "")
            name = "redbuttonr.png"

        var iconP = "../../images/" + name
        return iconP
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
            id : keywordselection
            height : 22
            width : 200
            anchors.verticalCenter: parent.verticalCenter
            model : operations.keywords
            onCurrentIndexChanged: {
                if ( currentCatalog){
                    var filterString = "(type='SingleOperation' or type='Workflow')"
                    if (currentIndex != 0)
                        filterString += " and keyword='" + model[currentIndex] + "'"
                    currentCatalog.filter(filterString)
                    catalogChanged()
                }
            }
        }
        OperationLayoutButtonBar{
            anchors.left: keywordselection.right
            anchors.leftMargin: 4
            width : 100
            height : 20
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
        OperationTable{
            id : iconListView
            opacity : 0
            height : 0
            enabled : false
        }
        states: [
            State {
                name : "iconGrid"
                PropertyChanges { target: iconListView; height : 0; opacity : 0;enabled : false}
                PropertyChanges { target: iconGridView; height : parent.height; opacity : 1;enabled : true}
            },
            State {
                name : "iconList"
                PropertyChanges { target: iconListView; height : parent.height;opacity : 1; enabled : true}
                PropertyChanges { target: iconGridView;  height : 0; opacity : 0;enabled : false}
            }
        ]

        transitions: [
            Transition {
                ParallelAnimation{
                    NumberAnimation { target: iconListView; property: "height"; duration: 400; easing.type: Easing.InOutQuad }
                    NumberAnimation { target: iconListView; property: "opacity"; duration: 400; easing.type: Easing.InOutQuad }
                }
                ParallelAnimation{
                    NumberAnimation { target: iconGridView; property: "height"; duration: 400; easing.type: Easing.InOutQuad }
                    NumberAnimation { target: iconGridView; property: "opacity"; duration: 400; easing.type: Easing.InOutQuad }
                }
            }
        ]
    }



    Component.onCompleted: {
        var url = mastercatalog.currentUrl
        currentCatalog = mastercatalog.newCatalog(url,"(type='SingleOperation' or type='Workflow')")
        if ( currentCatalog){
            currentCatalog.makeParent(catalogViews)
            mastercatalog.currentCatalog = currentCatalog
        }
    }

}

