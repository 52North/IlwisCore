import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0

import MasterCatalogModel 1.0
import CatalogModel 1.0
import ResourceModel 1.0
import IlwisObjectModel 1.0

Menu {
    id : contexMenu
    title: "Edit"
    property var ilwisobject


    Instantiator {
        model: ["ff","gg","hh"]
        onObjectAdded: contexMenu.insertItem( index, object )
        onObjectRemoved: contexMenu.removeItem( object )
        MenuItem {
            text: modelData
        }
    }

    Menu {
        id : submenu
        title :  "s"
        Instantiator {
            model: ["a","b"]
            onObjectAdded: submenu.insertItem( index, object )
            onObjectRemoved: submenu.removeItem( object )
            MenuItem {
                text: modelData

            }
        }

    }

}


