import QtQuick 2.2; import QtQuick.Controls 1.1;import QtQuick.Layouts 1.1;
            Column {
                width : parent.width - 5;
                height : parent.height;
                spacing: 10;
                Rectangle{
                    x: 20;
                    id: name_row;
                    height: 20;
                    width : parent.width;
                    color : background1;
                    Text {
                        id:label_name;
                        text: qsTr('Name');
                        wrapMode:Text.Wrap;
                    }
                    TextField{
                        id : name;
                        Component.onCompleted: {
                            text = 'type in name';
                        }
                        onTextChanged: {
                            applicationForm.workflowname = text;
                        }
                    }
                }
                Rectangle {
                    x: 20;
                    id: description_row;
                    width : parent.width;
                    height : label_name.height * 4;
                    Text {
                        id:label_description;
                        text: qsTr('Description');
                        wrapMode:Text.Wrap;
                    }
                    TextArea{
                        id : description;
                        Component.onCompleted: {
                            text = 'type in description';
                        }
                        onTextChanged: {
                            applicationForm.workflowdescription= text;
                        }
                    }
                }
            }
