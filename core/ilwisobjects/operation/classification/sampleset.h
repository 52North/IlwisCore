#ifndef SAMPLESET_H
#define SAMPLESET_H

namespace Ilwis {
class SampleSet : public Identity
{
public:
    SampleSet();
    void sampleRasterList(const IRasterCoverage& raster);
    void sampleRaster(const IRasterCoverage &raster);
    void thematicDomain(const IThematicDomain& dom);
    void backgroundRaster(const IRasterCoverage &raster);
    bool prepare();
    bool isValid() const;
private:
    IRasterCoverage _background;
    IRasterCoverage _sampleMap;
    IRasterCoverage _sampleMaps;
    IThematicDomain _sampleDomain;
    SampleStatistics _sampleStats;
    SampleHistogram  _sampleHistogram;
    SampleSum _sampleSum;
    SampleSumXY _sampleSumXY;

    std::vector<FeatureSpace> _featureSpaces;



};
}

#endif // SAMPLESET_H
