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
#include "ilwisoperation.h"
#include "operationhelper.h"
#include "operationhelpergrid.h"
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
    double minv=1e307,maxv = -1e307;
    PixelIterator pout(_outputRaster);
    for(auto& band : _bands){
        for(double value : band){
            *pout = value;
            minv = Ilwis::min(value,minv);
            maxv = Ilwis::max(value,maxv);
            updateTranquilizer(pout.linearPosition(), 1000);
            ++pout;
        }
    }
    double resolution = _outputRaster->datadefRef().domain()->range<NumericRange>()->resolution();
    NumericRange *rng = new NumericRange(minv, maxv, resolution);
    _outputRaster->datadefRef().range(rng);


    QVariant value;
    value.setValue<IRasterCoverage>(_outputRaster);
    logOperation(_outputRaster, _expression);
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
    bool ok, ok2=true;
    std::vector<double> items;
    QString dd = stackDef;
    int nr = dd.toInt(&ok);
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
                items.resize((1 + mmax - mmin)/res);
                double val = mmin;
                for(int i=0; i < items.size(); ++i){
                    items[i] = val;
                    val += res;
                }

            }
        }else {
            items.resize(parts.size());
            for(int i=0; i < items.size(); ++i){
                items[i] = parts[i].toDouble(&ok);
                if (!ok) {
                    ok2= false;
                    break;
                }
            }
        }

    }else {

        items.resize(nr);
        for(int i=0; i < nr; ++i)
            items[i] = i;
    }
    if  (ok2){
        int n = 0;
        if ( items.size() > _bands.size() && _bands.size() != 0){
            n = _bands.size();
        }else if ( items.size() <= _bands.size())
            n = items.size();
        else
            return false;
        _stackValueNumbers.clear();
        for(int i=0; i < n;++i)
            _stackValueNumbers.push_back(items[i]);
    }
    return ok2;
}

bool CreateRasterCoverage::parseStackDefintion(const QString& stacDef){
    QString stackDef = stacDef;
    stackDef.remove('\"');
    std::vector<QString> items;
    bool ok = true;
    if ( _stackDomain->ilwisType() == itNUMERICDOMAIN){
        if ( hasType(_stackDomain->valueType(), itINTEGER | itFLOAT | itDOUBLE)){
            ok = parseStackDefintionNumericCase(stackDef);
        }else if ( hasType(_stackDomain->valueType(), itDATETIME)){
            //TODO
        }
        return ok;
    }else if ( _stackDomain->ilwisType() == itITEMDOMAIN){
        IItemDomain itemdomain = _stackDomain.as<ItemDomain<DomainItem>>();
        if ( stackDef == ""){ // all items

            for(auto item : itemdomain){
                items.push_back(item->name());
            }
        }else {
            QStringList parts = stackDef.split(",");
            for(const QString& part : parts){
                if ( itemdomain->contains(part)){
                    items.push_back(part);;
                }else {
                    ok = false;
                }
            }
        }
    } else if ( _stackDomain->ilwisType() == itTEXTDOMAIN){
        QStringList parts = stackDef.split(",");
        for(auto item : parts){
            items.push_back(item);
        }
     }

    if  (ok){
        int n = 0;
        if ( items.size() >= _bands.size() && _bands.size() != 0){
            n = _bands.size();
        }else if ( items.size() < _bands.size())
            n = items.size();
        else
            return false;
        _stackValueStrings.clear();
        for(int i=0; i < n;++i)
            _stackValueStrings.push_back(items[i]);
    }
    return ok;

}

Ilwis::OperationImplementation::State CreateRasterCoverage::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
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
    if ( maps != sUNDEF && (maps.indexOf("?") != -1 || maps.indexOf("*") != -1)){
        maps = expandWildCards(maps);
    }
    if (maps != "" && maps != sUNDEF){
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

    for(quint32 i=0; i < _ranges.size(); ++i){
        if ( i < _outputRaster->size().zsize()){
           _outputRaster->datadefRef(i).range(_ranges[i]);
        }

    }
    initialize(_outputRaster->size().linearSize());
    return sPREPARED;
}

