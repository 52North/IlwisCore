#include <QString>
#include <QStringList>
#include "kernel.h"
#include "numericrange.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "inifile.h"
#include "rawconverter.h"
#include "ilwis3range.h"

using namespace Ilwis;
using namespace Ilwis3;

Ilwis3Range::Ilwis3Range()
{
}

Range *Ilwis3Range::findRange(const ODF &odf, const QString& section) {
    double vmin=-1e300, vmax=1e300;
    bool ok = true;
    QString minmax = odf->value(section,"MinMax");
    bool isInteger;

    if ( minmax != sUNDEF ) {
        ok = minMax2minMax(odf,minmax,vmin, vmax);
        if (!ok)
            return 0;
    }
    if ( vmin >= vmax) {  // if minmax is not defined we figure it out from the range, less precise but good enough
        QString range = odf->value(section,"Range");
        if ( range != sUNDEF ) {
            ok = range2MinMax(odf,range, vmin, vmax,isInteger);
            if (!ok)
                return 0;
        }
    }
    QString inf = odf->value(section,"DomainInfo");
    double resolution = 1;
    if ( inf != sUNDEF) {
        QStringList parts = inf.split(";");
        if ( parts.size() >= 6 && parts[4] != "") {
            QStringList vrparts = parts[4].split(":");
            if ( vmin >= vmax) { // even less precise but we have to do something
                vmin = vrparts[0].toDouble();
                vmax = vrparts[1].toDouble();
            }
            if( vrparts.size() >= 3 && vrparts[2].size() != 0) {
                resolution = 0; // TODO: find a better rule for resolution; maybe based on minmax;
            }

        }

    }
    if ( resolution == 1.0)
        return new NumericRange(vmin, vmax,1);
    return new NumericRange(vmin, vmax, resolution);

}

bool Ilwis3Range::minMax2minMax(const ODF &odf,const QString& minmax, double& vmin, double& vmax)  {
    bool ok;
    QStringList parts = minmax.split(":");
    if ( parts.size() >= 2) {
        vmin = parts[0].toDouble(&ok);
        if ( !ok) {
            kernel()->issues()->log(TR(ERR_INVALID_PROPERTY_FOR_2).arg("Maximum", odf->url()));
            return false;
        }
        vmax = parts[1].toDouble(&ok);
        if (!ok){
            kernel()->issues()->log(TR(ERR_INVALID_PROPERTY_FOR_2).arg("Minimum", odf->url()));
            return false;
        }
    }
    return true;
}
bool Ilwis3Range::range2MinMax(const ODF &odf, const QString &range, double& vmin, double &vmax, bool &isInteger)  {
    QStringList parts = range.split(":");
    isInteger = false;
    if ( parts.size() >= 2) {
        bool ok1, ok2;
        ok1 = ok2 = true;
        double v1 = parts[0].toDouble(&ok1);
        double v2 = parts[1].toDouble(&ok2);
        if ( ! ( ok1 && ok2)) {
            kernel()->issues()->log(TR(ERR_INVALID_PROPERTY_FOR_2).arg("Range",odf->url()));
            return false;
        }
        RawConverter conv = converter(odf,range);
        vmin = conv.raw2real(v1);
        vmax = conv.raw2real(v2);
    }
    return true;
}

RawConverter Ilwis3Range::converter(const ODF &odf, const QString &section)  {
    QString range = odf->value(section,"Range" );
    if ( range == sUNDEF) {
        QString dminfo = odf->value(section,"DomainInfo");
        if ( dminfo != sUNDEF) {
            QStringList parts = dminfo.split(";");
            QString dmtype = parts[2];
            return RawConverter(dmtype);
        }
    }
    QString sst = odf->value(section,"StoreType");
    IlwisTypes st = itUNKNOWN;
    if ( sst == "Byte")
        st = itUINT8;
    else if ( sst == "Int")
        st = itINT16;
    else if ( sst == "Long")
        st = itINT32;
    else if ( sst== "Real")
        st = itDOUBLE;
    else if ( sst == "Float")
        st = itFLOAT;

    QStringList parts = range.split(":");
    if ( parts.size() >= 2) {
        double scale = 1.0;
        double offset = 0;
        bool ok1, ok2;
        ok1 = ok2 = true;
        double vmin = parts[0].toDouble();
        double vmax = parts[1].toDouble();
        if ( parts.size() == 3) {
            offset = parts[2].mid(7).toDouble(&ok1);
        } if ( parts.size() == 4) {
            scale = st != itDOUBLE ? parts[2].toDouble() : 1.0;
            offset = parts[3].mid(7).toDouble(&ok2);

        }
        if ( ! ( ok1 && ok2)) {
            kernel()->issues()->log(TR(ERR_INVALID_PROPERTY_FOR_2).arg("Range",odf->url()));
            return RawConverter();
        }
        return RawConverter(offset, scale,vmin, vmax, st);
    }
    return RawConverter();
}
