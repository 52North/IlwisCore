import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0

Rectangle {
    id : container
    anchors.fill : parent
    color : "white"
    Loader {
        anchors.fill: parent

    }
    Component.onCompleted: {
        if(typeName == "rastercoverage"  ){
            source = "RasterCoverageDataProperties.qml"
        }
        else if ( typeName == "feature" || typeName == "featurecoverage" ||
                typeName == "pointcoverage" || typeName == "linecoverage" ||
                typeName == "polygoncoverage"){
            source = "FeatureCoverageDataProperties.qml"
        }
        else if ( typeName == "coordinatesystem" || typeName == "conventionalcoordinatesystem" ||
                typeName == "boundsonlycoordinatesystem"){
            source = "CoordinateSystemProperties.qml"
        }
        else if ( typeName == "itemdomain" ){
            var valueType = getProperty("valuetype")
            if ( valueType !== "" && valueType !== "indexidentifier")
                source = "ItemDomainProperties.qml"
        }  else if ( typeName == "numericdomain" ){
            source = "NumericDomainProperties.qml"
        }

    }
}

