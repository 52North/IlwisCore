#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "columndefinition.h"
#include "basetable.h"
#include "flattable.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "thematicitem.h"
#include "identifierrange.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "operationhelpergrid.h"
#include "geometryhelper.h"
#include "clusterraster.h"

using namespace Ilwis;
using namespace RasterOperations;

REGISTER_OPERATION(ClusterRaster)

ClusterRaster::ClusterRaster()
{
}

ClusterRaster::ClusterRaster(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid,expr)
{

}

bool ClusterRaster::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    IRasterCoverage indexLookup;
    OperationHelperRaster::initialize(_outputRaster, indexLookup, itCOORDSYSTEM | itRASTERSIZE | itGEOREF);
    indexLookup->datadefRef() = DataDefinition(IDomain("count"));

    PixelIterator iterIn(_inputRaster, BoundingBox(),PixelIterator::fZXY);
    for(auto& value : indexLookup){
        quint32 index = getFSIndex(iterIn);
        _histbands[index]._count++;
        value = index;
    }

    /* check actual number of combinations in combined histogram
        and fill histogram array from the bottom up. */
     long clusterCombinations = 0;
     long filled = 65535L; // 16 bit (2 bands x 8 or 4 bands x 4)
     if (_inputRaster->size().zsize() == 3)
         filled = 32767;   // 15 bit (3 bands x 5)
     for (long wi = 0; wi <= filled; wi++) {
         if (_histbands[wi]._count != 0) { // existing entry
             _histbands[clusterCombinations]._combinations._comb  =  _histbands[wi]._combinations._comb;
             _histbands[clusterCombinations]._count =  _histbands[wi]._count;
             clusterCombinations++;
         }
     }

    std::vector<ClusterRecord> newClusterSet(ClusterRaster::MAXBANDS);

    int newClusters = clusterCalculation(clusterCombinations, _outputRaster->size().linearSize(),newClusterSet);

    if (newClusters == 0)
        return 0;   // empty feature space (!), so we are done

    std::vector<quint8> classTabel(65536L,0);

    for (quint32 i = 0; i < newClusters; i++)
         for (long k = newClusterSet[i]._firstRecord; k < newClusterSet[i]._firstRecord + newClusterSet[i]._numRecInCube; k++)
             classTabel[_histbands[k]._combinations._comb] = i + 1;

    PixelIterator indexIter(indexLookup);
    for( auto& value : _outputRaster){
         value = classTabel[(quint32)*indexIter];
         ++indexIter;
    }

    QVariant value;
    value.setValue<IRasterCoverage>(_outputRaster);
    logOperation(_outputRaster,_expression);
    ctx->setOutput(symTable,value,_outputRaster->name(), itRASTER, _outputRaster->resource() );

    return true;
}

void ClusterRaster::initClusterSet(long total, long clusterCombinations, std::vector<ClusterRecord>& newClusterSet)
{
    for (auto& cluster : newClusterSet) {
        for (quint32 j = 0; j < _inputRaster->size().zsize(); j++) {
            cluster._bandsMin[j] = _aarIndex2Value[j][0];
            cluster._bandsMax[j] = _aarIndex2Value[j][_reducedValueSpread];
        };
    };

    newClusterSet[0]._firstRecord = 0;
    newClusterSet[0]._numRecInCube = clusterCombinations;
    newClusterSet[0]._numPixelsInCube = total;
}

