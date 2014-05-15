#ifndef CLASSIFIER_H
#define CLASSIFIER_H

namespace Ilwis {

class SampleSet;

namespace RasterOperations {
class Classifier
{
public:
    Classifier(const Ilwis::SampleSet &sampleset);
    virtual bool classify(PixelIterator& iter, Ilwis::PixelIterator &iterOut) const = 0;
    virtual bool prepare() = 0;

protected:
    const SampleSet& sampleset() const { return _sampleset; }

private:
    const SampleSet& _sampleset;
};

class BoxClassifier : public Classifier{
public:
    BoxClassifier(double factor, const SampleSet& sampleset);

    bool classify(PixelIterator& iter, Ilwis::PixelIterator &iterOut) const;
    bool prepare();
private:


    std::vector<std::vector<double>> _boxMax;
    std::vector<std::vector<double>> _boxMin;
    std::vector<Raw> _classSequence;
    double _widenFactor;

};
}
}

#endif // CLASSIFIER_H
