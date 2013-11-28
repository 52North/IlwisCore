#ifndef FEATUREITERATOR_H
#define FEATUREITERATOR_H

namespace Ilwis {
/**
 * FeatureIterator
 * The primary access method to the features and indexes is through the featureiterator. The
 * featureiterator abstracts the movement through the feature-elements by representing the whole
 * set of feature-elements as one linear set of feature-elements. Within this space a traditional(c++)
 * random access iterator has been defined with all the methods and operators traditionally associated
 * with such a concept. Particularly:
 *
 * - Operators ‘++’and ‘—‘ , moving one step in the data-array.
 * - Operator ‘*’accessing data
 * - Operator ‘[]’ random access
 *
 * Because this mechanism hides the actual organization of feature-elements and indexes, it allows in a
 * large number of cases for the very efficient formulating of algorithms.
 *
 * There are three modes of movement :
 *
 * - Iterate through all feature-elements at the same index level and when done move to the
 * next index level. Or,
 * - Iterate through all feature-elements of a feature and then move to the next feature.
 *
 * The first method is default.
 * The construction of featureiterators is somewhat more complex
 *
 *\sa FeatureCoverage
 */
class KERNELSHARED_EXPORT FeatureIterator  : public std::iterator<std::random_access_iterator_tag, int>
{
public:
    /**
     * @brief FeatureIterator
     */
    FeatureIterator();

    /**
     * @brief FeatureIterator
     * @param fcoverage
     */
    FeatureIterator(const Ilwis::IFeatureCoverage &fcoverage);

    /**
     * @brief FeatureIterator
     * @param fcoverage
     * @param subset
     */
    FeatureIterator(const Ilwis::IFeatureCoverage &fcoverage, const std::vector<quint32>& subset);

    /**
     * @brief FeatureIterator
     * @param iter
     */
    FeatureIterator(const FeatureIterator& iter);

    /**
     * @brief operator ++
     * @return
     */
    FeatureIterator& operator++() ;

    /**
     * @brief operator ++
     * @return
     */
    FeatureIterator operator++(int);

    /**
     * @brief operator +
     * @return
     */
    FeatureIterator& operator+(int);

    /**
     * @brief operator -
     * @return
     */
    FeatureIterator& operator-(int);

    /**
     * @brief operator ==
     * @param rhs
     * @return
     */
    bool operator==(const FeatureIterator& rhs) ;

    /**
     * @brief operator !=
     * @param rhs
     * @return
     */
    bool operator!=(const FeatureIterator& rhs);

    /**
     * @brief operator *
     * @return
     */
    SPFeatureI operator*();

    /**
     * @brief end
     * @return
     */
    FeatureIterator end() const;

    /**
     * @brief end
     * @return
     */
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

inline Ilwis::FeatureIterator begin(const Ilwis::IFeatureCoverage &fcoverage) {
    return Ilwis::FeatureIterator(fcoverage);
}

inline Ilwis::FeatureIterator end(const Ilwis::IFeatureCoverage &fcoverage) {
    Ilwis::FeatureIterator iter(fcoverage);
    return iter.end();
}

#endif // FEATUREITERATOR_H
