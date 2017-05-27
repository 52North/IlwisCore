import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import "../../Global.js" as Global

Rectangle {
    function storeData() {
        if ( propertyForm.editable){
            if ( dataTab.item)    {
                dataTab.item.storeData()
            }
        }
    }

    id : container
    anchors.fill : parent
         color : uicontext.lightestColor
    Loader {
        id : dataTab
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
            if ( valueType !== "" && valueType === "numericinterval"){
                source = "IntervalItemProperties.qml"
            }
            else if ( valueType !== "" && valueType !== "indexidentifier")
                source = "ItemDomainProperties.qml"
            else if ( valueType !== "" && valueType === "indexidentifier")
                source = "IndexedItemDomainProperties.qml"
        }
        else if ( typeName == "numericdomain" ){
            source = "NumericDomainProperties.qml"
        }
        else if ( typeName == "combinationmatrix" ){
            source = "CombinationMatrixProperties.qml"
        }
        else if ( typeName == "georeference"){
           source = "GeoreferenceProperties.qml"
        } else if ( typeName == "table" || typeName == "flattable"){
            source = "TableProperties.qml"
        } else if ( typeName == "ellipsoid"){
            source = "EllipsoidProperties.qml"
        } else if ( typeName == "singleoperation" || typeName == "workflow"){
            source = "OperationProperties.qml"
        } else if ( typeName == "catalog"){
            source = "CatalogProperties.qml"
        }

    }
}

