import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import IlwisObjectModel 1.0
import QtQuick 2.0
import "../.." as Base
import "../../Global.js" as Global


Rectangle {
    id : propertyForm
    property var editValues : []
    property bool editable : false
    function iconsource(name) {
        if ( name.indexOf("/") !== -1)
            return name

        var iconP = "../../images/" + name
        return iconP
    }
    color : propertyForm.editable ? "#DCDCDC" : uicontext.lightestColor

    function storeData() {
        if ( editable){
            for(var i = 0; i < propertyTabs.count; i = i + 1){
                var tab = propertyTabs.getTab(i)
                if ( tab && tab.item && tab.item.item){
                    tab.item.item.storeData()
                }
            }
        }
    }

    y : 4
    x : 4
    height :464
    width : parent.width

    Component {
        id : spatialData
        Loader{
            source : "SpatialProperties.qml"
        }
    }

    Component {
        id : projectionData
        Loader{
            source : "ProjectionPropertiesAll.qml"
        }
    }

    Component {
        id : generalData
        Loader {
            source : "GeneralPropertyPane.qml"
        }
    }

    Component {
        id : dataData
        Loader {
            source : "DataPropertyPane.qml"
        }
    }

    Rectangle {

        id : header
        width : parent.width
        height : 24
        color : uicontext.darkColor
        Image {
            id : typeico
            source : iconsource(iconPath)
            width : 21
            height : 21
            anchors.left: parent.left
            anchors.leftMargin: 5
            anchors.verticalCenter: parent.verticalCenter

        }

        Text {
            x : 5
            anchors.verticalCenter: parent.verticalCenter
            anchors.left : typeico.right
            anchors.leftMargin: 5
            text : displayName
            color : "white"
            font.pointSize: 10
            font.bold: true
            width : parent.width - typeico.width - 10
            clip : true
            elide: Text.ElideMiddle
        }

    }

    TabView {
        id : propertyTabs
        anchors.top : header.bottom
        anchors.topMargin: 3
        width : propertyForm.width - 4
        height : propertyForm.height - header.height - 35
        style: Base.TabStyle1{}
        onCurrentIndexChanged: {
            props.lastIndex = currentIndex
        }

        Component.onCompleted: {
            addTab(qsTr("General"), generalData )
            addTab(qsTr("Data"), dataData)
            if(typeName == "rastercoverage"  || typeName == "feature" ||
                    typeName == "featurecoverage" || typeName == "pointcoverage" ||
                    typeName == "linecoverage" || typeName == "polygoncoverage"){
                addTab(qsTr("Spatial"), spatialData)
            } else if ( typeName == "projection" || typeName == "coordinatesystem" || typeName == "conventionalcoordinatesystem"){
                if ( isProjected){
                    addTab(qsTr("Projection"), projectionData)
                }
            }
            if ( props.lastIndex < count)
                currentIndex = props.lastIndex
        }
    }
    Loader {
        anchors.top : propertyTabs.bottom
        anchors.right : parent.right
        anchors.rightMargin: 9
        anchors.topMargin: 5
        source : isSystemObject ?  "" : "PropertyEditButtons.qml"
    }
}

