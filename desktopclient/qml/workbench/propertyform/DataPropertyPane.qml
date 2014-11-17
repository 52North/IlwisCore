import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0

Rectangle {
    id : container
    anchors.fill : parent
    Loader {
        anchors.fill: parent

    }
    Component.onCompleted: {
        if(typeName == "rastercoverage"  ){
            source = "RasterCoverageDataProperties.qml"
        }
        if ( typeName == "feature" || typeName == "featurecoverage" ||
                typeName == "pointcoverage" || typeName == "linecoverage" ||
                typeName == "polygoncoverage"){
            source = "FeatureCoverageDataProperties.qml"
        }

    }
}

