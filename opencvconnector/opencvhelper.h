#ifndef OPENCVHELPER_H
#define OPENCVHELPER_H

namespace Ilwis {
namespace OpenCV {
class OpenCVHelper
{
public:
    OpenCVHelper();
    static bool raster2Mat(Ilwis::PixelIterator rasterIter, cv::Mat &cvRaster);
    static Ilwis::IRasterCoverage mat2Raster(const cv::Mat &cvRaster, const Ilwis::IGeoReference &grf);
    static quint32 ilwisType2OpenCVType(IlwisTypes tp);
    static IlwisTypes openCVType2IlwisType(quint32 cvtype);
    static bool mat2Raster(const cv::Mat &cvRaster, Ilwis::PixelIterator &iter);

    template<typename T> static void copy2Raster(const cv::Mat& cvRaster,Ilwis::PixelIterator iter, double& mmin, double& mmax) {

        for(int i = 0; i < cvRaster.rows; i++)
        {
            const T* mi = cvRaster.ptr<T>(i);
            for(int j = 0; j < cvRaster.cols; j++){
                T v = mi[j];
               *iter = v;
                mmin = Ilwis::min((double)v, mmin);
                mmax = Ilwis::max((double)v, mmax);
                ++iter;
            }

        }
    }
    static bool determineCVType(IlwisTypes valuetype, qint8 &sourcedepth);
    static void createHistogram(Ilwis::PixelIterator rasterIter, cv::SparseMat &histogram, bool accumulate);
};
}
}

#endif // OPENCVHELPER_H
