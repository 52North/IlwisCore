import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Dialogs 1.1
import WorkflowErrorModel 1.0
import ErrorModel 1.0

Rectangle {
    width: parent.width
    height: parent.height
    state: "smaller"

    function addError(id, error) {
        errorWrapper.errorModel.addError(id, error)
    }

    WorkflowErrorModel {
        id: errorWrapper

        errorModel.onErrorlistChanged: {

            if(errorModel.getCount() != 0)
            {
                if(errorview.state == "smaller")
                {
                    errorview.state = "bigger"
                }
            }
            else
            {
                if(errorview.state == "bigger")
                {
                    errorview.state = "smaller"
                }
            }
        }
    }

    ListView {
        model: errorWrapper.errorModel.errorlist
        id:errors
        anchors.fill: parent
        clip: true

        delegate: Text {
            text: modelData
            width: parent.width
            height: 20
            color: "red"
        }
    }
}

