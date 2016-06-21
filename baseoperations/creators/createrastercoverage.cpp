#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "raster.h"
#include "itemdomain.h"
#include "itemiterator.h"
#include "pixeliterator.h"
#include "catalog.h"
#include "createrastercoverage.h"
#include "ilwiscontext.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(CreateRasterCoverage)

CreateRasterCoverage::CreateRasterCoverage()
{

}

CreateRasterCoverage::CreateRasterCoverage(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool CreateRasterCoverage::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;


    initialize(_outputRaster->size().linearSize());
    PixelIterator pout(_outputRaster);
    for(auto& band : _bands){
        PixelIterator pin(band);
        for(double value : band){
            *pout = value;
//            if(!trq()->update(1))
//                return false;
            ++pout;
        }
    }


    QVariant value;
    value.setValue<IRasterCoverage>(_outputRaster);
    ctx->setOutput(symTable,value,_outputRaster->name(),itRASTER,_outputRaster->resource());

    return true;
}

Ilwis::OperationImplementation *CreateRasterCoverage::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new CreateRasterCoverage(metaid, expr);
}

bool CreateRasterCoverage::parseStackDefintionTimeCase(const QString& stackDef){
    QStringList parts = stackDef.split(",");
    if ( (parts.size() == 1 || parts.size() == 2) && stackDef.indexOf("..") > 0){
        QStringList parts2 = parts[0].split("..");
        if ( parts2.size() == 2){
            Duration res(1.0);
            Time mmin = parts2[0];
            Time mmax = parts2[1];
            if ( mmin > mmax) return false;
            if ( parts.size() == 2){
                res = parts[2];
            }
            if ( !mmin.isValid() || !mmax.isValid() || !res.isValid())
                return false;
            _stackValueNumbers.resize((mmax - mmin)/res);
            double val = mmin;
            for(int i=0; i < _stackValueNumbers.size(); ++i){
                _stackValueNumbers[i] = val;
                val += res;
            }

        }
    }else {
        _stackValueNumbers.resize(parts.size());
        for(int i=0; i < _stackValueNumbers.size(); ++i){
            _stackValueNumbers[i] = parts[i].toDouble();
        }
    }
    return true;
}

bool CreateRasterCoverage::parseStackDefintionNumericCase(const QString& stackDef){
    bool ok;
    int nr = stackDef.toUInt(&ok);
    if ( !ok){
        QStringList parts = stackDef.split(",");
        if ( (parts.size() == 1 || parts.size() == 2) && stackDef.indexOf("..") > 0){
            QStringList parts2 = parts[0].split("..");
            if ( parts2.size() == 2){
                double res = 1;
                double mmin = parts2[0].toDouble(&ok);
                if (!ok) return false;
                double mmax = parts2[1].toDouble(&ok);
                if (!ok)
                    mmax = _bands.size();

                if ( mmin > mmax) return false;
                if ( parts.size() == 2){
                    res = parts[2].toDouble(&ok);
                    if (!ok && res <= 0) return false;
                }
                _stackValueNumbers.resize((1 + mmax - mmin)/res);
                double val = mmin;
                for(int i=0; i < _stackValueNumbers.size(); ++i){
                    _stackValueNumbers[i] = val;
                    val += res;
                }
                return true;

            }
        }else {
            _stackValueNumbers.resize(parts.size());
            for(int i=0; i < _stackValueNumbers.size(); ++i){
                _stackValueNumbers[i] = parts[i].toDouble(&ok);
                if (!ok) return false;
            }
            return true;
        }

    }else {
        _stackValueNumbers.resize(nr);
        for(int i=0; i < nr; ++i)
            _stackValueNumbers[i] = i;
        return true;
    }
    return false;
}

bool CreateRasterCoverage::parseStackDefintion(const QString& stacDef){
    QString stackDef = stacDef;
    stackDef.remove('\"');
    if ( _stackDomain->ilwisType() == itNUMERICDOMAIN){
        if ( hasType(_stackDomain->valueType(), itINTEGER | itFLOAT | itDOUBLE)){
            return parseStackDefintionNumericCase(stackDef);
        }else if ( hasType(_stackDomain->valueType(), itDATETIME)){
            //TODO
        }
    }else if ( _stackDomain->ilwisType() == itITEMDOMAIN){
        IItemDomain itemdomain = _stackDomain.as<ItemDomain<DomainItem>>();
        if ( stackDef == ""){ // all items

            for(auto item : itemdomain){
                _stackValueStrings.push_back(item->name());
            }
        }else {
            QStringList parts = stackDef.split(",");
            for(const QString& part : parts){
                if ( itemdomain->contains(part)){
                    _stackValueStrings.push_back(part);;
                }else {
                    return false;
                }
            }
        }
    } else if ( _stackDomain->ilwisType() == itTEXTDOMAIN){
        QStringList parts = stackDef.split(",");
        for(auto item : parts){
            _stackValueStrings.push_back(item);
        }
    }
    return true;

}