int ClusterRaster::clusterCalculation(long clusterCombinations, long total,std::vector<ClusterRecord>& newClusterSet)
{
    int cubeIndex;  // cube index in pNewCluster
    quint16 iMinCluster, iMaxCluster;

    initClusterSet(total, clusterCombinations, newClusterSet);

    quint32 newClusters = 1;  // count for new nr. of cubes
    while (newClusters < _numberOfThematicItems) {
        long iSum, iCnt;

        long maxSize = -1;
        /*  Selection by largest nr. of pxels in cube */
        for (quint32 i=0; i<_numberOfThematicItems; i++) {
            if ((newClusterSet[i]._numPixelsInCube> maxSize) && (newClusterSet[i]._numRecInCube > 1)) {
                maxSize = newClusterSet[i]._numPixelsInCube;
                cubeIndex = i;
            }
        }
        if (maxSize == -1) break;

        // iCubeIndex contains now cube nr. to be split
        // determine largest cube side
        double maxWidth = -1;
        quint32 sortBands = 0;
        for (quint32 j = 0; j < _inputRaster->size().zsize(); j++) {
            double clusterSize = newClusterSet[cubeIndex].clusterSize(j);
            if (clusterSize > maxWidth) {
                maxWidth = clusterSize;
                sortBands = j;
            }
        }
        // iSortBands contains now cube side to be split

        // sort histogram records contained in cube iCubeIndex
        CompareHistRec compare(this);
        auto startSort = _histbands.begin() + newClusterSet[cubeIndex]._firstRecord;
        auto endSort = _histbands.begin() + newClusterSet[cubeIndex]._firstRecord + newClusterSet[cubeIndex]._numRecInCube - 1;
        std::sort(startSort, endSort,[&sortBands,&compare](const HistBandsRec& firstRec, const HistBandsRec& nextRec)
            { return compare.compareHistRec(firstRec, nextRec, sortBands);});


        // and split
        // find pos, so that in both new cubes are approx. same nr. of pixels
        iSum = newClusterSet[cubeIndex]._numPixelsInCube / 2;
        quint32 k = newClusterSet[cubeIndex]._firstRecord;
        iCnt = _histbands[k]._count;
        k++;
        for (;k <= newClusterSet[cubeIndex]._firstRecord + newClusterSet[cubeIndex]._numRecInCube-2; k++) {
            iCnt += _histbands[k]._count;
            if (iCnt > iSum) {
                iCnt -= _histbands[k]._count;
                break;
            }
        }
        // k now contains first hist. rec index of 'new' cube
        // iCnt now contains nr. of pixels in 'old' cube
        // adjust Bands values to 5 bits:
        iMaxCluster = _histbands[k - 1]._combinations._comb;
        iMinCluster = _histbands[k]._combinations._comb;

        iMinCluster = (iMinCluster >> (sortBands * _shift)) & _reducedValueSpread;
        iMaxCluster = (iMaxCluster >> (sortBands * _shift)) & _reducedValueSpread;

        // adjust current and old cube
        newClusterSet[newClusters]._firstRecord = k;
        newClusterSet[newClusters]._numPixelsInCube = newClusterSet[cubeIndex]._numPixelsInCube - iCnt;
        newClusterSet[newClusters]._numRecInCube = newClusterSet[cubeIndex]._numRecInCube - (k - newClusterSet[cubeIndex]._firstRecord);

        newClusterSet[cubeIndex]._numPixelsInCube = iCnt;
        newClusterSet[cubeIndex]._numRecInCube -=  newClusterSet[newClusters]._numRecInCube;
        for (quint32 j=0; j< _inputRaster->size().zsize(); j++) {
            newClusterSet[newClusters]._bandsMin[j] = newClusterSet[cubeIndex]._bandsMin[j];
            newClusterSet[newClusters]._bandsMax[j] = newClusterSet[cubeIndex]._bandsMax[j];
        }
        newClusterSet[newClusters]._bandsMin[sortBands] = _aarIndex2Value[sortBands][iMinCluster];//+1;
        newClusterSet[cubeIndex]._bandsMax[sortBands] = _aarIndex2Value[sortBands][iMaxCluster];
        newClusters++;
    }


    return newClusters;
}

long ClusterRaster::getFSIndex(PixelIterator& iterIn) {

    long index = 0;
    do{
        BoundRange& br = _abrLookup[iterIn.position().z];
        double val = *iterIn;
        // taking the 1% range needs check for values in the 1% range
        if (val < br.bottom) val = br.bottom;
        if (val > br.top) val = br.top;

        double ratio = (val - br.bottom) / (br.top - br.bottom);
        int subindex = (long) (ratio * _reducedValueSpread);
        index += (subindex) << (_indexShift * iterIn.position().z);
        ++iterIn;
    } while(!iterIn.xchanged());
    if ( index != 0){
        qDebug() << "stop";
    }
    return index;
}

void ClusterRaster::initFeatureSpaceHistogram(int noOfbands) {

    int i, j, k, l;
    _histbands.resize(65536L);
    switch (noOfbands) {
    case 1: {
        _reducedValueSpread = 255; _shift = 8; _indexShift = 0;
        for (i=0; i < 256; i++)
            for (j=0; j < 256; j++) {
                unsigned int x = j + (i << 8);
                _histbands[x]._count = 0;
                if (i == 0)
                    _histbands[x]._combinations._comb = j;
                else
                    _histbands[x]._combinations._comb = 0;
            }
        break;
    }
    case 2: {
        _reducedValueSpread = 255; _shift = 8; _indexShift = 0;
        for (i=0; i < 256; i++)
            for (j=0; j < 256; j++) {
                unsigned int x = j + (i << 8);
                _histbands[x]._count = 0;
                _histbands[x]._combinations._comb = x;
            }
        break;
    }
    case 3: {
        _reducedValueSpread = 31; _shift = 5; _indexShift = 5;
        for (i=0; i < 32; i++)
            for (j=0; j < 32; j++)
                for (k=0; k < 32; k++) {
                    unsigned int x = k + (j << 5) + (unsigned)(i << 10);
                    _histbands[x]._count = 0;
                    _histbands[x]._combinations._comb = x;
                }
        break;
    }
    case 4: {
        _reducedValueSpread = 15; _shift = 4; _indexShift = 4;
        for (i=0; i < 16; i++)
            for (j=0; j < 16; j++)
                for (k=0; k < 16; k++)
                    for (l=0; l < 16; l++) {
                        unsigned int x = l + (k << 4) + (unsigned)(j << 8) + (unsigned)(i << 12);
                        _histbands[x]._count = 0;
                        _histbands[x]._combinations._comb = x;
                    }
        break;
    }
    }
}

