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

    QString name = Identity::newAnonymousName();
    QString stmt = QString("script %1=%2 + %3").arg(name).arg(raster1->name()).arg(raster2->name());
    return doRasterOperation(stmt);
}

IRasterCoverage operator<(const IRasterCoverage &raster1, double v)
{

    QString name = Identity::newAnonymousName();
    QString stmt = QString("script %1=%2 < %3").arg(name).arg(raster1->name()).arg(v);
    return doRasterOperation(stmt);
}

IRasterCoverage operator/(const IRasterCoverage &raster1, const IRasterCoverage &raster2)
{

    QString name = Identity::newAnonymousName();;
    QString stmt = QString("script %1=%2 / %3").arg(name).arg(raster1->name()).arg(raster2->name());
    return doRasterOperation(stmt);
}

IRasterCoverage operator-(const IRasterCoverage &raster1, const IRasterCoverage &raster2)
{

    QString name = Identity::newAnonymousName();;
    QString stmt = QString("script %1=%2 - %3").arg(name).arg(raster1->name()).arg(raster2->name());
    return doRasterOperation(stmt);
}

IRasterCoverage operator*(const IRasterCoverage &raster1, const IRasterCoverage &raster2)
{

    QString name = Identity::newAnonymousName();;
    QString stmt = QString("script %1=%2 * %3").arg(name).arg(raster1->name()).arg(raster2->name());
    return doRasterOperation(stmt);
}

IRasterCoverage operator+(const IRasterCoverage &raster1, double number)
{
    QString name = Identity::newAnonymousName();;
    QString stmt = QString("script %1=%2 + %3").arg(name).arg(raster1->name()).arg(number);
    return doRasterOperation(stmt);
}

IRasterCoverage operator/(const IRasterCoverage &raster1, double number)
{
    QString name = Identity::newAnonymousName();;
    QString stmt = QString("script %1=%2 / %3").arg(name).arg(raster1->name()).arg(number);
    return doRasterOperation(stmt);
}

IRasterCoverage operator/(double number,const IRasterCoverage &raster1)
{
    QString name = Identity::newAnonymousName();;
    QString stmt = QString("script %1=%2 / %3").arg(name).arg(number).arg(raster1->name());
    return doRasterOperation(stmt);
}

IRasterCoverage operator-(const IRasterCoverage &raster1, double number)
{
    QString name = Identity::newAnonymousName();;
    QString stmt = QString("script %1=%2 - %3").arg(name).arg(raster1->name()).arg(number);
    return doRasterOperation(stmt);
}

IRasterCoverage operator-(double number,const IRasterCoverage &raster1)
{
    QString name = Identity::newAnonymousName();;
    QString stmt = QString("script %1=%2 - %3").arg(name).arg(number).arg(raster1->name());
    return doRasterOperation(stmt);
}

IRasterCoverage operator*(const IRasterCoverage &raster1, double number)
{
    QString name = Identity::newAnonymousName();;
    QString stmt = QString("script %1=%2 * %3").arg(name).arg(raster1->name()).arg(number);
    return doRasterOperation(stmt);
}


Ilwis::IRasterCoverage sin(const Ilwis::IRasterCoverage &raster1)
{
    QString name = Identity::newAnonymousName();;
    QString stmt = QString("script %1=sin(%2)").arg(name).arg(raster1->name());
    return doRasterOperation(stmt);
}

Ilwis::IRasterCoverage cos(const Ilwis::IRasterCoverage &raster1)
{
    QString name = Identity::newAnonymousName();;
    QString stmt = QString("script %1=cos(%2)").arg(name).arg(raster1->name());
    return doRasterOperation(stmt);
}

Ilwis::IRasterCoverage tan(const Ilwis::IRasterCoverage &raster1)
{
    QString name = Identity::newAnonymousName();;
    QString stmt = QString("script %1=tan(%2)").arg(name).arg(raster1->name());
    return doRasterOperation(stmt);
}

Ilwis::IRasterCoverage asin(const Ilwis::IRasterCoverage &raster1)
{
    QString name = Identity::newAnonymousName();;
    QString stmt = QString("script %1=asin(%2)").arg(name).arg(raster1->name());
    return doRasterOperation(stmt);
}

Ilwis::IRasterCoverage atan(const Ilwis::IRasterCoverage &raster1)
{
    QString name = Identity::newAnonymousName();;
    QString stmt = QString("script %1=atan(%2)").arg(name).arg(raster1->name());
    return doRasterOperation(stmt);
}

Ilwis::IRasterCoverage acos(const Ilwis::IRasterCoverage &raster1)
{
    QString name = Identity::newAnonymousName();;
    QString stmt = QString("script %1=acos(%2)").arg(name).arg(raster1->name());
    return doRasterOperation(stmt);
}

Ilwis::IRasterCoverage sinh(const Ilwis::IRasterCoverage &raster1)
{
    QString name = Identity::newAnonymousName();;
    QString stmt = QString("script %1=sinh(%2)").arg(name).arg(raster1->name());
    return doRasterOperation(stmt);
}

Ilwis::IRasterCoverage cosh(const Ilwis::IRasterCoverage &raster1)
{
    QString name = Identity::newAnonymousName();;
    QString stmt = QString("script %1=cosh(%2)").arg(name).arg(raster1->name());
    return doRasterOperation(stmt);
}

Ilwis::IRasterCoverage log10(const Ilwis::IRasterCoverage &raster1)
{
    QString name = Identity::newAnonymousName();;
    QString stmt = QString("script %1=log10(%2)").arg(name).arg(raster1->name());
    return doRasterOperation(stmt);
}

Ilwis::IRasterCoverage ln(const Ilwis::IRasterCoverage &raster1)
{
    QString name = Identity::newAnonymousName();;
    QString stmt = QString("script %1=ln(%2)").arg(name).arg(raster1->name());
    return doRasterOperation(stmt);
}

Ilwis::IRasterCoverage floor(const Ilwis::IRasterCoverage &raster1)
{
    QString name = Identity::newAnonymousName();;
    QString stmt = QString("script %1=floor(%2)").arg(name).arg(raster1->name());
    return doRasterOperation(stmt);
}

Ilwis::IRasterCoverage ceil(const Ilwis::IRasterCoverage &raster1)
{
    QString name = Identity::newAnonymousName();;
    QString stmt = QString("script %1=ceil(%2)").arg(name).arg(raster1->name());
    return doRasterOperation(stmt);
}

Ilwis::IRasterCoverage sign(const Ilwis::IRasterCoverage &raster1)
{
    QString name = Identity::newAnonymousName();;
    QString stmt = QString("script %1=sign(%2)").arg(name).arg(raster1->name());
    return doRasterOperation(stmt);
}

Ilwis::IRasterCoverage sqrt(const Ilwis::IRasterCoverage &raster1)
{
    QString name = Identity::newAnonymousName();;
    QString stmt = QString("script %1=sqrt(%2)").arg(name).arg(raster1->name());
    return doRasterOperation(stmt);
}
