import QtQuick 2.1

Item {
    id : catalogPanels
    width : parent.width
    Connections{
        target : iconListView
        onShowObject : { showObject(objectid,"")}
    }

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
    CatalogThumbGrid{
        id : thumbListView
        height : 0
        enabled : false
        opacity: 0

    }
    CatalogMap{
           id : catalogMapView
           height : 0
           enabled : false
           opacity: 0
       }

    states: [
        State {
            name : "iconGrid"
            PropertyChanges { target: thumbListView; height : 0; opacity : 0; enabled : false}
            PropertyChanges { target: iconListView; height : 0; opacity : 0;enabled : false}
            PropertyChanges { target: catalogMapView; height : 0; opacity : 0; enabled : false}
            PropertyChanges { target: iconGridView; height : parent.height; opacity : 1;enabled : true}
        },
        State {
            name : "thumbList"
            PropertyChanges { target: thumbListView; height : parent.height;opacity : 1; enabled : true}
            PropertyChanges { target: iconListView; height :0; opacity : 0;enabled : false}
            PropertyChanges { target: iconGridView;  height : 0; opacity : 0;enabled : false}
            PropertyChanges { target: catalogMapView; height : 0; opacity : 0; enabled : false}
        },
        State {
            name : "iconList"
            PropertyChanges { target: thumbListView; height : 0; opacity : 0;enabled : false}
            PropertyChanges { target: iconListView; height : parent.height;opacity : 1; enabled : true}
            PropertyChanges { target: iconGridView;  height : 0; opacity : 0;enabled : false}
            PropertyChanges { target: catalogMapView; height : 0; opacity : 0; enabled : false}
        },
        State {
            name : "bylocation"
            PropertyChanges { target: thumbListView; height : 0; opacity : 0;enabled : false}
            PropertyChanges { target: iconListView; height :0; opacity : 0;enabled : false}
            PropertyChanges { target: iconGridView;  height : 0; opacity : 0;enabled : false}
            PropertyChanges { target: catalogMapView; height : parent.height;opacity : 1; enabled : true}
        },
        State {
                name : "sized"
                PropertyChanges { target: catalogPanels; height : parent.height - 200}
            },
            State {
                name : "maxed"
                PropertyChanges { target: catalogPanels; height : parent.height - 25}
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
            ParallelAnimation {
                NumberAnimation { target : catalogPanels; properties: "height"; duration : 750 ; easing.type: Easing.InOutCubic }
             }
        }
    ]
}