void ClusterRaster::initStretchLookup() {
    _aarIndex2Value.resize(_inputRaster->size().zsize());

    for (quint32 j = 0; j < _inputRaster->size().zsize(); j++) {
        PixelIterator iter = _inputRaster->band(j);
        NumericStatistics stats;
        stats.calculate(iter, iter.end(),NumericStatistics::pHISTOGRAM,262);
        double minValue = stats(NumericStatistics::pMIN,1);
        double maxValue = stats(NumericStatistics::pMAX,1);
        _abrLookup[j].bottom = minValue;
        _abrLookup[j].top = maxValue;

        _aarIndex2Value[j].resize(_reducedValueSpread + 1);
        for (quint32 i = 0; i <= _reducedValueSpread; ++i) {
            _aarIndex2Value[j][i] = minValue + ((maxValue - minValue) * i) / (double) _reducedValueSpread;
        }
    }
}

Ilwis::OperationImplementation *ClusterRaster::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new ClusterRaster(metaid, expr);
}

Ilwis::OperationImplementation::State ClusterRaster::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    try{
        OperationImplementation::prepare(ctx,st);
        OperationHelper::check([&] ()->bool { return _inputRaster.prepare(_expression.input<QString>(0), itRASTER); },
        {ERR_COULD_NOT_LOAD_2,_expression.input<QString>(0), "" } );

        OperationHelper::check([&] ()->bool { return _inputRaster->size().zsize() <= 4; },
        {ERR_OPERATION_NOTSUPPORTED2,QString(TR("layer number %1").arg(_inputRaster->size().zsize())), "clusterraster" } );

        quint32 noOfClusters = _expression.input<quint32>(1);

        OperationHelper::check([&] ()->bool { return   noOfClusters >= 2 && noOfClusters < 255; },
        {ERR_ILLEGAL_VALUE_2, TR("number of clusters"),QString::number(noOfClusters) } );

        if ( _expression.parameterCount() == 3){
            QString choice = _expression.input<QString>(2);
            _createAttributeTable =  choice.toLower() == "yes" || choice.toLower() == "true";
        }

        QString outputName = _expression.parm(0,false).value();

        OperationHelperRaster::initialize(_inputRaster, _outputRaster, itCOORDSYSTEM | itGEODETICDATUM | itGEOREF );
        if ( !_outputRaster.isValid()) {
            ERROR1(ERR_NO_INITIALIZED_1, "output rastercoverage");
            return sPREPAREFAILED;
        }
        _thematicDomain.prepare();
        _outputRaster->datadefRef().domain(_thematicDomain);
        _outputRaster->size(_inputRaster->size().twod());
        if ( outputName!= sUNDEF)
            _outputRaster->name(outputName);

        initFeatureSpaceHistogram(_inputRaster->size().zsize());
        initStretchLookup();

        return sPREPARED;

    } catch(const CheckExpressionError& err){
        ERROR0(err.message());
    }
    return sPREPAREFAILED;

}

quint64 ClusterRaster::createMetadata()
{
    OperationResource operation({"ilwis://operations/clusterraster"});
    operation.setSyntax("clusterraster(inputraster,numberOfCluster[,createAttributeTable]]");
    operation.setDescription(TR("unsupervised classification in which image data is grouped into spectral clusters. a maximum of 4 input bands"));
    operation.setInParameterCount({2,3});
    operation.addInParameter(0,itRASTER,  TR("input raster"),TR("ratser to be classified; must be 8-bit images"));
    operation.addInParameter(1,itPOSITIVEINTEGER, TR("number of clusters"),TR("number of thematic items in the output"));
    operation.addOptionalInParameter(2,itBOOL, TR("create attribute table"),TR("create an attribute table with statistical information about the thematic items"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("classified raster"), TR("output raster were all the pixels have been classified"));
    operation.setKeywords("raster, classification");

    mastercatalog()->addItems({operation});
    return operation.id();
}

//----------------------------------------------------------------------------------------
bool CompareHistRec::compareHistRec(const ClusterRaster::HistBandsRec &h1,const ClusterRaster::HistBandsRec &h2, quint32 sortBands)
{
     quint8 bMax = app->_reducedValueSpread;
     int iShift =app->_shift;
     switch (app->_inputRaster->size().zsize()) {
     case 1:
         return h1._combinations._comb  < h2._combinations._comb;
     case 2: {
         const quint16 MaskBands[4] = { bMax, bMax << iShift, 0, 0 };
         return (h1._combinations._comb & MaskBands[sortBands]) < (h2._combinations._comb & MaskBands[sortBands]);
             }
     case 3: {
         const quint16 MaskBands[4] = { bMax, bMax << iShift, bMax << (2 * iShift), 0 };
         return (h1._combinations._comb & MaskBands[sortBands]) < (h2._combinations._comb & MaskBands[sortBands]);
             }
     case 4: {
         const quint16 MaskBands[4] = { bMax, bMax << iShift, bMax << (2 * iShift), bMax << (3 * iShift) };
         return (h1._combinations._comb & MaskBands[sortBands]) < (h2._combinations._comb & MaskBands[sortBands]);
             }
     }
     return false;
}
