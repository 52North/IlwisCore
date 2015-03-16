import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Window 2.1
import LayerManager 1.0

Window {
    function transferLayers(layermanager){
        layerview.transferLayers(layermanager)
    }

    Visualize{
        id :layerview
    }
    x : 100
    y : 100

}

