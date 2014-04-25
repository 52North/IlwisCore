#ifndef SAMPLESET_H
#define SAMPLESET_H

namespace Ilwis {

class SampleStatistics;
class SampleHistogram;
class SampleSum;
class SampleSumXY;
class FeatureSpace;

typedef std::unique_ptr<SampleStatistics> UPSampleStatistics;
typedef std::unique_ptr<FeatureSpace> UPFeatureSpace;

class KERNELSHARED_EXPORT SampleSet : public Identity
{
public:
    SampleSet();
    SampleSet(const Resource& res);
    SampleSet(const SampleSet& sampleset);
    SampleSet(const IRasterCoverage& samplemaps, const IThematicDomain& dom, const IRasterCoverage& samplemap=IRasterCoverage());
    virtual ~SampleSet();
    bool prepare();
    SampleSet& operator=(const SampleSet& sms);

    void sampleRasterSet(const IRasterCoverage& raster);
    void sampleRaster(const IRasterCoverage &raster);
    void thematicDomain(const IThematicDomain& dom);

    UPSampleStatistics& statistics() ;
    const UPSampleStatistics &statistics() const;
    IRasterCoverage sampleRaster() const;
    IRasterCoverage sampleRasterSet() const;
    IThematicDomain thematicDomain() const;

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
    IRasterCoverage _sampleMap;
    IRasterCoverage _sampleMaps;
    IThematicDomain _sampleDomain;
    std::unique_ptr<SampleStatistics> _sampleStats;
    std::unique_ptr<SampleHistogram>  _sampleHistogram;
    std::unique_ptr<SampleSum> _sampleSum;
    std::unique_ptr<SampleSumXY> _sampleSumXY;

    quint32 _nrOfClasses = 0;

    std::map<quint64, UPFeatureSpace> _featureSpaces;



    void changeSums(const Pixel &pixel, Raw targetClass, int change);
};
}

Q_DECLARE_METATYPE(Ilwis::SampleSet);

#endif // SAMPLESET_H
