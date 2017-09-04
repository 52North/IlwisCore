#include "kernel.h"
#include "raster.h"
#include "pixeliterator.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "opencv.hpp"
#include "range.h"
#include "itemrange.h"
#include "colorrange.h"
#include "opencvhelper.h"

using namespace Ilwis;
using namespace OpenCV;

OpenCVHelper::OpenCVHelper()
{
}

bool OpenCVHelper::raster2Mat(PixelIterator rasterIter, cv::Mat& cvRaster){

    int opencvType = ilwisType2OpenCVType(rasterIter.raster()->datadef().domain()->valueType());
    try{
        bool usesColor = rasterIter.raster()->datadef().domain()->ilwisType() == itCOLORDOMAIN;
        if ( usesColor){
            const int dims[] = {(int)rasterIter.box().ylength(), (int)rasterIter.box().xlength(),3};
            cvRaster.create(3, dims,CV_8UC3);
        }
        else
            cvRaster.create(rasterIter.box().ylength(), (int)rasterIter.box().xlength(),opencvType);

        PixelIterator rasterIterEnd = rasterIter.end();
        if ( usesColor){
            while(rasterIter != rasterIterEnd){
                quint64 colorint = *rasterIter;
                LocalColor *localcolor = reinterpret_cast<LocalColor *>(&colorint);
                cvRaster.at<quint8>(rasterIter.position().y, rasterIter.position().x, 0) = localcolor->_component1;
                cvRaster.at<quint8>(rasterIter.position().y, rasterIter.position().x, 1) = localcolor->_component2;
                cvRaster.at<quint8>(rasterIter.position().y, rasterIter.position().x, 2) = localcolor->_component3;
            }

        }else {
            while(rasterIter != rasterIterEnd){

                switch(opencvType){
                case CV_8U:
                    cvRaster.at<quint8>(rasterIter.position().y, rasterIter.position().x) = *rasterIter;break;
                case CV_8S:
                    cvRaster.at<qint8>(rasterIter.position().y, rasterIter.position().x) = *rasterIter;break;
                case CV_16U:
                    cvRaster.at<quint16>(rasterIter.position().y, rasterIter.position().x) = *rasterIter;break;
                case CV_16S:
                    cvRaster.at<qint16>(rasterIter.position().y, rasterIter.position().x) = *rasterIter;break;
                case CV_32S:
                    cvRaster.at<qint32>(rasterIter.position().y, rasterIter.position().x) = *rasterIter;break;
                case CV_32F:
                    cvRaster.at<float>(rasterIter.position().y, rasterIter.position().x) = *rasterIter;break;
                case CV_64F:
                    cvRaster.at<double>(rasterIter.position().y, rasterIter.position().x) = *rasterIter;break;
                }


                ++rasterIter;
            }
        }
        return true;
    } catch(cv::Exception ex){
        throw ErrorObject(QString::fromStdString(ex.msg));
    }
    return false;
}

bool OpenCVHelper::mat2Raster(const cv::Mat& cvRaster, PixelIterator& iter){
    if ( !iter.isValid()){
        return false;
    }

    if ( iter.box().size().twod() != Size<>(cvRaster.cols, cvRaster.rows,1).twod()){
        return ERROR2(ERR_NO_INITIALIZED_2,"size", "raster");
    }

    double mmin = 1e308, mmax = -1e308;

    switch(cvRaster.type()){
    case CV_8U:
        copy2Raster<quint8>(cvRaster, iter, mmin, mmax);break;
    case CV_8S:
        copy2Raster<qint8>(cvRaster, iter, mmin, mmax);break;
    case CV_16U:
        copy2Raster<quint16>(cvRaster, iter, mmin, mmax);break;
    case CV_16S:
        copy2Raster<qint16>(cvRaster, iter, mmin, mmax);break;
    case CV_32S:
        copy2Raster<qint32>(cvRaster, iter, mmin, mmax);break;
    case CV_32F:
        copy2Raster<float>(cvRaster, iter, mmin, mmax);break;
    case CV_64F:
        copy2Raster<double>(cvRaster, iter, mmin, mmax);break;
    }

    iter.raster()->datadefRef().range(new NumericRange(mmin, mmax, hasType(iter.raster()->datadef().domain()->valueType(), itINTEGER) ? 1 : 0));

    return true;
}