Ilwis::OperationImplementation::State CreateRasterCoverage::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    auto CreateStackDomain = [&](const QString& dom)-> Ilwis::OperationImplementation::State{
        _stackDomain.prepare(dom);
        if ( !_stackDomain.isValid()){
            kernel()->issues()->log(QString(TR("%1 is and invalid stack domain")).arg(dom));
            return sPREPAREFAILED;
        }
        return sPREPARED;
    };

    QString grf = _expression.input<QString>(0);
    _grf.prepare(grf);
    if ( !_grf.isValid()){
        kernel()->issues()->log(QString(TR("%1 is and invalid georeference")).arg(grf));
        return sPREPAREFAILED;
    }
    QString maps = _expression.input<QString>(2);
    if ( maps.indexOf("?") != -1 || maps.indexOf("*") != -1){
        maps = expandWildCards(maps);
    }
    if (maps != ""){
        QStringList bands = maps.split(",");
        for(QString band : bands){
            IRasterCoverage raster(band);
            if ( raster.isValid() && ( raster->georeference()->isCompatible(_grf) || _autoresample)){
                _bands.push_back(raster);
                _ranges.push_back(raster->datadef().range()->clone());
            }else{
                kernel()->issues()->log(QString(TR("%1 is and invalid band; raster can not be build")).arg(raster->name()));
                return sPREPAREFAILED;
            }
        }
    }
    QString dom = _expression.input<QString>(1);
    if ( dom == "as bands"){
        if (_bands.size() > 0)
            dom = _bands[0]->datadef().domain()->resource().url().toString();
        else
            dom = "value";
    }
    _domain.prepare(dom);
    if ( !_domain.isValid()){
        kernel()->issues()->log(QString(TR("%1 is and invalid domain")).arg(dom));
        return sPREPAREFAILED;
    }


    _stackDomain = IDomain("count");
    _stackValueStrings = {"1"};
    _stackValueNumbers = {1};
    if ( _expression.parameterCount() >= 4){
        for(int i=3; i < _expression.parameterCount(); ++i){
            if ( hasType(_expression.parm(i).valuetype(),itSTRING|itINTEGER)){
                parseStackDefintion(_expression.input<QString>(i));
            }
            else if ( hasType(_expression.parm(i).valuetype(),itDOMAIN)){
                if (CreateStackDomain(_expression.input<QString>(i)) == sPREPAREFAILED)
                    return sPREPAREFAILED;
            }
            else if ( hasType(_expression.parm(i).valuetype(),itBOOL) && maps.size() != 0){
                _autoresample = _expression.input<bool>(i);
            }
        }
    }


    _outputRaster.prepare();
    _outputRaster->georeference(_grf);
    _outputRaster->size( Size<>(_grf->size().xsize(), _grf->size().ysize(), std::max(_stackValueNumbers.size(), _stackValueStrings.size())));
    if ( _stackDomain->ilwisType() == itNUMERICDOMAIN)
        _outputRaster->setDataDefintions(_domain, _stackValueNumbers , _stackDomain);
    else
         _outputRaster->setDataDefintions(_domain, _stackValueStrings, _stackDomain);

    return sPREPARED;
}

QString CreateRasterCoverage::expandWildCards(const QString& wildmaps){
    QString result;
    QString maps = wildmaps;
    maps.replace("*","%");
    maps.replace("?","_");
    QString containerPath = context()->workingCatalog()->resource().url().toString();
    QString query = "container='" + containerPath + "' and name LIKE " + maps;
    std::vector<Resource> resources = mastercatalog()->select(query);
    for(auto resource : resources){
        if ( result != "")
            result += ",";
        result += resource.url().toString();
    }
    return result;
}

quint64 CreateRasterCoverage::createMetadata()
{
    OperationResource resource({"ilwis://operations/createrastercoverage"});
    resource.setLongName("Create Raster Coverage");
    resource.setSyntax("createrastercoverage(georeference, domain,bands[, stack-defintion][,stackdomain][,auto-resample])");
    resource.setInParameterCount({3,4,5,6});
    resource.addInParameter(0, itGEOREF,TR("Georeference"), TR("Geometry of the new rastercoverage"));
    resource.addInParameter(1, itDOMAIN|itSTRING,TR("Domain"), TR("Domain used by the raster coverage"));
    resource.addInParameter(2, itSTRING, TR("Bands"), TR("parameter defining a the bands that will be copied to the new raster coverage, Note that the bands maybe empty in which case an empty raster will be created"));
    resource.addOptionalInParameter(3, itSTRING|itINTEGER,TR("Stack defintion"), TR("Content of the stack, numbers, elements of item domain or sets of numbers"));
    resource.addOptionalInParameter(4, itDOMAIN,TR("Stack domain"), TR("Option Domain of the z direction (stack), default is 'count'"));
    resource.addOptionalInParameter(5, itBOOL,TR("Auto resample"), TR("Checking this option will automatically resample all bands to the input georeference"));
    resource.setOutParameterCount({1});
    resource.addOutParameter(0, itRASTER, TR("raster coverage"), TR("The newly created raster"));
    resource.setKeywords("rastercoverage, create");

    mastercatalog()->addItems({resource});
    return resource.id();
}



