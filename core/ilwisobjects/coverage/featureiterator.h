#ifndef FEATUREITERATOR_H
#define FEATUREITERATOR_H

namespace Ilwis {

class KERNELSHARED_EXPORT FeatureIterator  : public std::iterator<std::random_access_iterator_tag, int>
{
public:
    enum Flow{fFEATURES, fTRACK, fBYATTRIBUTE};
    FeatureIterator(const Ilwis::IFeatureCoverage &fcoverage);
    FeatureIterator(const Ilwis::IFeatureCoverage &fcoverage, const Box3D<double>& envelope);
    FeatureIterator(const FeatureIterator& iter);
    FeatureIterator& operator++() ;
    FeatureIterator operator++(int);
    FeatureIterator& operator+(int);
    FeatureIterator& operator-(int);
    bool operator==(const FeatureIterator& rhs) ;
    bool operator!=(const FeatureIterator& rhs);
    FeatureInterface& operator*();
    FeatureIterator end() const;
    void flow(Flow f);

private:
    bool init();
    bool move();
    IFeatureCoverage _fcoverage;
    Features::iterator _iterFeatures;
    bool _isInitial;
    Box3D<double> _envelope;
    quint32 _trackCounter;
    Flow _flow;
    FeatureProxy _proxy;
};
}

#endif // FEATUREITERATOR_H