IRasterCoverage OpenCVHelper::mat2Raster(const cv::Mat& cvRaster, const IGeoReference& grf){
    IRasterCoverage raster;
    raster.prepare();
    raster->georeference(grf);
    Envelope env = grf->pixel2Coord(BoundingBox(Size<>(cvRaster.cols, cvRaster.rows,1)));
    raster->envelope(env);
    IDomain dom;
    switch ( openCVType2IlwisType(cvRaster.type())){
    case itUINT8:
        dom.prepare("image"); break;
    case itINT8:
    case itINT16:
    case itINT32:
        dom.prepare("integer"); break;
    case itUINT16:
        dom.prepare("image16"); break;
    case itFLOAT:
    case itDOUBLE:
        dom.prepare("value");
    default:
        return IRasterCoverage();

    }
    raster->datadefRef() = DataDefinition(dom);
    PixelIterator iter(raster);
    if ( ! mat2Raster(cvRaster, iter))
        return IRasterCoverage();

    return raster;
}

void OpenCVHelper::createHistogram(Ilwis::PixelIterator rasterIter, cv::SparseMat& histogram, bool accumulate){

    cv::Mat cvRaster;
    OpenCVHelper::raster2Mat(rasterIter, cvRaster);
    IlwisTypes valueType = rasterIter.raster()->datadef().domain()->valueType();
    if ( hasType(valueType,itNUMBER) ){
        int channels[] = {0};
        int bins = 300;
        NumericStatistics& stats = rasterIter.raster()->statistics();
        if ( !stats.isValid()){
            stats.calculate(rasterIter,rasterIter.end());
        }
        double lowerLimit = rasterIter.raster()->statistics()[NumericStatistics::pMIN];
        double upperLimit = rasterIter.raster()->statistics()[NumericStatistics::pMAX];
        switch(valueType){
        case itINT8:
        case itUINT8:
            bins = 40;
            break;
        case itUINT16:
        case itINT16:
            bins = 80;
            break;
        case itINT32:
        case itUINT32:
            bins = 200;
            break;
        default:
            bins = 500;
        }

        if (hasType(valueType, itINT8))
            bins = 40;
        else if ( hasType(valueType , itINT16 | itUINT16))
            bins = 80;
        else if(hasType(valueType , itINT32 | itUINT32))
            bins = 160;
        int histSize[] = { bins};
        float sranges[] = { lowerLimit, upperLimit };
        const float * ranges[] = { sranges };
        cv::calcHist(&cvRaster, 1, channels, cv::Mat(), histogram, 1, histSize, ranges, true,accumulate );
    } if ( hasType(valueType,itCONTINUOUSCOLOR | itPALETTECOLOR)){
        //TODO: histograms of color rasters
    }


}

IlwisTypes OpenCVHelper::openCVType2IlwisType(quint32 cvtype){
    switch(cvtype) {
    case CV_8U:
        return itUINT8; break;
    case CV_8S:
        return itINT8; break;
    case CV_16U:
        return itUINT16; break;
    case CV_16S:
        return itINT16; break;
    case CV_32S:
        return itINT32; break;
    case CV_32F:
        return itFLOAT; break;
    case CV_64F:
        return itDOUBLE; break;
    }
    return iUNDEF;
}

quint32 OpenCVHelper::ilwisType2OpenCVType(IlwisTypes tp)
{
    switch(tp){
    case itUINT8:
        return CV_8U;
    case itINT8:
        return CV_8S;
    case itUINT16:
        return CV_16U;
    case itINT16:
        return CV_16S;
    case itINT32:
    case itUINT32:
        return CV_32S;
    case itDOUBLE:
    case itUINT64:
    case itINT64:
        return CV_64F;
    case itCONTINUOUSCOLOR:
    case itPALETTECOLOR:
        return CV_8U; break;
    default:
        ERROR2(ERR_OPERATION_NOTSUPPORTED2, "Value type", "OpenCV");

    }
    return iUNDEF;
}

bool OpenCVHelper::determineCVType(IlwisTypes valuetype, qint8 &sourcedepth)
{
    switch(valuetype){
    case itUINT8:
        sourcedepth = CV_8U; break;
    case itUINT16:
        sourcedepth = CV_16U; break;
    case itINT16:
        sourcedepth = CV_16S; break;
    case itINT32:
        sourcedepth = CV_32S; break;
    case itDOUBLE:
        sourcedepth = CV_64F; break;
    default:
        ERROR2(ERR_OPERATION_NOTSUPPORTED2, "Value type", "Sobel filter");

    }
    return true;
}

