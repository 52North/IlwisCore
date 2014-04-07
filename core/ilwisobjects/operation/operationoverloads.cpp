#include "kernel.h"
#include "raster.h"
#include "connectorinterface.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "operationoverloads.h"

using namespace Ilwis;

IRasterCoverage doRasterOperation(const QString& stmt){
    SymbolTable symtbl;
    ExecutionContext ctx;

    Operation opadd({stmt.toLatin1()});
    if ( opadd->execute(&ctx,symtbl)){
        if ( ctx._results.size() > 0) {
            QVariant var = symtbl.getValue(ctx._results[0]);
            IRasterCoverage mp = var.value<IRasterCoverage>();
            return mp;
        }

    }
    return IRasterCoverage();
}

IRasterCoverage operator+(const IRasterCoverage &raster1, const IRasterCoverage &raster2)
{

    QString name = ANONYMOUS_PREFIX;
    QString stmt = QString("script %1=%2 + %3").arg(name).arg(raster1->name()).arg(raster2->name());
    return doRasterOperation(stmt);
}

IRasterCoverage operator/(const IRasterCoverage &raster1, const IRasterCoverage &raster2)
{

    QString name = ANONYMOUS_PREFIX;
    QString stmt = QString("script %1=%2 / %3").arg(name).arg(raster1->name()).arg(raster2->name());
    return doRasterOperation(stmt);
}

IRasterCoverage operator-(const IRasterCoverage &raster1, const IRasterCoverage &raster2)
{

    QString name = ANONYMOUS_PREFIX;
    QString stmt = QString("script %1=%2 - %3").arg(name).arg(raster1->name()).arg(raster2->name());
    return doRasterOperation(stmt);
}

IRasterCoverage operator*(const IRasterCoverage &raster1, const IRasterCoverage &raster2)
{

    QString name = ANONYMOUS_PREFIX;
    QString stmt = QString("script %1=%2 * %3").arg(name).arg(raster1->name()).arg(raster2->name());
    return doRasterOperation(stmt);
}

IRasterCoverage operator+(const IRasterCoverage &raster1, double number)
{
    QString name = ANONYMOUS_PREFIX;
    QString stmt = QString("script %1=%2 + %3").arg(name).arg(raster1->name()).arg(number);
    return doRasterOperation(stmt);
}

IRasterCoverage operator/(const IRasterCoverage &raster1, double number)
{
    QString name = ANONYMOUS_PREFIX;
    QString stmt = QString("script %1=%2 / %3").arg(name).arg(raster1->name()).arg(number);
    return doRasterOperation(stmt);
}

IRasterCoverage operator-(const IRasterCoverage &raster1, double number)
{
    QString name = ANONYMOUS_PREFIX;
    QString stmt = QString("script %1=%2 - %3").arg(name).arg(raster1->name()).arg(number);
    return doRasterOperation(stmt);
}

IRasterCoverage operator*(const IRasterCoverage &raster1, double number)
{
    QString name = ANONYMOUS_PREFIX;
    QString stmt = QString("script %1=%2 * %3").arg(name).arg(raster1->name()).arg(number);
    return doRasterOperation(stmt);
}