QString CreateRasterCoverage::expandWildCards(const QString& wildmaps){
    QString result;
    QString maps = wildmaps;
    maps.replace("*","%");
    maps.replace("?","_");
    QString extraPath ;
    if ( maps.indexOf("/") != -1){
        QStringList parts = maps.split("/");
        maps = parts.back();
        for(int i=0; i < parts.size() - 1; ++i)
            extraPath +=  "/" + parts[i] ;
    }
    QString containerPath = context()->workingCatalog()->resource().url().toString();
    QString query = "container='" + containerPath + extraPath + "' and name LIKE '" + maps + "'";
    std::vector<Resource> resources = mastercatalog()->select(query);
    for(auto resource : resources){
        if ( resource.ilwisType() == itRASTER){
            if ( result != "")
                result += ",";
            result += resource.url().toString();
        }
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
     resource.addOptionalInParameter(3, itDOMAIN,TR("Stack domain"), TR("Option Domain of the z direction (stack), default is 'count'"));
     resource.addOptionalInParameter(4, itSTRING|itINTEGER,TR("Stack defintion"), TR("Content of the stack, numbers, elements of item domain or sets of numbers"));
     resource.addOptionalInParameter(5, itBOOL,TR("Auto resample"), TR("Checking this option will automatically resample all bands to the input georeference"));
     resource.setOutParameterCount({1});
     resource.addOutParameter(0, itRASTER, TR("raster coverage"), TR("The newly created raster"));
     resource.setKeywords("raster,create,workflow");

     mastercatalog()->addItems({resource});
     return resource.id();
}
//-----------------------------------------------------------------------------------------------------
REGISTER_OPERATION(CreateSimpelRasterCoverage)

quint64 CreateSimpelRasterCoverage::createMetadata()
{
    OperationResource resource({"ilwis://operations/createrastercoverage"});
    resource.setLongName("Create Simpel Raster coverage");
    resource.setSyntax("createrastercoverage(georeference, empty=!true|false)");
    resource.setDescription(TR("Creates a empty raster based on the provided georeference. If a georeference is used to create it it will always have a numeric domain; else it will copy it from the raster"));
    resource.setInParameterCount({2});
    resource.addInParameter(0, itGEOREF|itRASTER,TR("Georeference"), TR("Geometry of the new rastercoverage"));
    resource.addInParameter(1, itBOOL,TR("Clean"), TR("If  clean the raster will contain no layers, else a default empty layer will be created"));
    resource.setOutParameterCount({1});
    resource.addOutParameter(0, itRASTER, TR("raster coverage"), TR("The newly created raster"));
    resource.setKeywords("raster,create,workflow");

    mastercatalog()->addItems({resource});
    return resource.id();
}

OperationImplementation *CreateSimpelRasterCoverage::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new CreateSimpelRasterCoverage( metaid, expr);
}

CreateSimpelRasterCoverage::CreateSimpelRasterCoverage(quint64 metaid,const Ilwis::OperationExpression &expr) : CreateRasterCoverage(metaid, expr)
{}

Ilwis::OperationImplementation::State CreateSimpelRasterCoverage::prepare(ExecutionContext *ctx,const SymbolTable&){
    IRasterCoverage inputRaster;
    if ( _expression.parm(0).valuetype() == itRASTER){
        if(!inputRaster.prepare(_expression.input<QString>(0))){
            kernel()->issues()->log(TR("Invalid raster used to create new raster:") + _expression.input<QString>(0));
            return sPREPAREFAILED;
        }
        _outputRaster = OperationHelperRaster::initialize(inputRaster,itRASTER,itRASTERSIZE|itDOMAIN|itCOORDSYSTEM|itGEOREF);
        return sPREPARED;
    }
    QString grf = _expression.input<QString>(0);
    if ( ! _grf.prepare(grf)){
        kernel()->issues()->log(QString(TR("%1 is and invalid georeference")).arg(grf));
        return sPREPAREFAILED;
    }
     _empty = _expression.input<bool>(1);
    _domain.prepare("code=domain:value");
    _stackDomain = IDomain("count");
    if (!_empty){
        _stackValueStrings = {"1"};
        _stackValueNumbers = {1};
    }
    _outputRaster.prepare();
    _outputRaster->georeference(_grf);
    _outputRaster->setDataDefintions(_domain, _stackValueNumbers , _stackDomain);

    return sPREPARED;
}


bool CreateSimpelRasterCoverage::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    setOutput(_outputRaster,ctx,symTable);
    logOperation(_outputRaster, _expression);

    return true;
}
