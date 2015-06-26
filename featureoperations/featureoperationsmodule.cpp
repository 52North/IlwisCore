#include <QtPlugin>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include "coverage.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "featureoperationsmodule.h"
#include "table.h"
#include "featurecoverage.h"
#include "feature.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "operationExpression.h"
#include "operationmetadata.h"
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

   kernel()->issues()->log("Loaded module FeatureOperations",IssueObject::itMessage);

}

QString FeatureOperationsModule::name() const
{
    return "Feature Operations plugin";
}

QString FeatureOperationsModule::version() const
{
    return "1.0";
}






