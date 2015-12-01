import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Dialogs 1.1
import WorkflowErrorModel 1.0
import ErrorModel 1.0

Rectangle {


    property variant errormodel: errorWrapper.errorModel;


    id: modellerErrorView
    width: parent.width

    WorkflowErrorModel {
        id: errorWrapper

        errorModel.onErrorlistChanged: {
            if(errorview.state == "smaller")
            {
                errorview.state = "bigger"
            }
        }
    }

    ListView {
        model: errormodel.errorlist
        id:errors
        width: parent.width
        height: parent.height

        delegate: Text {
            text: modelData
            width: 100
            height: 20
        }
    }
}

