#include "workflowmetadataformbuilder.h"


WorkflowMetadataFormBuilder::WorkflowMetadataFormBuilder()
{
}

QString WorkflowMetadataFormBuilder::createWorkflowForm(quint64 metaid) const
{

    // TODO auto create from workflow resource parameters

    QString form;
    form = "import QtQuick 2.2; import QtQuick.Controls 1.1;import QtQuick.Layouts 1.1;\
            Column { \
            width : parent.width - 5;\
            height : parent.height;\
            spacing :10;\
            Rectangle{\
                height : 20;\
                width : parent.width;\
                color : background1;\
                Text {\
                    x:20 + 0;\
                    text: qsTr('Name');\
                    id:label_pin_0;\
                    width : 100 - 20 - 0;\
                    wrapMode:Text.Wrap\
                }\
                DropArea{\
                    x : 100;\
                    height : 20;\
                    width : parent.width - label_pin_0.width - 5 - 20 - 24 - 0;\
                    onDropped : {\
                        pin_0.text = drag.source.message\
                    }\
                    TextField{ \
                        id : pin_0; \
                        anchors.fill : parent;\
                        Component.onCompleted: {\
                            text = 'super workflow';\
                        }\
                        onTextChanged: {\
                            applicationForm.workflowname = text;\
                        }\
                    }\
                }\
            }\
        }";


    return form;
}
