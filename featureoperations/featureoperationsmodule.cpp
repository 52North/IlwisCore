#include <QtPlugin>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include "kernel.h"
#include "symboltable.h"
#include "OperationExpression.h"
#include "featureoperationsmodule.h"
#include "ilwisdata.h"
#include "coverage.h"
#include "columndefinition.h"
#include "table.h"
#include "attributerecord.h"
#include "feature.h"
#include "domain.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "gridding.h"


using namespace Ilwis;
using namespace FeatureOperations;

FeatureOperationsModule::FeatureOperationsModule(QObject *parent) :
    Module(parent, "FeatureOperations", "iv40","1.0")
{
}

QString FeatureOperationsModule::getInterfaceVersion() const
{
    return "iv40";

}

void FeatureOperationsModule::prepare()
{
   commandhandler()->addOperation(Gridding::createMetadata(), Gridding::create);

}

QString FeatureOperationsModule::name() const
{
    return "Feature Operations plugin";
}

QString FeatureOperationsModule::version() const
{
    return "1.0";
}






