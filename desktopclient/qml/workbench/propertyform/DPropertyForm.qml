import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import IlwisObjectModel 1.0
import QtQuick 2.0
import "../.." as Base


Component {
    id : propertyFormDelegate


    Item {
        function iconsource(name) {
            if ( name.indexOf("/") !== -1)
                return name

            var iconP = "../../images/" + name
            return iconP
        }

        y : 4
        id : propertyForm
        height :410
        width : parent.width

        Component {
            id : spatialData
            Loader{
                source : "SpatialProperties.qml"
            }
        }

        Rectangle {
            id : header
            width : parent.width
            height : 24
            color : "steelblue"
            Text {
                x : 5
                anchors.verticalCenter: parent.verticalCenter
                text : displayName
                color : "white"
                font.pointSize: 10
                font.bold: true
                width : parent.width - typeico.width - 9
                clip : true
                elide: Text.ElideMiddle
            }
            Image {
                id : typeico
                source : iconsource(iconPath)
                width : 20
                height : 20
                anchors.right: parent.right
                anchors.rightMargin: 5
                anchors.verticalCenter: parent.verticalCenter

            }
        }

        TabView {
            anchors.top : header.bottom
            anchors.topMargin: 3
            width : propertyForm.width
            height : propertyForm.height - header.height
            style: Base.TabStyle1{}
            Tab {
                title : qsTr("General")
                GeneralPropertyPane{}
            }
            Tab {
                title : qsTr("Data")
                DataPropertyPane{}
            }
            Component.onCompleted: {
                if(typeName == "rastercoverage"  || typeName == "feature" ||
                        typeName == "featurecoverage" || typeName == "pointcoverage" ||
                        typeName == "linecoverage" || typeName == "polygoncoverage"){
                    addTab("Spatial", spatialData)
                }
            }
        }
    }
}
