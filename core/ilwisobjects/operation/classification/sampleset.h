#ifndef SAMPLESET_H
#define SAMPLESET_H

namespace Ilwis {

class SampleStatistics;
class SampleHistogram;
class SampleSum;
class SampleSumXY;
class FeatureSpace;

typedef std::unique_ptr<SampleStatistics> UPSampleStatistics;

class KERNELSHARED_EXPORT SampleSet : public Identity
{
public:
    SampleSet();
    bool prepare();

    void sampleRasterList(const IRasterCoverage& raster);
    void sampleRaster(const IRasterCoverage &raster);
    void thematicDomain(const IThematicDomain& dom);
    void backgroundRaster(const IRasterCoverage &raster);

    UPSampleStatistics& statistics() ;
    IRasterCoverage sampleMap() const;

    bool isValid() const;

    Raw addClass(const QString &className);
    void deleteClass(Raw raw);
    Raw mergeClass(Raw raw1, Raw raw2);
    void decrementSums(const Pixel &pixel, Raw targetClass);
    void incrementSums(const Pixel &pixel, Raw targetClass);
    void incrementSampels(Raw newClass);

    void registerFeatureSpace(FeatureSpace *fs);
    void unregisterFeatureSpace(quint64 id);


private:
    IRasterCoverage _background;
    IRasterCoverage _sampleMap;
    IRasterCoverage _sampleMaps;
    IThematicDomain _sampleDomain;
    std::unique_ptr<SampleStatistics> _sampleStats;
    std::unique_ptr<SampleHistogram>  _sampleHistogram;
    std::unique_ptr<SampleSum> _sampleSum;
    std::unique_ptr<SampleSumXY> _sampleSumXY;

    quint32 _nrOfClasses = 0;

    std::map<quint64, std::unique_ptr<FeatureSpace>> _featureSpaces;



    void changeSums(const Pixel &pixel, Raw targetClass, int change);
};
}

#endif // SAMPLESET_H
