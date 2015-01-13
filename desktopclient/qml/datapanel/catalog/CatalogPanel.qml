import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import MasterCatalogModel 1.0

Item {
    property int heightButtons : 26
    property string tabLocation : "left"
    id : catalogViews
    width : parent.width
    height : parent.height


    signal catalogChanged()

    function showObject(objectid){
        mainsplit.showObject(objectid)
    }

    function toggleFilter(objecttype, togglestate){
        mastercatalog.setObjectFilterCurrentCatalog(objecttype, togglestate)
        catalogChanged()
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
        height : 35
        color : "white"

        Action {
            id :refreshCatalog
            onTriggered: {
                mastercatalog.refreshWorkingCatalog()
            }
        }

        RowLayout{
            spacing: 1
            anchors.verticalCenter: parent.verticalCenter
            ExclusiveGroup { id : catalogViewStatus
                onCurrentChanged: {
                    if( showList.checked)
                        catalogView.state = "iconList"
                    if ( showGrid.checked)
                        catalogView.state = "iconGrid"
                    if ( showThumbs.checked)
                        catalogView.state = "thumbList"
                }
            }

            ToolBarButton{
                id : showList
                iconSource: iconsource("listCS1.png")
                exclusiveGroup: catalogViewStatus
                checked : true
            }
            ToolBarButton{
                id : showGrid
                iconSource: iconsource("gridCS1.png")
                checked : false
                exclusiveGroup: catalogViewStatus
            }
            ToolBarButton{
                id : showThumbs
                implicitHeight: heightButtons
                iconSource: iconsource("thumblistCS1.png")
                checkable: true
                checked: false
                exclusiveGroup: catalogViewStatus
            }

            Rectangle{
                implicitHeight: heightButtons
                width: 10
                color : "grey"
                opacity: 0
            }

            ToolBarButton{
                id : refresh
                implicitHeight: heightButtons
                iconSource: iconsource("refreshCS1.png")
                action : refreshCatalog
            }

            Rectangle{
                implicitHeight: heightButtons
                width: 20
                color : "grey"
                opacity: 0
            }

            CheckBox{
                id : allSelected
                checkedState: Qt.Checked
                implicitHeight: heightButtons
            }

            ToolBarButton{
                id : showRasters
                implicitHeight: heightButtons
                iconSource: iconsource("raster20CS1.png")
                checkable: true
                checked: true
                onClicked: toggleFilter("rastercoverage", checked);
            }
            ToolBarButton{
                id : showFeatures
                implicitHeight: heightButtons
                iconSource: iconsource("feature20CS1.png")
                checkable: true
                checked: true
                onClicked: toggleFilter("featurecoverage", checked);
            }
            ToolBarButton{
                id : showCoverages
                implicitHeight: heightButtons
                iconSource: iconsource("coverage20.png")
                checkable: true
                checked: true
                onClicked: toggleFilter("coverage", checked);
            }
            ToolBarButton{
                id : showTables
                implicitHeight: heightButtons
                iconSource: iconsource("table20CS1.png")
                checkable: true
                checked: true
                onClicked: toggleFilter("table", checked);
            }
            ToolBarButton{
                id : showCsys
                implicitHeight: heightButtons
                iconSource: iconsource("csy20.png")
                checkable: true
                checked: true
                onClicked: toggleFilter("coordinatesystem", checked);
            }
            ToolBarButton{
                id : showGrfs
                implicitHeight: heightButtons
                iconSource: iconsource("georeference20.png")
                checkable: true
                checked: true
                onClicked: toggleFilter("georeference", checked);
            }
            ToolBarButton{
                id : showDomains
                implicitHeight: heightButtons
                iconSource: iconsource("domain.png")
                checkable: true
                checked: true
                onClicked: toggleFilter("domain", checked);
            }
        }


    }
    Item {
        id : catalogView
        width : parent.width
        anchors.top: toolbar.bottom
        anchors.bottom: parent.bottom
        Connections{
            target : iconListView
            onShowObject : { showObject(objectid)}
        }

        CatalogGrid{
            id : iconGridView
            height : 0
            enabled : false
            opacity : 0
        }
        CatalogTable{
            id : iconListView
            height : parent.height
        }
        CatalogThumbGrid{
            id : thumbListView
            height : 0
            enabled : false
            opacity: 0

        }


        states: [
            State {
                name : "iconGrid"
                PropertyChanges { target: thumbListView; height : 0; opacity : 0; enabled : false}
                PropertyChanges { target: iconListView; height : 0; opacity : 0;enabled : false}
                PropertyChanges { target: iconGridView; height : parent.height; opacity : 1;enabled : true}
            },
            State {
                name : "thumbList"
                PropertyChanges { target: thumbListView; height : parent.height;opacity : 1; enabled : true}
                PropertyChanges { target: iconListView; height :0; opacity : 0;enabled : false}
                PropertyChanges { target: iconGridView;  height : 0; opacity : 0;enabled : false}
            },
            State {
                name : "iconList"
                PropertyChanges { target: thumbListView; height : 0; opacity : 0;enabled : false}
                PropertyChanges { target: iconListView; height : parent.height;opacity : 1; enabled : true}
                PropertyChanges { target: iconGridView;  height : 0; opacity : 0;enabled : false}
            }

        ]

        transitions: [
            Transition {
                ParallelAnimation{
                    NumberAnimation { target: thumbListView; properties: "height"; duration: 400; easing.type: Easing.InOutQuad }
                    NumberAnimation { target: thumbListView; properties: "opacity"; duration: 400; easing.type: Easing.InOutQuad }
                }
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

}
