#ifndef SAMPLESTATISTICS_H
#define SAMPLESTATISTICS_H

namespace Ilwis {

const int MAXCLASS{250};

struct SampleCell {
    enum Marker{mMEAN=0, mSTANDARDDEV=1};

    SampleCell() : _classMean(0), _classStd(0){}
    double _classMean;
    double _classStd;
};

class SampleStats {
public:
    SampleStats() : _nrOfBands(0) {}
    virtual void prepare(IThematicDomain thematicDomain, quint32 nrOfBands) {}
    virtual void addClass(Raw key) = 0;
    virtual void delClass(Raw key) = 0;
    virtual void resetClass(Raw key) = 0;
    virtual void mergeClass(Raw key1, Raw key2) {}
protected:
    quint32 _nrOfBands;
    IThematicDomain _thematicDomain;
};

class SampleHistogram : public SampleStats{
public:
    void prepare(IThematicDomain thematicDomain, const IRasterCoverage &raster);
    void addClass(Raw key);
    void delClass(Raw key);
    void resetClass(Raw key);
    void mergeClass(Raw key1, Raw key2);

    quint32& operator()(Raw raw, quint32 band, quint32 value) { return select(raw, band, value); }

private:
    quint32 &select(Raw key, quint32 band, quint32 value);

private:
    std::vector<std::vector<std::vector<quint32>>> _hist;
    IRasterCoverage _raster;
    std::vector<double> _minValuesperBand;
};

class SampleSum : public SampleStats{
public:
    void prepare(IThematicDomain thematicDomain, quint32 nrOfBands);
    void addClass(Raw key);
    void delClass(Raw key);
    void resetClass(Raw key);
    void mergeClass(Raw key1, Raw key2);
    double pixelInClass(Raw key);
    quint32& operator()(Raw raw, quint32 band1) { return select(raw, band1); }
private:
    quint32& select(Raw raw, quint32 band1);
    std::vector<std::vector<quint32>> _sums;
};

class SampleSumXY : public SampleStats
{
public:
    void prepare(IThematicDomain thematicDomain, quint32 nrOfBands);
    void addClass(Raw key);
    void delClass(Raw key);
    void resetClass(Raw key);
    void mergeClass(Raw key1, Raw key2);
    quint32& operator()(Raw key, quint32 band1, quint32 band2) { return select(key, band1, band2);}
private:
    quint32& select(Raw key, quint32 band1, quint32 band2);
    std::vector<std::vector<std::vector<quint32>>> _sums;

};

class SampleStatistics: public SampleStats
{
public:
    SampleStatistics();
    void prepare(IThematicDomain thematicDomain, quint32 nrOfBands);
    void addClass(quint32 key);
    void delClass(quint32 key);
    void resetClass(Raw key);
    void mergeClass(quint32 key1, quint32 key2, SampleSum &sum, SampleSumXY &sumXY);
    double& operator()(Raw key, quint32 band, SampleCell::Marker val) { return select(key, band, val); }

private:
    double& select(Raw key, quint32 band, SampleCell::Marker val);
    std::vector<std::vector<SampleCell>> _sampleSpace;

};
}

#endif // SAMPLESTATISTICS_H
