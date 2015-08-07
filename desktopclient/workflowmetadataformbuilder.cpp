#include "kernel.h"
#include "mastercatalog.h"
#include "models/resourcemodel.h"

#include "workflowmetadataformbuilder.h"


using namespace Ilwis;

WorkflowMetadataFormBuilder::WorkflowMetadataFormBuilder()
{
}

QString WorkflowMetadataFormBuilder::createWorkflowForm(quint64 metaid) const
{
    // TODO auto create from workflow resource parameters
    Resource resource = mastercatalog()->id2Resource(metaid);


    QString hardcodedform;
    hardcodedform = "import QtQuick 2.2; import QtQuick.Controls 1.1;import QtQuick.Layouts 1.1;\
            Rectangle{\
                x: 20;\
                color : background1;\
                width : parent.width - 5;\
                anchors.margins : 20;\
                height : parent.height;\
                Column {\
                    spacing: 10;\
                    Rectangle{\
                        id: name_row;\
                        width : parent.width;\
                        height: 18;\
                        Text {\
                            id:label_name;\
                            text: qsTr('Name');\
                            wrapMode:Text.Wrap;\
                        }\
                        TextField{\
                            id : name;\
                            x : 60;\
                            Component.onCompleted: {\
                                text = 'type in name';\
                            }\
                            onTextChanged: {\
                                applicationForm.workflowname = text;\
                            }\
                        }\
                    }\
                    Rectangle {\
                        height: 36;\
                        id: description_row;\
                        anchors.top : name_row.bottom;\
                        anchors.margins : 20;\
                        Text {\
                            id:label_description;\
                            text: qsTr('Description');\
                            wrapMode:Text.Wrap;\
                        }\
                        TextArea{\
                            height: 60;\
                            id : description;\
                            x : 60;\
                            Component.onCompleted: {\
                                text = 'type in description';\
                            }\
                            onTextChanged: {\
                                applicationForm.workflowdescription= text;\
                            }\
                        }\
                    }\
                }\
            }\
";

            // TODO add a create button


    return hardcodedform;
}
