#include "kernel.h"
#include "uicontextmodel.h"

UIContextModel::UIContextModel(QObject *parent) :
    QObject(parent)
{
}

void UIContextModel::currentKey(int keycode)
{
    _currentKey = keycode;
    if ( keycode == Qt::Key_Control || keycode == Qt::Key_Shift){
        _activeModifierKey = keycode;
    }

}

int UIContextModel::currentKey() const
{
    return _currentKey;
}

int UIContextModel::modifierKey() const
{
    return _activeModifierKey;
}

