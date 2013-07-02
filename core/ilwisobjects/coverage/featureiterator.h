#ifndef FEATUREITERATOR_H
#define FEATUREITERATOR_H

namespace Ilwis {

class KERNELSHARED_EXPORT FeatureIterator  : public std::iterator<std::random_access_iterator_tag, int>
{
public:
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
    SPFeatureI operator*();
    FeatureIterator end() const;
    FeatureIterator end();
private:
    bool init();
    bool move(qint32 distance=1);
    IFeatureCoverage _fcoverage;
    Features::iterator _iterFeatures;
    bool _isInitial;
    std::vector<quint32> _subset;
    quint32 _iterPosition = iUNDEF;
    bool _useVectorIter = true;
};
}

#endif // FEATUREITERATOR_H
