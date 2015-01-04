#include "kernel.h"

#include "uicontextmodel.h"

std::unique_ptr<UIContextModel> UIContextModel::_uicontext;

UIContextModel::UIContextModel(QObject *parent) :
    QObject(parent)
{
}

std::map<QString, CreatePropertyEditor> UIContextModel::propertyEditors(quint64 objecttype) const
{
    auto iter = _propertyEditors.find(objecttype);
    if ( iter != _propertyEditors.end()){
        return (*iter).second;
    }
    return std::map<QString, CreatePropertyEditor>();
}

void UIContextModel::init()
{

}

void UIContextModel::addPropertyEditor(quint64 objecttype, const QString &propertyName, CreatePropertyEditor createFunction)
{
    auto iter = _propertyEditors.find(objecttype);
    if ( iter == _propertyEditors.end()){
            _propertyEditors[objecttype][propertyName] = createFunction;
    }
}



std::unique_ptr<UIContextModel>& uicontext() {
    if ( UIContextModel::_uicontext.get() == 0) {
        UIContextModel::_uicontext.reset( new UIContextModel());
        UIContextModel::_uicontext->init();

    }
    return UIContextModel::_uicontext;
}
