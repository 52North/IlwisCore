#include "domainitemmodel.h"

DomainItemModel::DomainItemModel()
{

}

DomainItemModel::DomainItemModel(Ilwis::SPItemRange range, const QString& name, QObject *parent) :
    QObject(parent), _range(range), _itemname(name)
{
}

QString DomainItemModel::name() const
{
    if ( !_range.isNull())    {
        auto item = _range->item(_itemname);
        if ( !item.isNull())
            return _range->item(_itemname)->name();
    }
    return "";
}

QString DomainItemModel::code() const
{
    if (hasType(_range->valueType(), itTHEMATICITEM | itNUMERICITEM)){
        Ilwis::ThematicRange *thematicrange =  _range->as<Ilwis::ThematicRange>();
        if ( thematicrange){
            Ilwis::SPDomainItem item = thematicrange->item(_itemname);
            if ( !item.isNull()){
                QString codestr = thematicrange->item(_itemname)->as<Ilwis::ThematicItem>()->code();
                if ( codestr != sUNDEF)
                    return codestr;
            }

        }
    }
    return "";
}

QString DomainItemModel::description() const
{
    if ( hasType(_range->valueType(), itTHEMATICITEM | itNUMERICITEM)){
        Ilwis::ThematicRange *thematicrange =  _range->as<Ilwis::ThematicRange>();
        if ( thematicrange){
            Ilwis::SPDomainItem item = thematicrange->item(_itemname);
            if ( !item.isNull())
                return thematicrange->item(_itemname)->as<Ilwis::ThematicItem>()->description();
        }
    }
    return "";
}
