import QtQuick 2.0

Column {
    id : workspaceinfo
    state : "zerosize"
    height: 0
    width : parent.width
    spacing : 5
    opacity : 0



    WorkSpaceDataGrid{

    }

    WorkSpaceOperationGrid{

    }

    WorkSpaceMetadata{

    }

    states: [
        State { name: "fullsize"

            PropertyChanges {
                target: workspaceinfo
                height : parent.height - 40
                opacity : 1
            }
        },
        State {
            name : "zerosize"
            PropertyChanges {
                target: workspaceinfo
                height : 0
                opacity : 0
            }
        }

    ]
    transitions: [
        Transition {
            NumberAnimation { properties: "height"; duration : 500 ; easing.type: Easing.InOutCubic }
            NumberAnimation { properties: "opacity"; duration : 500 ; easing.type: Easing.InOutCubic }
        }
    ]
}

