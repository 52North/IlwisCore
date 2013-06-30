#ifndef FEATUREITERATOR_H
#define FEATUREITERATOR_H

namespace Ilwis {

class KERNELSHARED_EXPORT FeatureIterator  : public std::iterator<std::random_access_iterator_tag, int>
{
public:
    enum Flow{fFEATURES, fTRACK, fBYATTRIBUTE};
    FeatureIterator();
    FeatureIterator(const Ilwis::IFeatureCoverage &fcoverage);
    FeatureIterator(const Ilwis::IFeatureCoverage &fcoverage, const std::vector<quint32>& subset);
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
    std::vector<quint32> _subset;
    quint32 _trackCounter;
    Flow _flow;
    FeatureProxy _proxy;
    FeatureProxy _illegal;
};
}

#endif // FEATUREITERATOR_H
