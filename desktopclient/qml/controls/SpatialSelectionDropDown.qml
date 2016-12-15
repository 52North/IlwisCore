import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0

import "../Global.js" as Global

Item {
    id : sdp
    height : 22 + mapitem.height
    property alias envelope : topline.envelope
    property alias readOnly :topline.readOnly

    property alias labelText : topline.label

    function filter(type, value){
        envelope = value
    }

    SpatialSelection{
        id : mapitem
        height : 0
        width : parent.width - 20
        z : 1
        y : 22
        filterTarget: sdp

        onHeightChanged: {
            sdp.height = 22 + height
            if ( height % 10 == 0)
                currentEnvelope = "entiremap"
        }
    }
    LatLonEnvelopeLine {
        label : qsTr("Envelope")
        labelWidth: 80
        id : topline
        height : 22
        readOnly: false
        Image {
            id : marker
            width : 25
            fillMode : Image.Pad
            source : "../images/bookmark_close.png"
            rotation: 90
            y : 3
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if ( marker.rotation == 90)    {
                        marker.state = "open"
                        marker.source = "../images/bookmark_open.png"
                        mapitem.state = "visible"
                    }else{
                        marker.state = "closed"
                        marker.source = "../images/bookmark_close.png"
                        mapitem.state = "invisible"
                    }
                }
            }
            states: [
                State { name: "closed"

                    PropertyChanges {
                        target: marker
                        rotation : 90
                    }

                },
                State {
                    name : "open"
                    PropertyChanges {
                        target: marker
                        rotation : 0
                    }
                }

            ]
            transitions: [
                Transition {
                    NumberAnimation
                    { id : opatrans; properties: "rotation"; duration : 500 ; easing.type: Easing.OutCubic
                    }

                }
            ]

        }

    }
}
