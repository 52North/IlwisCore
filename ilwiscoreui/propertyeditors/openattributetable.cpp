#include "coverage.h"
#include "table.h"
#include "models/visualattributemodel.h"
#include "openattributetable.h"

REGISTER_PROPERTYEDITOR("viewattributes",OpenAttributeTable)

OpenAttributeTable::OpenAttributeTable(QObject *parent) : VisualAttributeEditor("viewattributes",TR("View Attributes"),QUrl("ViewAttributes.qml"), parent)
{
}

bool OpenAttributeTable::canUse(const IIlwisObject &obj, const QString &name) const
{
    if (!obj.isValid())
        return false;
    if(!hasType(obj->ilwisType(), itCOVERAGE))
        return false;
    Ilwis::ICoverage cov = obj.as<Coverage>();

    return name == VisualAttributeModel::LAYER_ONLY && cov->hasAttributes();
}

QStringList OpenAttributeTable::attributeTable() const
{
    QStringList result;
    if ( attribute()->layer()){
        if(attribute()->layer()->coverage()->hasAttributes() ){
            Resource res;
            if ( attribute()->layer()->coverage()->ilwisType() == itFEATURE)
                res = attribute()->layer()->coverage()->resource();
            else
                res = attribute()->layer()->coverage()->attributeTable()->resource();
            result.append(QString::number(res.id()));
            result.append(res.url().toString());

        }
    }
    return result;
}

VisualAttributeEditor *OpenAttributeTable::create()
{
    return new OpenAttributeTable();
}
