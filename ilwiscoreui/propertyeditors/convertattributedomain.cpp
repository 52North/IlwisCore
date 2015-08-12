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

QStringList ConvertAttributeDomain::colorSchemes() const{
    QStringList names;
    QSqlQuery db(kernel()->database());
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

