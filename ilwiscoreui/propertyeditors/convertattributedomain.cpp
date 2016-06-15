#include <QSqlQuery>
#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "symboltable.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operationExpression.h"
#include "operation.h"
#include "table.h"
#include "models/visualattributemodel.h"
#include "convertattributedomain.h"

REGISTER_PROPERTYEDITOR("convertattributedomain",ConvertAttributeDomain)

ConvertAttributeDomain::ConvertAttributeDomain(QObject *parent) : VisualAttributeEditor("convertattributedomain",TR("Convert domain"),QUrl("ConvertAttributeDomain.qml"), parent)
{
}

bool ConvertAttributeDomain::canUse(const IIlwisObject &obj, const ColumnDefinition &def) const{
    if ( !def.isValid() || !def.datadef().domain().isValid())
        return false;
    if ( !hasType(def.datadef().domain()->ilwisType(), itTEXTDOMAIN | itNUMERICDOMAIN | itITEMDOMAIN))
        return false;
    return true;
}

void ConvertAttributeDomain::prepare( VisualAttributeModel *vattrib, const IIlwisObject &obj, const ColumnDefinition &coldef)
{
    VisualAttributeEditor::prepare(vattrib, obj, coldef);
    _attributeDef = coldef;

}

bool ConvertAttributeDomain::execute(const QString &targetDomainType, const QString &domainName, const QString& colorScheme, const QString& viewerId)
{
    try{
        QString attrib = attribute()->layer()->activeAttribute();
        QString expr = QString("convertcolumndomain(%1,%2,%3)").arg(attribute()->layer()->url()).arg(attrib).arg(targetDomainType);

        Ilwis::OperationExpression ex(expr);
        Ilwis::Operation op(ex);
        ExecutionContext ctx;
        SymbolTable tbl;
        if (op->execute(&ctx, tbl)){

               attribute()->layer()->updateAttribute(attrib, colorScheme);
               return true;
        }
    } catch(const ErrorObject& err){
    }
    return false;
}

QStringList ConvertAttributeDomain::allowedDomains() const
{
    return  QStringList({"identifier","thematic","time","float","integer","color"});
}

int ConvertAttributeDomain::domainTypeIndex()
{
    if ( _attributeDef.isValid()){
        IDomain dom = _attributeDef.datadef().domain();
        if ( hasType(dom->valueType(), itIDENTIFIERITEM|itINDEXEDITEM))
            return 0;
        if ( hasType(dom->valueType(), itTHEMATICITEM))
            return 1;
        if ( hasType(dom->valueType(), itTIME))
            return 2;
        if ( hasType(dom->valueType(), itINTEGER))
            return 3;
        if ( hasType(dom->valueType(), itFLOAT|itDOUBLE))
            return 4;
        if ( hasType(dom->valueType(), itPALETTECOLOR))
            return 5;

    }
    return 0;
}

QStringList ConvertAttributeDomain::colorSchemes() const{
    QStringList names;
    InternalDatabaseConnection db;
    QString query = QString("Select code from representation where representationtype = 'palettecolor'");
    if (db.exec(query)) {
        while ( db.next()){
            names.append(db.value("code").toString());
        }
    }
    return names;
}

VisualAttributeEditor *ConvertAttributeDomain::create(){
    return new ConvertAttributeDomain();
}

