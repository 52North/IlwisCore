#ifndef SAMPLESET_H
#define SAMPLESET_H

namespace Ilwis {
class SampleSet : public Identity
{
public:
    SampleSet();
    bool prepare();

    void sampleRasterList(const IRasterCoverage& raster);
    void sampleRaster(const IRasterCoverage &raster);
    void thematicDomain(const IThematicDomain& dom);
    void backgroundRaster(const IRasterCoverage &raster);

    bool isValid() const;

    Raw addClass(const QString &className);
    void deleteClass(Raw raw);
    Raw mergeClass(Raw raw1, Raw raw2);
    void decrementSums(const Pixel &pixel, Raw targetClass);
    void incrementSums(const Pixel &pixel, Raw targetClass);
    void incrementSampels(Raw newClass);
private:
    IRasterCoverage _background;
    IRasterCoverage _sampleMap;
    IRasterCoverage _sampleMaps;
    IThematicDomain _sampleDomain;
    SampleStatistics _sampleStats;
    SampleHistogram  _sampleHistogram;
    SampleSum _sampleSum;
    SampleSumXY _sampleSumXY;

    quint32 _nrOfClasses = 0;

    std::vector<FeatureSpace> _featureSpaces;



    void changeSums(const Pixel &pixel, Raw targetClass, int change);
};
}

#endif // SAMPLESET_H
